/* Sensor -*- C++ -*- */
#ifndef _SENSOR_H_
#define _SENSOR_H_

#include <math.h>
#include <cmath>
#include <vector>
#include <set>
#include "common.h"
#include "Point.h"
#include "Image.h"

#define SENSOR_GAUSS_NORMALIZATION_RANGE        2

class SensorCollection;
class Dataset;
class ZeroSensor;
sensor_ptr getZeroSensor();

/** Sensor calculates filter values from the source. */
class Sensor : public std::enable_shared_from_this<Sensor> {
 protected:
    Sensor();
 public:
    enum  UpdateParam {UPD_NOTHING=0, UPD_DATA=1, 
		       UPD_SCALE=2, UPD_CWEIGHTS=4, UPD_DIR=8, 
		       UPD_MINMAX=16,
		       UPD_LAST=32, UPD_ALL=0xffffffff 
    };
    /* default constructor is protected */
    /* constructor 
     \param _source if NULL getZeroSensor() will be used
                   other Sensors or Dataset is possible 
    */
    Sensor(sensor_cptr _source);
    /** Destructor */
    virtual ~Sensor();

    sensor_cptr getSource() {return source;}
    virtual void changeSource(sensor_cptr _source);
    void replaceBy(sensor_ptr target);
    bool assignRef(sensor_cptr rhs);
    virtual Sensor& assign(const Sensor& rhs);

    //! get value at discrete position
    virtual float getValue(int x, int y) const {
	return calcValue( x, y);
    }
    //! get value at position p using nearest neighbour interpolation
    float getValue(const Point &p) const {
	return getValue((int)p.x, (int)p.y);
    }
    //! get multi-channel value
    virtual std::vector<float> getMValue(int x, int y) const {
	return calcMValue(x,y);
    }
    //! get gradient at discrete position
    virtual Point2D getGradient(int x, int y) const {
	return calcGradient(x,y);
    }
    //! get gradient at position p using nearest neighbor interpolation
    Point2D getGradient(const Point2D &p) const {
// 	float v = getValue(p);
// 	return Point2D(getValue(p+Point2D(1,0))-v,
// 		       getValue(p+Point2D(0,1))-v);
	return getGradient((int)p.x, (int)p.y);
    };
    float getMax() const {
      if(isModified()) ((Sensor*)this)->performUpdate();
      return maxval;
    }
    float getMin() const {
      if(isModified()) ((Sensor*)this)->performUpdate();
      return minval;
    }
    float getMinMaxRange() const {
      if(isModified()) ((Sensor*)this)->performUpdate();
      return maxval-minval;
    }
    //! get max normalized value at discrete position range [0,1]
    float getWeightedValue(int x, int y) const {
#define _USE_MINMAX_NORMALIZATION_
#ifdef _USE_MINMAX_NORMALIZATION_
      float range = getMinMaxRange();
      if(range==0) return 0.f;
      return (getValue(x,y)-minval)/range;
#else
      float range = stdev*SENSOR_GAUSS_NORMALIZATION_RANGE;
      if(range==0) range = 1;
      float value = (getValue(x,y)-minval)/range;
      return value > 1.f ? 1.f : value; //(value < 0.f ? 0.f : value);
#endif
    }
    //! get range weighted gradient at discrete position
    Point2D getWeightedGradient(int x, int y) const {
      float range = getMinMaxRange();
      if(range==0) range = 1;
      return getGradient(x,y)/range;
    }
  
    void setCWeights(const std::vector<float>& weights);
    const std::vector<float>& getCWeights() const { return cweights; }
    void setDirection(const Point &dir);
    const Point& getDirection() const { return dir; }
    void setScale(float _scale);
    float getScale() const { return scale; }
  
    /** Check whether something is marked as modified.  enum
	Sensor::UpdateParam is used. Also, mask is filtered through
	internal updateMask. So only the properties that matter to the
	current sensor will take effect.
     */
    bool isModified(dword mask=UPD_ALL) const 
	{ return (toupdate & (mask & updateMask)) != 0; }
    void setModified(dword mask=UPD_ALL) { toupdate |= mask; }
    void unsetModified(dword mask=UPD_ALL) { toupdate &= ~mask; }
    bool isUpdate(dword udMask) const { return updateMask & udMask; }
    void enableUpdate(dword udMask) { updateMask |= udMask; }
    void disableUpdate(dword udMask) { updateMask &= ~udMask; }

    /** Perform an update if isModified() */
    virtual bool performUpdate();

/** the real dataset will override these */
    virtual int getDim1Size() const { return source->getDim1Size(); }
    virtual int getDim2Size() const { return source->getDim2Size(); }
    virtual int getDim3Size() const { return source->getDim3Size(); }
    virtual int getNChannels() const { return source->getNChannels(); }
    virtual int getSkip() const { return m_Skip; }
    bool isValid(int x, int y) const;
    virtual Image<float> createSensorImage() const;

    const std::string& getID() const { return m_ID; }
    void setID(const std::string& id) { m_ID = id; }

    static dword getStringNumber(const char *sid);
    static void getNumberString(char sid[5], dword id);

    virtual std::ostream& hprint(std::ostream &os, SensorCollection *sc) const;
    virtual std::ostream& print(std::ostream &os) const;

