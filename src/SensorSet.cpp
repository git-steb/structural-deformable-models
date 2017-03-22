#ifdef WIN32
#include <windows.h>
#endif
#include <iostream>
#include <fstream>
#include <typeinfo>
#include "SensorSet.h"
#include "SensorColl.h"
#include "mathutil.h"
#include "Fourier.h"
#include "DMatrixUtil.h"
using namespace std;

static
Image<double>& create2DGaussian(Image<double>& flt, double stdev, 
                                float clampd, float cx, float cy);

//-----------------------------------------------------------------------------
//class SmoothIntensitySensor
SmoothIntensitySensor::SmoothIntensitySensor(float _scale)
{
    enableUpdate(UPD_SCALE|UPD_DATA);
    togglePP(PP_FORCE);
    setScale(_scale);
}

bool SmoothIntensitySensor::performUpdate() {
    if(!isModified()) return false;
    if(source->getDim1Size()>0) {
        if(m_SData.getSizeX() != source->getDim1Size() ||
           m_SData.getSizeY() != source->getDim2Size()) {
            m_SData.setSize(source->getDim1Size(), source->getDim2Size());
            m_FData.setSize(Fourier2D::complexX(source->getDim1Size())/2,
                            source->getDim2Size());
            m_FT.initTransform(m_SData.getSizeX(), m_SData.getSizeY(),
                               m_SData.getData(), (double*)m_FData.getData());
        }
        updateScale();
    }
    return PPSensor::performUpdate();
}

void SmoothIntensitySensor::calcAllValues() 
{
    PPSensor::calcAllValues();
    if(m_FFilter.initialized() && m_FData.initialized()) {
        m_SData.convertFrom(values);
        m_FT.transform();
        m_FData *= m_FFilter; 
        m_FT.transformInv();
        values.convertFrom(m_SData);
        //values = values.convolve(m_SFilter);
    }
}

void SmoothIntensitySensor::updateScale() {
    assert(source->getDim1Size()>0);
    bool nosizematch = !(m_SFilter.getSizeX() == source->getDim1Size() &&
                         m_SFilter.getSizeY() == source->getDim2Size());
    if(isModified(UPD_SCALE) || nosizematch) {
        unsetModified(UPD_SCALE);
        if(nosizematch) {
            m_SFilter.setSize(source->getDim1Size(), source->getDim2Size());
            m_FFilter.setSize(Fourier2D::complexX(m_SFilter.getSizeX())/2, 
                              m_SFilter.getSizeY());
            m_FTflt.initTransform(m_SFilter.getSizeX(), m_SFilter.getSizeY(),
                                  m_SFilter.getData(), 
                                  (double*)m_FFilter.getData());
        }
	float ffltsize = pow(2,scale);
	int fltsize = (int)ffltsize;
	if(fltsize<1) fltsize=1;
        m_AddSkip = fltsize/2+1;
	double stdev = ffltsize*0.5*0.2;
        create2DGaussian(m_SFilter, stdev, ffltsize, 
                         m_SFilter.getSizeX()/2.0f,m_SFilter.getSizeY()/2.0f);
        m_FTflt.transform();
//normalize transform and perform shift
        double factor = 1.0/m_SFilter.getSize();
        double nfactor = -factor;
        complex<double>* freq = m_FFilter.getData();
        for(int j=0; j<m_FFilter.getSizeY(); j++) {
            for(int i=0; i<m_FFilter.getSizeX(); i++, freq++) {
                if((i+j)&0x01) *freq *= nfactor;
                else *freq *= factor;
            }
        }
        setModified(UPD_DATA);
    }
}

//--- CombiSensor ------------------------------------------------------

CombiSensor::CombiSensor(int nchannels)
    : m_NormalizeInput(true)
{
    setNSources(nchannels);
    enableUpdate(UPD_DATA|UPD_CWEIGHTS);
    //togglePP(PP_FORCE);
}

CombiSensor::~CombiSensor()
{
    for(vector<sensor_ptr>::iterator s=sources.begin();
        s!=sources.end(); s++)
    {
        (*s)->unrefSuperSensor( shared_from_this() );
    }
}

void CombiSensor::clearSources() 
{
    vector<sensor_ptr>::iterator s=sources.begin();
    while(s!=sources.end()) {
        (*s)->unrefSuperSensor( shared_from_this() );
        s++;
    }
    sources.clear();
    cweights.clear();
    setModified(UPD_CWEIGHTS|UPD_DATA);
}

void CombiSensor::setNSources(int n) 
{
    if(n != (int)sources.size()) {
        //clearSources();
        sources.resize(n,(sensor_ptr)getZeroSensor());
        cweights.resize(n, 1);
	setModified(UPD_CWEIGHTS|UPD_DATA);
    }
}

