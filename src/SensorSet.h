/* Sensor -*- C++ -*- */
#ifndef _SENSORSET_H_
#define _SENSORSET_H_

#include <math.h>
#include <complex>
#include "Sensor.h"
#include "Fourier.h"
#include "DMatrix.h"

//usable Sensor implementations

/** Intensity sensor (identity operator) */
class IntensitySensor : public Sensor {
public:
    std::ostream& print(std::ostream& os) const {
        Sensor::print(os) << "i";
        return os;
    }
protected:
    /** Returns the Intensity */
    float calcValue(int x, int y) const {
        return source->getValue(x,y);
    }
};

/** Intensity sensor (identity operator) */
class PPIntensitySensor : public PPSensor {
protected:
    /** Returns the Intensity */
    float calcValue(int x, int y) const {
        return source->getValue(x,y);
    }
};

/** Multi-channel intensity Sensor **/
class MCIntensitySensor : public PPSensor {
public:
    int getNChannels() const { return 1; }
    std::ostream& print(std::ostream& os) const {
        Sensor::print(os) << "m ";
        for(dword i=0; i<cweights.size(); i++) { //source->getNChannels()
            if(i) os << " ";
            os << cweights[i];
        }
        return os;
    }
protected:
    //! Computes a scalar product between the multi-channel intensitis and the cweights vector.
    float calcValue(int x, int y) const {
        float result = 0.0f;
        const std::vector<float> mv = source->getMValue(x,y);
        assert(mv.size() == cweights.size());
        for(dword i=0; i<mv.size(); i++) { //source->getNChannels()
            float weight = cweights[i];
            if(weight!=0.0f) result += mv[i]*weight;
        }
        return result;
    }
};

/** Multi-channel gradient Sensor **/
class MCGSensor : public PPSensor {
public:
    MCGSensor(sensor_cptr _source=NULL)
        { m_AddSkip = 1; }
    int getNChannels() const { return 1; }
    std::ostream& print(std::ostream& os) const {
        Sensor::print(os)
            << "M";
        return os;
    }
protected:
    //! Computes a scalar product between the multi-channel intensities and the cweights vector.
    float calcValue(int x, int y) const {
        float result = 0.0f;
        const std::vector<float> mv = source->getMValue(x,y);
        for(std::vector<float>::const_iterator v=mv.begin();
            v != mv.end(); v++)
            result+=*v;
        return result;
    }
    virtual Point calcGradient(int x, int y) const {
        std::vector<float> c = source->getMValue(x,y);
        std::vector<float> r = source->getMValue(x+1,y);
        std::vector<float> b = source->getMValue(x,y+1);
        Point2D g(0,0);
        for(int i=0; i<source->getNChannels(); i++)
        {
            register float dx = r[i]-c[i];
            g.x += dx*dx;
            register float dy = b[i]-c[i];
            g.y += dy*dy;
        }
        g.x = sqrt(g.x);
        g.y = sqrt(g.y);
        return g;
    }
};

/** Multi-chrominance Sensor **/
class CRSensor : public PPSensor {
public:
    CRSensor()
        { m_AddSkip = 1; }
    int getNChannels() const { return 1; }
    std::ostream& print(std::ostream& os) const {
        Sensor::print(os)
            << "r";
        return os;
    }
protected:
    //! Computes saturation or some other colour indicator
    float calcValue(int x, int y) const {
        float result = 0.0f;
        std::vector<float> mv = source->getMValue(x,y);
#define SATURATION
#ifdef CHROMITHING
        float g = 0;
        for(std::vector<float>::const_iterator v=mv.begin();
            v != mv.end(); v++)
            g+=*v;
        g/=mv.size();
        for(std::vector<float>::const_iterator v=mv.begin();
            v != mv.end(); v++)
        {
            register float d = g-*v;
            result += d*d;
        }
        return sqrt(result);
#elif defined(SATURATION)
        float mi = std::numeric_limits<float>::max();
        float ma = std::numeric_limits<float>::min();
        for(std::vector<float>::const_iterator v=mv.begin();
            v != mv.end(); v++)
        {
            if(*v>ma) ma = *v;
            if(*v<mi) mi = *v;
        }
        return ma>0.00001 ? (ma-mi)/ma : 0;
#endif

#undef SATURATION
#undef CHROMITHING
    }
};