 protected:
    /** Compute a sensed value at position x,y.
	This function is essential for all derived sensors.
     */
    virtual float calcValue(int x, int y) const = 0;
    /** Calculate a multi-valued sensor result. 
	The size of the returned vector is the same as getNChannels().
     */
    virtual std::vector<float> calcMValue(int x, int y) const {
	return std::vector<float>(getNChannels(), calcValue(x,y));
    }
    /** Calculates the gradient using central differencing. */
    virtual Point calcGradient(int x, int y) const {
	float v = getValue(x,y);
	return Point2D(getValue(x+1,y)-v,
		       getValue(x,y+1)-v);
    }

  virtual void calcMinMax();

public:
    void refSuperSensor(sensor_ptr super);
    void unrefSuperSensor(sensor_ptr super);
    void invalidateSource();
    template <typename Derived>
    std::shared_ptr<Derived> shared_from_base()
    {
        return std::static_pointer_cast<Derived>(shared_from_this());
    }

protected:
    //------------------------- data ----------------
    /** Handle of the source */
    sensor_cptr source;
    /* parameters */
    /** Scale of filter (log2 scale --> 0 means 'full' resolution) */
    float scale;
    std::vector<float> cweights;	//!< multi-channel weights ('color')
    Point dir;                          //!< direction parameter
    float maxval, minval;               //!< overall minimum and maximum
    float mean, stdev;                  //!< overall mean and stdev

    dword toupdate;	//!< bitflag for updates
    dword updateMask;   //!< bitflag to mask unimportant updates
    std::string m_ID;
    int   m_Skip;
    int   m_AddSkip;
private:
    std::set<sensor_ptr> superSensors;	//!< references to attached sensors
};

template<typename SensorType, typename FirstArgType, typename... Args>
std::shared_ptr<SensorType> makeSensor(FirstArgType source, Args... args)
{
    std::shared_ptr<SensorType> s = std::make_shared<SensorType>(getZeroSensor(), args...);
    s->changeSource(source);
    return s;
}

template<typename SensorType>
std::shared_ptr<SensorType> makeSensor()
{
    return std::make_shared<SensorType>();
}

/** a sensor that does nothing except returning zeros */
class ZeroSensor : public Sensor {
public:
    ZeroSensor() : Sensor()
    {
        updateMask = UPD_NOTHING;
        unsetModified();
        setID("0");
    }
    void changeSource(sensor_cptr source) {};
    int getDim1Size() const { return 2; }
    int getDim2Size() const { return 2; }
    int getDim3Size() const { return 1; }
    int getNChannels() const { return 1; }
    float getWeightedValue(int x, int y) const { return 0; }
    Point2D getWeightedGradient(int x, int y) const { return Point(); }
    std::ostream& print(std::ostream &os) const 
        { return Sensor::print(os)<<"o"; }
    
protected:
    //! Computes a zero.
    float calcValue(int x, int y) const { return 0.0f; }
    Point calcGradient(int x, int y) const { return Point(0,0); }
};

/** Sensor with preprocessing */
class PPSensor : public Sensor {
 public:
    enum PPState { PP_DONT=0, PP_DO, PP_FORCE };
    PPSensor(sensor_cptr _source);
    /** Return cached value or compute and cache new one. */
    virtual float getValue(int x, int y) const;
    /** Return cached gradient or compute and cache new one. */
    virtual Point getGradient(int x, int y) const;
    /** clear all data sheets */
    virtual bool performUpdate();
    Sensor& assign(const Sensor& rhs); 
    /** Tell how strict precomputation will be done. 
	PPState::PP_DO will compute and cache on the fly. 
	PPState::PP_FORCE will compute entire caching sheet.
    */
    void togglePP(enum PPState state=PP_DO) { doPP = state; }
    dword getPPState() const { return doPP; }
protected:
    void fitSheets();
    virtual void calcAllValues();
    virtual void calcAllGradients();
    //virtual const Image<float> createSensorImage() const;

private:
    /** do complete preprocessing */
    bool performFullUpdate();

 protected:
    Image<float> values;		//!< caching sheet for sensed values
    Image<Point> gradients;		//!< caching sheet for gradients
    dword doPP;				//!< do preprocessing?
};

//-----------------------------------------------------------------------------
//inline functions

//--------------------------------- PPSensor

inline float PPSensor::getValue(int x, int y) const {
    if(doPP) {
	if(isModified()) const_cast<PPSensor*>(this)->performUpdate();
	int index = (int)values.getBoundedIndex(x,y);
	if(index==-1) return 0;
	float v;
	if(std::isnan((v = values[index]))) {
	    if(doPP == PPSensor::PP_FORCE) {
		assert(false); //no NAN values in forced full update at this point
		const_cast<PPSensor*>(this)->performFullUpdate();
		return values[index];
	    } else {
		return (const_cast<PPSensor*>(this)->values[index] = calcValue(x,y));
	    }		
	} else return v;
    } else return calcValue(x,y);
}

inline Point PPSensor::getGradient(int x, int y) const {
    if(doPP) {
	if(isModified()) const_cast<PPSensor*>(this)->performUpdate();
	int index = gradients.getBoundedIndex(x,y);
	if(index<0) return Point();
	if(std::isnan(gradients[index].x)) {  // no precalculated value?
// 	    if(doPP == PP_FORCE) {
// 		//this should not happen
// 		assert(false);
// 		const_cast<PPSensor*>(this)->performFullUpdate();
// 		return gradients[index];
// 	    } else 
	    return (const_cast<PPSensor*>(this)->gradients[index] 
		    = calcGradient(x,y));
	} else return gradients[index];
    } else return calcGradient(x,y);
}

#endif
