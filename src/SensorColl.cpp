#include <typeinfo>
#include "SensorColl.h"
#include "SensorSet.h"
#include "Data.h"
#include "Errors.h"
#include "Model.h"
using namespace std;

//#define VERBOSE
#ifdef VERBOSE
#define SHOW(a) TRACE(a)
#else
#define SHOW(a)
#endif

SensorCollection::SensorCollection()
{
    addSensor(getZeroSensor()); // this is our zeroth sensor
}

SensorCollection::~SensorCollection()
{ }

sensor_ptr SensorCollection::addSensor(const std::string& key, sensor_ptr s)
{
    s->setID(key);
    return addSensor(s);
}

sensor_ptr SensorCollection::addSensor(sensor_ptr s) {
    if(!s) return NULL;
    if(s->getID().empty()) return NULL; //s->setID(size());
    sensor_ptr &spr = operator[](s->getID());
    selectSensor(s->getID());
    if(spr == NULL) {
        spr = s;
        return s;
    }else {
        SHOW("replacing sensor "<<s->getID());
        if(spr->assignRef(s)) {
            SHOW("assigning");
            s->replaceBy(spr);
        } else {
            SHOW("replacing");
            spr->replaceBy(s);
            spr = s;
        }
        return spr;
    }
}

sensor_ptr SensorCollection::getSensor(const string& id) {
    iterator fs = find(id);
    if(fs != end()) {
        return fs->second;
    } else {
//             char sid[5];
//             Sensor::getNumberString(sid,id);
//             cout << "zeroing " << sid << endl;
        sensor_ptr zs = makeSensor<ZeroSensor>();
        zs->setID(id);
        return addSensor(zs);
    }
}

void SensorCollection::clearPrintList(bool skipdefaults)
{
    m_Printlist.clear();
    if(skipdefaults) {
        m_Printlist.insert("d0");
        m_Printlist.insert("0");
    }
}

bool SensorCollection::isPrinted( const string& id ) const
{
    return m_Printlist.find(id) != m_Printlist.end();
}

void SensorCollection::setPrinted( const string& id )
{
    m_Printlist.insert(id);
}

ostream& operator<<(ostream& os, const SensorCollection& sc)
{
    ((SensorCollection*)&sc)->m_Printlist.clear();
    for(map<string,sensor_ptr>::const_iterator s=sc.begin(); s!=sc.end(); s++) {
        if (s->second->getID() != "0"       && // zero sensor
            !s->second->getID().empty()     && // uninitialized sensor
            s->second->getID() != "d0"      &&
            !sc.isPrinted(s->second->getID())
            )
        {
            s->second->hprint(os, (SensorCollection*)&sc);
        }
    }
    return os;
}