/** Smoothed intensity sensor (using gaussian) */
class SmoothIntensitySensor : public PPSensor {
public:
    SmoothIntensitySensor(float _scale=0);
    bool performUpdate();
    //dword getFilterSize() const {return smoothflt.getSizeX();}
    std::ostream& print(std::ostream& os) const {
        Sensor::print(os)
            << "g " << getScale();
        return os;
    }

protected:
    void calcAllValues();
    float calcValue(int x, int y) const {
        return source->getValue(x,y);
    }
    void updateScale();
protected:
    Fourier2D                           m_FT, m_FTflt;
    Image< double >                     m_SData, m_SFilter;
    Image< std::complex<double> >       m_FData, m_FFilter;
};

/** gradient magnitude Sensor **/
class GradMagSensor : public PPSensor {
public:
    GradMagSensor()
        {
            enableUpdate(UPD_DATA);
            togglePP(PPSensor::PP_FORCE);
            m_AddSkip = 1;
            performUpdate();
        };
    bool performUpdate() {
        if(isModified(UPD_DATA)) {
            m_Skip = source->getSkip()+m_AddSkip;
        }
        return PPSensor::performUpdate();
    }
    std::ostream& print(std::ostream& os) const {
        Sensor::print(os)
            << "d";
        return os;
    }
protected:
    /** Compute gradient magnitude */
    float calcValue(int x, int y) const {
        if(!isValid(x,y)) return 0;
        else return source->getGradient(x,y).norm();
    }
};

/** curvature Sensor **/
class CornerSensor : public PPSensor {
public:
    CornerSensor()
        {
            enableUpdate(UPD_DATA);
            togglePP(PPSensor::PP_FORCE);
            m_AddSkip = 3;
        };

    bool performUpdate() {
        if(isModified(UPD_DATA)) {
            m_Skip = source->getSkip()+m_AddSkip;
        }
        return PPSensor::performUpdate();
    }
    std::ostream& print(std::ostream& os) const {
        Sensor::print(os)
            << "c";
        return os;
    }
protected:
    /** Compute cornerness */
    float calcValue(int x, int y) const {
        if(!isValid(x,y)) return 0;
        float value = source->getValue(x,y);
        if(value==0) return 0;
        Point c=source->getGradient(x,y);
        Point ddx = source->getGradient(x+1,y)-c;
        Point ddy = source->getGradient(x,y+1)-c;
//#define SS_GRADIENT_WEIGHTED_DETERMINANT
#ifdef SS_GRADIENT_WEIGHTED_DETERMINANT
        float n1 = ddx.normalize();
        float n2 = ddy.normalize();
        float det = ddx.x*ddy.y-ddx.y*ddy.x;
        det *= min(n1,n2);
#else
        float det = ddx.x*ddy.y-ddx.y*ddy.x;
#endif
        if(det<0) return 0; // repelling regions are no good
        float ret=det*value;
        return ret;
/* using eigenvalues
//cout << det;
//cout << ddx << endl << ddy << endl;
float p = (ddx.x+ddy.y)/2;
float d = sqrt(p*p-ddx.x*ddy.y+ddx.y*ddy.x);
//cout << "p=" << p << " d=" << d<<endl;
float e1 = p+d;
float e2 = p-d;
//cout << "e1="<<e1<<" e2="<<e2<<endl;
//cout << "---------------------" << endl;
return e1*e2;
*/
    }
};

/** Multi-Sensor */
class CombiSensor : public PPSensor {
public:
    CombiSensor(int nchannels=0);
    virtual ~CombiSensor();

//virtual functions
    int getNChannels() const { return sources.size(); }
    int getSkip() const {
        int m_Skip = 0;
        for(std::vector<sensor_ptr>::const_iterator s = sources.begin();
            s!=sources.end(); s++)
            if((*s)->getSkip() > m_Skip) m_Skip = (*s)->getSkip();
        return m_Skip;
    }

    void changeSource(sensor_cptr _source);
    std::ostream& print(std::ostream& os) const;
    std::ostream& hprint(std::ostream &os, SensorCollection *sc) const;
//member functions
    void clearSources();
    void setNSources(int n);
    void setSource(sensor_ptr _source, float weight, int id);
    void setSource(sensor_ptr _source, float weight)
        { setSource(_source, weight, sources.size()); }
    Sensor& assign(const Sensor& rhs);
    void normalizeInput(bool doit=true) { m_NormalizeInput = doit; }
    bool isInputNormalized() const { return m_NormalizeInput; }
protected:
    std::vector<sensor_ptr> sources;
    bool                    m_NormalizeInput;

