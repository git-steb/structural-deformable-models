#ifdef WIN32
#include <windows.h>
#endif
#include <typeinfo>
#include <math.h>
#include "Sensor.h"
#include "SensorColl.h"
#include "airnan.h"
#include <string.h>

using namespace std;

Sensor::Sensor()
    : source(),
      scale(0.f),
      maxval(), minval(), mean(), stdev(),
      toupdate(), updateMask((dword)0),
      m_ID(""), m_Skip(0), m_AddSkip(0)
{ }

Sensor::Sensor(sensor_cptr _source)
    : source(getZeroSensor()),
	  scale(0.f),
	  maxval(), minval(), mean(), stdev(),
	  toupdate(), updateMask((dword)UPD_DATA),
      m_ID(""), m_Skip(0), m_AddSkip(0)
{
    cweights.resize(1, 1.f);
    if(_source && _source != getZeroSensor() )	{
        throw logic_error("Only call the sensor constructor with ZeroSensor source");
        changeSource(_source);
    }
    setModified();
}

Sensor::~Sensor()
{
    if(source && source != getZeroSensor())
        std::const_pointer_cast<Sensor>(source)->unrefSuperSensor( shared_from_this() );
    for(set<sensor_ptr>::iterator ss = superSensors.begin();
        ss != superSensors.end();
        ss++)
            (*ss)->invalidateSource();
}

void Sensor::replaceBy(sensor_ptr target) {
    for(set<sensor_ptr>::iterator ss = superSensors.begin();
	ss != superSensors.end();) 
    {
        set<sensor_ptr>::iterator last = ss;
        ss++;
        (*last)->changeSource(target);
    }
}

Sensor& Sensor::assign(const Sensor& rhs)
{ 
    if(&rhs != this) { 
        if(source != rhs.source) changeSource(rhs.source);
        if(scale != rhs.scale) setScale(rhs.scale);
        if(dir != rhs.dir) setDirection(rhs.dir);
        if(cweights != rhs.cweights) setCWeights(rhs.cweights);
        m_ID = rhs.m_ID;
        m_Skip = rhs.m_Skip;
        m_AddSkip = rhs.m_AddSkip;
        //superSensors are not changed here (no sensor fusion)
    } 
    return *this; 
}

bool Sensor::assignRef(sensor_cptr rhs) {
    if(typeid(*this) != typeid(*rhs)) return false;
    assign(*rhs);
    return true;
}

dword Sensor::getStringNumber(const char *sid) {
    if(!sid) return 0;
    else {
	dword val;
	int i;
	for(i=0, val=0; i<4 && sid[i]!=0; i++) {
	    if(sid[i]>32)
		val |= ((dword)(sid[i]))<<(8*i);
	}
	return val;
    }
}

void Sensor::getNumberString(char sid[5], dword id)
{
    int i;
    for(i=0;i<4 && id!=0;i++,id=id>>8)
	sid[i] = char(id);
    sid[i] = 0;
}

void Sensor::changeSource(sensor_cptr _source) {
    assert(source);
    std::const_pointer_cast<Sensor>(source)->unrefSuperSensor( shared_from_this() );
    if(!_source)
        source = getZeroSensor();
    else 
        source = _source;
    if(cweights.size() != size_t(source->getNChannels()))
        cweights.resize(source->getNChannels(), 1.f);
    std::const_pointer_cast<Sensor>(source)->refSuperSensor( shared_from_this() );
    setModified(UPD_DATA);
}
    
void Sensor::setCWeights(const vector<float>& weights) {
    //if((int)weights.size() == source->getNChannels()) {
	cweights = weights;
	setModified(UPD_CWEIGHTS);
    //}
}
void Sensor::setDirection(const Point &dir) {
    this->dir = dir;
    setModified(UPD_DIR);
}
void Sensor::setScale(float _scale) { 
    scale = _scale; 
    setModified(UPD_SCALE);
}
bool Sensor::performUpdate()  {
    bool modified = false;
    if(isModified()) {
	if(isModified(UPD_DATA)) {
            m_Skip = source->getSkip()+m_AddSkip;
	    //if(cweights.size() != source->getNChannels()) {
            //	cweights.resize(source->getNChannels(),1.f);
            //modified = true;
	    //}
	}
	unsetModified();
	if(updateMask&UPD_MINMAX) {
	  calcMinMax();
	}
	// do nothing for this sensor, but for the others...
	for(set<sensor_ptr>::iterator ss = superSensors.begin();
	    ss != superSensors.end(); ss++) 
	{
	  (*ss)->setModified(Sensor::UPD_DATA);
	}
	for(set<sensor_ptr>::iterator ss = superSensors.begin();
	    ss != superSensors.end(); ss++) 
	{
	  (*ss)->performUpdate();
	}
    } 
    return modified;  // no modification to *this
}

void Sensor::calcMinMax() {
  if(!source) {
    minval = maxval = 0;
    mean = stdev = 0;
    return;
  }
  int i,j;
  minval=numeric_limits<float>::max();
  maxval=numeric_limits<float>::min();
  double avg=0, avg2=0;
  double size = double(source->getDim1Size()*source->getDim2Size());
  if(size > 0.) {
      for(j=0; j<source->getDim2Size(); j++) {
          for(i=0; i<source->getDim1Size(); i++) {
              float val = getValue(i,j);
              if(val>maxval) maxval = val;
              if(val<minval) minval = val;
              avg+=double(val);
              avg2+=double(val*val);
          }
      }
      avg  /= size;
      avg2 /= size;
  } else {
      minval = maxval = 0;
  }
  mean = float(avg);
  stdev = float(sqrt(avg2-avg*avg))*1.5;
}