void CombiSensor::setSource(sensor_ptr _source, float weight, int id)
{
    assert(id>=0);
    if(id>=(int)sources.size()) {
        setNSources(id+1);
    }
    else if (sources[id] != 0 && sources[id] != getZeroSensor()) 
    {
        sources[id]->unrefSuperSensor( shared_from_this() );
    }
    sources[id] = _source;
    cweights[id] = weight;
    _source->refSuperSensor( shared_from_this() );
    source = _source;
    if(!std::const_pointer_cast<Sensor>(source)->isUpdate(UPD_MINMAX)) {
        std::const_pointer_cast<Sensor>(source)->enableUpdate(UPD_MINMAX);
        std::const_pointer_cast<Sensor>(source)->setModified(UPD_MINMAX);
    }
    setModified(UPD_CWEIGHTS|UPD_DATA);
}

void CombiSensor::changeSource(sensor_cptr _source)
{
    if(!_source) return;
    const string& sid = _source->getID();
    vector<sensor_ptr>::iterator s = sources.begin();
    while(s!=sources.end() && (*s)->getID()!=sid)
        s++;
    if(s!=sources.end()) {      // && sid == (*s)->getID()) { // use assert?
        source = *s;
        PPSensor::changeSource(_source);
        *s = std::const_pointer_cast<Sensor>(source);
        if(!std::const_pointer_cast<Sensor>(source)->isUpdate(UPD_MINMAX)) {
            std::const_pointer_cast<Sensor>(source)->enableUpdate(UPD_MINMAX);
            std::const_pointer_cast<Sensor>(source)->setModified(UPD_MINMAX);
        }
    } else {
        TRACE("error replacing element <" << sid << "> in combi sensor - appending instead.");
        print(cout) << endl;
        setSource(std::const_pointer_cast<Sensor>(_source),1.f);
    }
}

/*
void CombiSensor::calcMinMax() 
{
    minval = 0;
    maxval = 0;
    for(vector<float>::const_iterator w = cweights.begin();
        w != cweights.end(); w++) {
        maxval += *w;
    }
}
*/

Sensor& CombiSensor::assign(const Sensor& rhs)
{ 
    if(&rhs != this) { 
        PPSensor::assign(rhs);
        if(typeid(&rhs) == typeid(CombiSensor*)) {
            const CombiSensor& crhs = (const CombiSensor&) rhs;
            if(sources != crhs.sources) {
                setNSources(crhs.sources.size());
                int sid = 0;
                const vector<float>& weights = crhs.getCWeights();
                for(vector<sensor_ptr>::const_iterator s = crhs.sources.begin();
                    s != crhs.sources.end(); s++, sid++)
                    setSource(std::const_pointer_cast<Sensor>(*s), weights[sid], sid);
            }
        }
    } 
    return *this;
}

ostream& CombiSensor::print(ostream& os) const 
{
    os << "s " << getID() << " ";
    //Sensor::print(os); would print modified source
    if(m_NormalizeInput) os << "k";
    else os << "K";
    vector<float>::const_iterator w = cweights.begin();
    for(vector<sensor_ptr>::const_iterator s=sources.begin();
        s!=sources.end(); s++, w++)
    {
        os << " " << (*s)->getID() << " " << *w;
    }
    return os;
}

ostream& CombiSensor::hprint(ostream &os, SensorCollection *sc) const
{
    if(sc->isPrinted(m_ID)) return os;
    for(vector<sensor_ptr>::const_iterator s=sources.begin();
        s!=sources.end(); s++)
    {
        (*s)->hprint(os, sc);
    }
    if(sc->isPrinted(m_ID)) return os;
    sc->setPrinted(m_ID);
    return (print(os) << endl);
}

//----------------------------------------------------------------------------
Sensor& MahalSensor::assign(const Sensor& rhs)
{ 
    if(&rhs != this) { 
        PPSensor::assign(rhs);
        if(typeid(&rhs) == typeid(MahalSensor*)) {
            const MahalSensor& crhs = (const MahalSensor&) rhs;
            m = crhs.m;
            icov = crhs.icov;
            m_Filename = crhs.m_Filename;
        }
    } 
    return *this;
}

bool MahalSensor::loadConfig(const string& fname)
{
    const string& fn = fname.empty() ? m_Filename : fname;
    ifstream is(fn.c_str());
    if(!is) return false;
    is >> icov;
    is >> m;
    if(icov.sizeX() != 3 || icov.sizeY() != 3) {
        icov = dmutil::makeIdentity<float>(3);
    }
    if(m.sizeX() != 1 || m.sizeY() != 3) {
        m.resize(1,3,0);
    }
    if(!fname.empty()) m_Filename = fname;
    return true;
}

bool MahalSensor::saveConfig(const string& fname) const
{
    const string& fn = fname.empty() ? m_Filename : fname;
    ofstream os(fn.c_str());
    if(!os) return false;
    os << icov;
    os << mean;
    if(!fname.empty()) m_Filename = fname;
    return true;
}

//----------------------------------------------------------------------------
//keep consistent with enum MappingTypes!
const char* MappingSensor::s_MappingNames[] = 
{"identity","clamplu","clampl","clampu","gaussnorm","mgaussnorm",
 "mclampu", "bias", 0};