    //! Computes a scalar product between the multi-channel intensities and the cweights vector.
    float calcValue(int x, int y) const {
        float result = 0.0f;
        std::vector<sensor_ptr>::const_iterator s = sources.begin();
        std::vector<float>::const_iterator w = cweights.begin();
        if(m_NormalizeInput)
            for(; s != sources.end(); s++,w++) {
                result += (*s)->getWeightedValue(x,y)*(*w);
            }
        else
            for(; s != sources.end(); s++,w++) {
                result += (*s)->getValue(x,y)*(*w);
            }
        return result;
    }

    std::vector<float> calcMValue(int x, int y) const {
        std::vector<float> vv(getNChannels());
        std::vector<float>::iterator v = vv.begin();
        std::vector<sensor_ptr>::const_iterator s = sources.begin();
        std::vector<float>::const_iterator w = cweights.begin();
        if(m_NormalizeInput)
            for(; v != vv.end(); v++, s++,w++)
                *v = (*s)->getWeightedValue(x,y)*(*w);
        else
            for(; v != vv.end(); v++, s++,w++)
                *v = (*s)->getValue(x,y)*(*w);
        return vv;
    }

    //void calcMinMax();
};

/** Mahalanobis distance Sensor **/
class MahalSensor : public PPSensor {
public:
    MahalSensor(const std::string& fname="")
        {
            m_AddSkip = 1;
            loadConfig(fname);
        }
    int getNChannels() const { return 1; }
    Sensor& assign(const Sensor& rhs);
    std::ostream& print(std::ostream& os) const {
        Sensor::print(os)
            << "C " << m_Filename;
        return os;
    }
    bool loadConfig(const std::string& fname = "");
    bool saveConfig(const std::string& fname = "") const;
    void setCovi(const DMatrix<float>& icov) { this->icov = icov; }
    void setMean(const DMatrix<float>& mean) { this->m = mean; }
    const DMatrix<float>& getICov() const { return icov; }
    const DMatrix<float>& getMean() const { return m; }
    const std::string& getFilename() const { return m_Filename; }
protected:
    //! Computes saturation or some other colour indicator
    float calcValue(int x, int y) const {
        std::vector<float> mv = source->getMValue(x,y);
        DMatrix<float> v(1,std::min(mv.size(),size_t(3)),&mv.front());
        v -= m;
        DMatrix<float> vt(v); vt.transpose();
        float d = (vt.mulRight(icov.mulRight(v))).at(0,0);
        return exp(-0.5*d);
    }
    DMatrix<float> m, icov;
    mutable std::string m_Filename;
};

/** Sensor performing various mappings */
class MappingSensor : public PPSensor {
public:
    enum MappingTypes {MS_IDENTITY, MS_CLAMPLU, MS_CLAMPL, MS_CLAMPU,
                       MS_GAUSSNORM, MS_MGAUSSNORM, MS_MCLAMPU, MS_BIAS,
                       MS_LAST};

    MappingSensor(const std::string& mapn="")
        : m_MappingID()
        {
            m_AddSkip = 1;
            setMapping(mapn);
        }
    int getNChannels() const { return 1; }
    Sensor& assign(const Sensor& rhs);
    std::ostream& print(std::ostream& os) const;
    const char* getMappingName() const
        { return s_MappingNames[m_MappingID]; }
    dword getMappingID() const { return m_MappingID; }
    dword setMappingID(dword id);
    dword setMapping(const std::string& mname);
    bool readParams(std::istream& is);
    bool performUpdate();
protected:
    static const char* s_MappingNames[];
    //! Computes saturation or some other colour indicator
    virtual float calcValue(int x, int y) const {
        float v = source->getValue(x,y);
        switch(m_MappingID) {
//             case MS_IDENTITY:
//                 break;
        case MS_CLAMPL:
            if(v < m_Param[0]) v = m_Param[0];
            break;
        case MS_CLAMPU:
            if(v > m_Param[0]) v = m_Param[0];
            break;
        case MS_CLAMPLU:
            if(v < m_Param[0]) v = m_Param[0];
            else if(v > m_Param[1]) v = m_Param[1];
            break;
        case MS_MCLAMPU:
            if(v > m_Param[1]) v = m_Param[1];
            break;
        case MS_GAUSSNORM:
            v = (v-m_Param[1])*m_Param[2]+0.5;
            break;
        case MS_MGAUSSNORM:
            if(v>0.f) v = (v-m_Param[1])*m_Param[2];
            break;
        case MS_BIAS:
            //if(v>0.f) {
            v = (v-m_Param[1])*m_Param[2];
            v = erf(v)*0.5+0.5;
            //} else v = 0.f;
            break;
        }
        return v;
    }

protected:
    dword               m_MappingID;
    std::vector<float>  m_Param;
};


#endif