void Sensor::refSuperSensor(sensor_ptr super) {
    if(super != getZeroSensor())
        superSensors.insert(super);
}
void Sensor::unrefSuperSensor(sensor_ptr super) {
    if(super != getZeroSensor())
        superSensors.erase(super);
}
void Sensor::invalidateSource() {
    if(source && source != getZeroSensor()) {
        std::const_pointer_cast<Sensor>(source)->unrefSuperSensor( shared_from_this() );
        source = getZeroSensor();
    }
}

bool Sensor::isValid(int x, int y) const 
{
    return x>=m_Skip && x<getDim1Size()-m_Skip 
        && y>=m_Skip && y<getDim2Size()-m_Skip;
}

Image<float> Sensor::createSensorImage() const {
    Image<float> img(getDim1Size(), getDim2Size());
    int i,j;
    for(j=0;j<getDim2Size();j++)
	for(i=0;i<getDim1Size();i++) {
	    img.setPixel(i,j, getWeightedValue(i,j));
	}
    return img;
}

ostream& Sensor::print(ostream &os) const
{
    os << "s " << m_ID << " ";
    if(source && source->getID() != "d0" && source != getZeroSensor()) {
        os << "source " << source->getID() << " ";
    }
    return os;
}

ostream& Sensor::hprint(ostream &os, SensorCollection *sc) const
{
    if(sc->isPrinted(m_ID)) return os;
    if(source && source != getZeroSensor()) source->hprint(os, sc);
    if(sc->isPrinted(m_ID)) return os;
    sc->setPrinted(m_ID);
    return (print(os) << endl);
}

//-----------------------------------------------------------------------------
//class ZeroSensor

sensor_ptr getZeroSensor()
{
    static sensor_ptr zeros = std::make_shared<ZeroSensor>();
    return zeros;
}

//-----------------------------------------------------------------------------
//class PPSensor
PPSensor::PPSensor(sensor_cptr _source) 
    : Sensor(_source), 
      values(source->getDim1Size(), source->getDim2Size(), AIR_NAN),
      gradients(source->getDim1Size(), source->getDim2Size(), 
                Point(AIR_NAN,AIR_NAN)),
      doPP(PPSensor::PP_DONT)
{}

void PPSensor::fitSheets() {
    if(values.getSizeX() != source->getDim1Size() ||
       values.getSizeY() != source->getDim2Size()) {
	values.setSize(source->getDim1Size(), source->getDim2Size());
	gradients.setSize(source->getDim1Size(), source->getDim2Size());
    }
    //this is also done later on by Sensor::performUpdate, 
    //but we might need it earlier
    //if(cweights.size() != source->getNChannels()) {
//	cweights.resize(source->getNChannels());
//    }
}

void PPSensor::calcAllValues() {
    if(values.getSizeX() == source->getDim1Size() &&
       values.getSizeY() == source->getDim2Size()) 
    {
	int i,j;
	Image<float>::iterator v=values.begin();
	for(j=0; j<source->getDim2Size(); j++) {
	    for(i=0; i<source->getDim1Size(); i++, v++)
	    {
		*v = calcValue(i,j);
		assert(!isnan(*v));
	    }
	}
    }
}

void PPSensor::calcAllGradients() {
    if(gradients.getSizeX() == source->getDim1Size() &&
       gradients.getSizeY() == source->getDim2Size())
    {
	int i,j;
	Image<Point>::iterator g=gradients.begin();
	for(j=0; j<source->getDim2Size()-1; j++, g++) {
	    for(i=0; i<source->getDim1Size()-1; i++, g++) {
		*g = calcGradient(i,j);
	    }
	}
    }
}

bool PPSensor::performFullUpdate()
{
    if(isModified()) {
	fitSheets();
	dword toupdatesave = toupdate;
	unsetModified();	// otherwise getValue reenters performUpdate
	calcAllValues();
	calcAllGradients();
	setModified(toupdatesave);
	Sensor::performUpdate();
	return true;
    } else return false;
}

bool PPSensor::performUpdate()
{
    if(isModified()) {
	if(doPP == PPSensor::PP_FORCE) return performFullUpdate();
	else {
	    fitSheets();
	    for(Image<float>::iterator v=values.begin(); v!=values.end(); v++) {
		*v = AIR_NAN;
	    }
	    for(Image<Point>::iterator g=gradients.begin(); g!=gradients.end(); g++) {
		g->x = AIR_NAN;
	    }
	    Sensor::performUpdate();
	    return true;
	}
    }else return false;
}

Sensor& PPSensor::assign(const Sensor& rhs)
{ 
    if(&rhs != this) { 
        Sensor::assign(rhs);
        if(typeid(&rhs) == typeid(PPSensor*)) {
            const PPSensor& crhs = (const PPSensor&) rhs;
            doPP = crhs.doPP;
        }
    } 
    return *this;
}

/*
const Image<float> PPSensor::createSensorImage() const
{
    dword sPP = doPP;
    ((PPSensor*)this)->doPP = PPSensor::PP_FORCE;
    ((PPSensor*)this)->performUpdate();
    ((PPSensor*)this)->doPP = sPP;
    return Image<float>(values);
}
*/