Sensor& MappingSensor::assign(const Sensor& rhs)
{ 
    if(&rhs != this) { 
        PPSensor::assign(rhs);
        if(typeid(&rhs) == typeid(MappingSensor*)) {
            const MappingSensor& crhs = (const MappingSensor&) rhs;
            m_MappingID = crhs.m_MappingID;
            m_Param = crhs.m_Param;
        }
    } 
    return *this;
}

std::ostream& MappingSensor::print(std::ostream& os) const
{
    Sensor::print(os)
        << "f " << getMappingName();
    for(vector<float>::const_iterator p=m_Param.begin(); p!=m_Param.end(); p++)
    {
        os << " " << *p;
    }
    return os;
}

dword MappingSensor::setMappingID(dword id)
{
    if(id>=MS_LAST) m_MappingID = 0; else m_MappingID = id;
    return m_MappingID;
}

dword MappingSensor::setMapping(const string& mname)
{
    dword mid = 0;
    while(mid < MS_LAST && mname != s_MappingNames[mid]) mid++;
    return setMappingID(mid);
}

bool MappingSensor::readParams(istream& is)
{
    m_Param.clear();
    is >> ws;
    while(is) {
        float para= -0.12347891f;
        is >> para;
        if(para != -0.12347891f) m_Param.push_back(para);
        is >> ws;
    }
    dword psize = m_Param.size();
    if((m_MappingID == MS_CLAMPL || 
        m_MappingID == MS_CLAMPU ||
        m_MappingID == MS_GAUSSNORM ||
        m_MappingID == MS_MGAUSSNORM ||
        m_MappingID == MS_MCLAMPU ||
        m_MappingID == MS_BIAS)
       && psize<1)
    {
        m_MappingID = MS_IDENTITY; 
        return false; 
    }
    if(m_MappingID == MS_CLAMPLU && psize<2) 
    { 
        m_MappingID = MS_IDENTITY; return false; 
    }
    return true;
}

bool MappingSensor::performUpdate() {
    if(!isModified()) return false;
    if(source->getDim1Size()>0) {
        bool doanalysis = true;
        switch(m_MappingID) {
            case MS_MCLAMPU:
            case MS_GAUSSNORM:
            case MS_MGAUSSNORM:
            case MS_BIAS:
                if(m_Param.size() < 4) m_Param.resize(4);
                m_Param[1] = m_Param[2] = m_Param[3] = 0.f;
                break;
            default:
                doanalysis = false;
        }
        if(doanalysis) {
            int i,j;
            for(j=0; j<source->getDim2Size(); j++) {
                for(i=0; i<source->getDim1Size(); i++)
                {
                    float v = source->getValue(i,j);
                    switch(m_MappingID) {
                        case MS_BIAS:
                        case MS_GAUSSNORM:
                            m_Param[1] += v;
                            m_Param[2] += v*v;
                            m_Param[3]++;
                            break;
                        case MS_MCLAMPU:
                        case MS_MGAUSSNORM:
                            if(v>0) {
                                m_Param[1] += v;
                                m_Param[2] += v*v;
                                m_Param[3]++;
                            }
                            break;
                    }
                }
            }
            switch(m_MappingID) {
                case MS_MCLAMPU:
                    m_Param[1] /= m_Param[3];
                    m_Param[2] /= m_Param[3];
                    m_Param[2] = sqrt(m_Param[2]-m_Param[1]*m_Param[1]);
                    m_Param[1] = m_Param[1]+m_Param[2]*m_Param[0];
                    break;
                case MS_BIAS:
                case MS_GAUSSNORM:
                    m_Param[1] /= m_Param[3];
                    m_Param[2] /= m_Param[3];
                    m_Param[2] = sqrt(m_Param[2]-m_Param[1]*m_Param[1]);
                    m_Param[2] = 1/(m_Param[2]*m_Param[0]);
                    break;
                case MS_MGAUSSNORM:
                    m_Param[1] /= m_Param[3];
                    m_Param[2] /= m_Param[3];
                    m_Param[2] = sqrt(m_Param[2]-m_Param[1]*m_Param[1]);
                    m_Param[2] = 1/(m_Param[2]*m_Param[0]);
                    m_Param[1] = m_Param[1] - (m_Param[1]/m_Param[2]);
                    break;
            }
        }
    }
    return PPSensor::performUpdate();
}

//----------------------------------------------------------------------------
static
Image<double>& create2DGaussian(Image<double>& flt, double stdev, 
                                float clampd, float cx, float cy)
{
    clampd = clampd*clampd;
    int i,j;
    double sum=0;
    double* pixel = flt.getData();
    int xsize=flt.getSizeX(), ysize=flt.getSizeY();
    TRACE("creating filter " << xsize << " " << ysize);
    for(j=0; j<ysize; j++)
        for(i=0; i<xsize; i++, pixel++) {
            float ii = (float)i-cx;
            float jj = (float)j-cy;
            float d2 = ii*ii+jj*jj;
            if(d2<clampd) {
                double value = gaussd2(d2, stdev);
                *pixel = value;
                sum += value;
            }
        }
    flt *= (1.f/sum);
    return flt;
}