sensor_ptr SensorCollection::readSensor(ParseFile& is)
{
    if(!is.getNextLine()) return NULL;
    if(is.getKey() != "s") {
        is.pushLine();
        return NULL;
    }
    istringstream vs(is.getValue());
    vs >> ws;       // read key
    string skey;
    //vs.width(2047);
    vs >> skey;
    vs >> ws;       // read type
    string para1;
    vs >> para1;
    char stype;
    sensor_cptr source = NULL;
    if (para1 == "source") {
        string sname;
        vs >> sname;
        source = getSensor(sname);
        if(std::dynamic_pointer_cast<const ZeroSensor>(source))
            cout << "Using zero sensor as source. Source name was: "
                 << sname <<endl;
        else {
            SHOW("Using source " << source->getID());
        }
        vs >> stype;
    } else {
        stype = para1[0];
    }
    if(source == NULL) {
        source = getSensor(string("d0"));
    }
    sensor_ptr s = NULL;
    switch(stype) {
    case 'i' :
        s = makeSensor<IntensitySensor>(source);
        SHOW("created intensity sensor");
        break;
    case 'g' :
    {
        float scale=0;
        vs >> scale;
        s = makeSensor<SmoothIntensitySensor>(source, scale);
        SHOW("created Gaussian sensor of scale " << scale);
        break;
    }
    case 'd' :
    {
        //float scale=0;
        //vs >> scale;
        s = makeSensor<GradMagSensor>(source);
        SHOW("created gm sensor");// of scale " << scale);
        break;
    }
    case 'c' :
    {
//      float scale=0;
//      vs >> scale;
        s = makeSensor<CornerSensor>(source);
        SHOW("created corner sensor");// of scale " << scale);
        break;
    }
    case 'K' :
        s = makeSensor<CombiSensor>();
        std::dynamic_pointer_cast<CombiSensor>(s)->normalizeInput(false);
        // is legally constructed in the next case 'k'
    case 'k' :
    {
        int nsensors=0;
        //vs >> nsensors;
        if(!s) s = makeSensor<CombiSensor>();       //normalized input
        string sname;
        while(vs >> sname) {
            float weight=1;
            vs >> weight;
            sensor_ptr ss = getSensor(sname);
            if(ss == getZeroSensor())
                cout << "added zero sensor" << endl;
            std::dynamic_pointer_cast<CombiSensor>(s)->setSource(ss,weight);
            nsensors++;
        }
        SHOW("created combi sensor with " << nsensors<<" sub-sensors.");
        break;
    }
    case 'm' :
    {
        int nc = source ? source->getNChannels() : 0;
        vector<float> weights;
        weights.reserve(nc);
        float weight=1;
        while(vs >> weight) {
            weights.push_back(weight);
            cout << weight << " ";
        }
        cout << endl;
        s = makeSensor<MCIntensitySensor>(source);
        s->setCWeights(weights);
        SHOW("created multi-channel sensor.");
        break;
    }
    case 'M' :
    {
        s = makeSensor<MCGSensor>(source);
        SHOW("created multi-channel gradient sensor.");
        break;
    }
    case 'r' :
    {
        s = makeSensor<CRSensor>(source);
        SHOW("created multi-channel chrominance sensor.");
        break;
    }
    case 'C' :
    {
        string fname;
        vs >> fname;
        if(!fname.empty()) {
            s = makeSensor<MahalSensor>(source, fname);
            if(!std::dynamic_pointer_cast<MahalSensor>(s)->getFilename().empty()) {
                SHOW("created colour classification sensor");
            } else {
                SHOW("error loading configuration file for colour sensor "
                     << fname);
                s = sensor_ptr();
            }
        }
        break;
    }
    case 'f' :
    {
        string mapping;
        vs >> mapping;
        s = makeSensor<MappingSensor>(source, mapping);
        std::dynamic_pointer_cast<MappingSensor>(s)->readParams(vs);
        SHOW("created mapping sensor of type " <<
             std::dynamic_pointer_cast<MappingSensor>(s)->getMappingName());
        break;
    }
    case 'o' :
    {
        s = makeSensor<ZeroSensor>();
        SHOW("created zero sensor");
        break;
    }
    }
    if(!s) {
        is.setParseError(string("unknown sensor type ")+stype);
        throw new IOException("Error reading sensor.");
    }
    s->setID(skey);
    return s;
}

SensorCollection& SensorCollection::merge(SensorCollection& rhs)
{
    if(this == &rhs) return *this;
    for(map<string, sensor_ptr >::iterator si = rhs.begin();
        si != rhs.end(); si++)
    {
        if(!si->first.empty() && si->first!="0") {
            sensor_ptr spr = operator[](si->first); // sensor from lhs of collection assignment
            if(!spr) {
                spr = si->second;
            } else {
                bool lnzs = !std::dynamic_pointer_cast<ZeroSensor>(spr); // lhs sensor not zero
                bool rnzs = !std::dynamic_pointer_cast<ZeroSensor>(si->second); // rhs sensor not zero
                // TODO: inverted original logic, test if still valid
                if(rnzs) {
                    spr->replaceBy(si->second);
                    // spr = si->second;
                } else {
                    si->second->replaceBy(spr);
                }
            }
        }
    }
    return *this;
}

void SensorCollection::refModel(Model* model) const
{
    m_Models.insert(model);
}

void SensorCollection::unrefModel(Model* model) const
{
    m_Models.erase(model);
}

void SensorCollection::updateModels() const
{
    for(set<Model*>::iterator mo = m_Models.begin();
        mo != m_Models.end(); mo++)
        (*mo)->reattachSensors();
}
