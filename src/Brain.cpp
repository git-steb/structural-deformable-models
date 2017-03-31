#include <fxkeys.h>
#include <algorithm>
#include <functional>
#include <sstream>
#include <cstdint>

#include "Brain.h"
#include "ParseFile.h"

#include "deform.h"
#include "Data.h"
#include "SensorSet.h"
#include "mathutil.h"
#include "utils.h"
#include "glutils.h"

//#include "camgraph/gprocess.h"

using namespace std;

#undef __SHAPEWEIGHT
#define __SHAPEWEIGHT   0.1
#define MERGENAME "all"

//--------------------------------------

Brain::Brain() : m_DBSelector(m_DB), m_TimeStep(TIMESTEP), m_TimeScale(1),
                 m_UpdateAllModels(false), m_DrawOwnState(false),
                 m_SModel(*this)
{
    m_Data = std::make_shared<Dataset>();
    m_Data->changeSource(m_Data);
    m_BrowseData = std::make_shared<Dataset>();
    m_BrowseData->changeSource(m_BrowseData);
    m_Done = false;
    m_Sensors.addSensor("d0", m_Data);
    m_Geom = new Model(m_Data, &m_Sensors);
    m_CStates = new vector<Model*>; //&m_Instances[m_Geom->getName()];
    m_CurPrototype = m_Geom->getName();
    m_CStates->push_back(m_Geom);
    m_CurState=0;
    m_DesignMode = false;
    m_ImmediateData = false;
    for(int i = 0; i<DO_LAST; i++) m_ThreadActive[i] = false;
    startThread(Brain::DO_ANIMATE);
}

Brain::~Brain()
{
    for(int i = 0; i<DO_LAST; i++)
        if(m_ThreadActive[i]) m_RunMutex[i].lock();
    // will be unlocked by the thread
    for(int i = 0; i<DO_LAST; i++)
        if(m_ThreadActive[i]) m_RunMutex[i].lock();
    for(int i = 0; i<DO_LAST; i++)
        if(m_ThreadActive[i]) m_RunMutex[i].unlock();
    m_GeomMutex.lock();
//     for(map<string, vector<Model*> >::iterator ii = m_Instances.begin();
//         ii != m_Instances.end(); ii++)
//     {
//         changeModel(ii->first);
    if(m_CStates) {
        for(vector<Model*>::iterator gi=m_CStates->begin();
            gi!=m_CStates->end(); gi++)
        {
            delete *gi;
        }
        delete m_CStates;
        m_CStates = NULL;
    }
//    }
//     for(map<string,Model*>::iterator gi=m_Prototypes.begin();
//         gi!=m_Prototypes.end(); gi++)
//     {
//         delete gi->second;
//     }
}

const Dataset& Brain::getData() const
{
    return *m_BrowseData;
}

Dataset& Brain::getSensorData()
{
    return *m_Data;
}

bool Brain::load(const char *bfile) {
    ParseFile is(bfile);
    if(is) {
        cout << "loading " << bfile << endl;
        string sbpath = is.getPath();
        m_CSpecies.reset();
        //try?
        while(is.getNextLine()) {
            string param = is.getKey();
            if(param == "db") {
                if(!loadDB((sbpath+is.getValue()).c_str())) {
                    cerr << "Couldn't load data base " << is.getValue()<< endl;
                    return false;
                }
            } else if(param == "dbsel") {
                if(!loadDBSelector((sbpath+is.getValue()).c_str())) {
                    cerr<<"Couldn't load data base selector "
                        <<is.getValue()<<endl;
                    return false;
                }
            } else if(param == "data") {
                if(!loadData((sbpath+is.getValue()).c_str())) {
                    cerr << "Couldn't load data set " <<is.getValue()<< endl;
                    return false;
                }
            } else if(param == "model") {
                cout << "loading model " << is.getValue() << endl;
                if(!loadModel((sbpath+is.getValue()).c_str())) {
                    cerr << "Couldn't load model file " <<is.getValue()<< endl;
                    return false;
                }
            } else if(param == "structure") {
                cout << "loading structure " << is.getValue() << endl;
                vuLock glock(m_GeomMutex);
                if(!m_SModel.load((sbpath+is.getValue()).c_str())) {
                    cerr << "Couldn't load structure description "
                         << (sbpath+is.getValue()) << endl;
                    return false;
                }
                cout << m_SModel << endl;
//                 list<Model*> ml;
//                 if(getAllModels( back_inserter( ml ) )) {
//                     for(list<Model*>::iterator gi=ml.begin();
//                         gi != ml.end(); gi++)
//                         (*gi)->reattachSensors();
//                 }
//                 m_Search.reattachSensors();
                m_Sensors.updateModels();
            } else if(param == "searcher") {
                cout << "configuring searcher from " << is.getValue() << endl;
                ParseFile pf((sbpath+is.getValue()).c_str());
                if(!getSearchPara().read(pf)) {
                    cerr << "Couldn't searcher config "<<is.getValue()<< endl;
                    return false;
                }
                getSearchPara().write(cout);
            } else if(param.size()>0) {
                is.setParseError("Error parsing brain description.");
                cerr << is.getErrorMsg() << endl;
                return false;
            }
        } // of while getline
        if(!(*m_Geom)) {
            MStructure& cs = m_SModel.getNextStruct();
            if(cs) cout << "current structure is: " << cs.getName() << endl;
            if(!cs.getRefModel(m_CSpecies.id, *m_Geom))
                *m_Geom = cs.getModel();
        }
        cout << "finished loading " << bfile << endl;
    } else {
        cerr << "Could not open " << bfile << endl;
        return false;
    }
    return true;
}

bool Brain::loadModel(const char *filename)
{
    vuLock glock(m_GeomMutex);
    //Model *model = new Model(&m_Data, &m_Sensors);
    bool ret=m_Geom->readFile(filename);
    //changeModel(model->getName());
    //m_CStates->push_back(model);
    //m_Geom = model;
    //m_CurState = m_CStates->size()-1;
//     if(ret) {
//         m_Prototypes[m_Geom->getName()] = new Model(*m_Geom);
//         //remove initial dummy model
//         string dmn = Model().getName();
//         if(m_Instances.count(dmn)) {
//             for(vector<Model*>::iterator mi=m_Instances[dmn].begin();
//                 mi != m_Instances[dmn].end(); mi++)
//                 delete *mi;
//             m_Instances.erase(dmn);
//         }
//     }
//     list<Model*> ml;
//     if(getAllModels( back_inserter( ml ) )) {
//         for(list<Model*>::iterator gi=ml.begin(); gi != ml.end(); gi++)
//             if(*gi != m_Geom) (*gi)->reattachSensors();
//     }
//     m_SModel.reattachSensors();
//     m_Search.reattachSensors();
    m_Sensors.updateModels();
    setupSearch();
    return true;
}

bool Brain::loadDB(const char *filename)
{
    if(!m_DB.load(filename)) return false;
    m_DBSelector.update();
    loadSpecies(m_DBSelector.getCurSpecies());
    return true;
}

bool Brain::loadDBSelector(const char *filename)
{
    if(!m_DBSelector.load(filename)) return false;
    loadSpecies(m_DBSelector.getCurSpecies());
    return true;
}

bool Brain::loadSpecies(const Species& spec)
{
    m_CSpecies = spec;
    map<string,string>::const_iterator in = spec.find("image");
    if(in == spec.end()) return false;

    string msg = "species_id: ";
    stringstream sstrm;
    sstrm << m_CSpecies.getScale();
    string number;
    sstrm >> number;
    msg += m_CSpecies["species_id"] + "  image: " + m_CSpecies["image"]
        + " flags: " + m_CSpecies.getFlagString()
        + " scale (pixels per mm): "+number;
    setStatusText(msg.c_str());
    setTitle(m_CSpecies["name"].c_str());

    return loadData((m_DB.getDirectory()+"/"+in->second).c_str(),
                    spec.getScale());
}

bool Brain::changeModel(const string &mname) {
    return false;
#if 0
    m_CurPrototype = mname;
    m_CStates = &m_Instances[mname];
    switchState(0);
    return true;
#else
    map<string, vector<Model*> >::iterator fs =
        m_Instances.find(mname);
    if(fs != m_Instances.end()) {
//         iterator map<string, vector<Model*> >::iterator os =
//             m_Instances.find(m_CurPrototype);
//         if(os->first) {
//             os->second = m_CStates;
//         }
        m_CStates = &fs->second;
        m_CurPrototype = mname;
        switchState(0);
        return true;
    } else {
        m_Instances[mname].resize(0);
        m_CStates = &m_Instances[mname];
        m_CurPrototype = mname;
        //m_Geom is invalid now!
        return false;
    }
#endif
}

bool Brain::mergeInstances()
{
    return false;
    static const string mergename(MERGENAME);
    if(m_Instances.size() <= 1) return false;
    if(changeModel(mergename)) {
        for(vector<Model*>::iterator mi=m_CStates->begin();
            mi!=m_CStates->end(); mi++)
            delete *mi;
    }
    int msize=0;
    for(map<string, vector<Model*> >::iterator ii = m_Instances.begin();
        ii != m_Instances.end(); ii++)
        msize+=ii->second.size();
    m_CStates->reserve(msize);
    for(map<string, vector<Model*> >::iterator ii = m_Instances.begin();
        ii != m_Instances.end(); ii++)
    {
        if(ii->first != mergename)
            for(vector<Model*>::iterator mi=ii->second.begin();
                mi!=ii->second.end(); mi++)
                m_CStates->push_back(new Model(**mi));
    }
    switchState(0);
    return true;
}

bool Brain::loadData(const char *filename, dword ppmm)
{
    vuLock dlock(m_DataMutex);
    if(!ppmm) ppmm = m_CSpecies.getScale();
    bool ret = m_BrowseData->load(filename, ppmm);
    if(ret) {
        if(m_ImmediateData) attachBrowseData();
        else if(m_Data->initialized()) m_Data->clear();
    }
    return ret;
}

void Brain::attachBrowseData()
{
    if(m_Data->getFilename() == m_BrowseData->getFilename()) {
        cout << "dataset already attached" << endl;
        return;
    }
    cout << "attaching data..." << endl;
    m_Data->copyData(*m_BrowseData);
    list<Model*> ml;
    if(getAllModels(back_inserter(ml))) {
        for(list<Model*>::iterator m=ml.begin(); m!=ml.end(); m++)
            (*m)->adaptDataScale();
    }
    m_SModel.adaptDataScale();
    setupSearch();
    cout << "attaching data done." << endl;
}

#define EXIT_THREAD(t)                          \
    {                                           \
        m_ThreadActive[t] = false;              \
        m_RunMutex[t].unlock();                 \
    }

#define CHECK_THREAD(t)                         \
    {                                           \
        m_RunMutex[t].unlock();                 \
        vuThread::usleep(100);                  \
        if(!m_RunMutex[t].trylock()) {          \
            EXIT_THREAD(t);                     \
            break;                              \
        }                                       \
    }

void Brain::run(int whatsup, void* data)
{
    unsigned long lastticks = 0;
    static int anastate = 0;
    switch(whatsup) {
    case DO_ANIMATE:
        if(m_ThreadActive[DO_ANIMATE]) break;
        m_ThreadActive[DO_ANIMATE] = true;
        while(m_RunMutex[DO_ANIMATE].trylock()) {
            double dt;
            unsigned long nowticks = getMilliSeconds();
            if(!lastticks) {
                lastticks = nowticks;
                dt = 0.0;
            } else {
                dt = (nowticks-lastticks)*0.001; //seconds
            }
            if(dt>m_TimeStep) {
                vuLock glock(m_GeomMutex);
                vuLock dlock(m_DataMutex);
                if(dt>2*m_TimeStep) dt = m_TimeStep;
                lastticks=nowticks;
                if(m_Data->getDim1Size()) {
                    evolve(dt*m_TimeScale);
                }
                if(m_UpdateAllModels) {
                    for(vector<Model*>::iterator m = m_CStates->begin();
                        m!=m_CStates->end(); m++)
                    {
                        (*m)->updateParticles(dt*m_TimeScale);
                    }
                }
            }
            vuThread::usleep((dword)(1000*m_TimeStep));
            m_RunMutex[DO_ANIMATE].unlock();
        }
        m_ThreadActive[DO_ANIMATE] = false;
        m_RunMutex[DO_ANIMATE].unlock();
        break;
    case DO_NOTHING:
        if(m_ThreadActive[DO_NOTHING]) break;
        m_ThreadActive[DO_NOTHING] = true;
        while(m_RunMutex[DO_NOTHING].trylock()) {
            usleep((dword)(1000*m_TimeStep));
            m_RunMutex[DO_NOTHING].unlock();
        }
        m_ThreadActive[DO_NOTHING] = false;
        m_RunMutex[DO_NOTHING].unlock();
        break;
    case DO_ANALYSIS:
    {
        anastate = (int)(intptr_t)data;
        if(m_ThreadActive[DO_ANALYSIS]) break;
        m_ThreadActive[DO_ANALYSIS] = true;
        if(!m_RunMutex[DO_ANALYSIS].trylock()) {
            EXIT_THREAD(DO_ANALYSIS);
            break;
        } // now we have locked the runmutex
        MStructure& st = m_SModel.getNextStruct(0);
        if(m_DBSelector.nextSelection(-2)) {
            ofstream os("structmatch.txt",ios::app);
            os << "checking structural matches" << endl << endl;
            do{
                if(m_DBSelector.getCurSpecies().hasFlag(Species::SEL5)) {
                    m_GeomMutex.lock();
                    loadSpecies(m_DBSelector.getCurSpecies());
                    m_GeomMutex.unlock();
                    CHECK_THREAD(DO_ANALYSIS);
                    cout << "hello" << endl;
                    m_GeomMutex.lock();
                    attachBrowseData();
                    m_GeomMutex.unlock();
                    //do analysis here (anahere)
                    os << "species id: " << m_CSpecies.id
                       << "  name: " << m_CSpecies["name"] << endl;
                    unsigned long nowticks = getMilliSeconds();
                    m_SModel.startSearch();
                    while(!m_SModel.searchDone()) {
                        CHECK_THREAD(DO_ANALYSIS);  // sleep
                    }
                    const map<string,StructPath>& interp =
                        m_SModel.getIPaths();
                    os << "best interpretation: "
                       << m_SModel.getBestInterpretation() << endl;
                    for(map<string,StructPath>::const_iterator ip =
                            interp.begin(); ip != interp.end(); ip++)
                    {
                        os << ip->first << ": "
                           << ip->second.getRelGoodness() << endl;
                    }
                    os << "time: " << (getMilliSeconds()-nowticks)*0.001
                       << endl;
                    cout << "best interpretation: "
                         << m_SModel.getBestInterpretation() << endl;
                    //output results
                }
                if(anastate == -1) { EXIT_THREAD(DO_ANALYSIS); break; }
            } while(m_DBSelector.nextSelection(1,false));
        }
        EXIT_THREAD(DO_ANALYSIS);
        cout << "analysis terminated" << endl;
        break;
    }
    default:
        break;
    }
}

bool Brain::triggerTest(int mx, int my, int what) {
    static int keymode=0;
    static int curpid=-1;
    static string command;
    static list<vector<float> > colors;
    switch(keymode) {
    case 1:
    {
        vuLock glock(m_GeomMutex);
        keymode = 0;
        return m_Search.triggerTest(mx, my, what);
    }
    break;
    case 2:
        if(what == KEY_Return) {
            cout << "command: " << command << endl;
            command = "";
            keymode = 1;
        } else {
            command += char(what);
            cout << char(what) << flush;
        }
        return true;
        break;
    case 3:
    {
        vuLock glock(m_GeomMutex);
        keymode = 0;
        bool ret = m_SModel.triggerTest(mx, my, what);
        return ret;
    }
    break;
    }
    switch(what) {
    case KEY_X:
        cout << "type command: " << flush;
        keymode = 2;
        break;
    case KEY_h:
        if(keymode) {
            if(keymode == 128) {
                dword cnt = 0;
                ofstream os("antcol.dat",ios::app);
                for(list< vector<float> >::const_iterator col=
                        colors.begin(); col != colors.end(); col++, cnt++)
                {
                    for(vector<float>::const_iterator c=col->begin();
                        c!=col->end(); c++)
                        os << *c << " ";
                    os << endl;
                }
                os << endl;
                cout << "wrote " << cnt << " colors to antcol.dat" << endl;
            }
            keymode = 0;
            colors.clear();
        } else keymode = 128;
        break;
    case KEY_j:
        if(keymode) {
            if(keymode == 129) {
                ofstream os("antbg.dat",ios::app);
                dword cnt=0;
                for(list< vector<float> >::const_iterator col=
                        colors.begin(); col != colors.end(); col++, cnt++)
                {
                    for(vector<float>::const_iterator c=col->begin();
                        c!=col->end(); c++)
                        os << *c << " ";
                    os << endl;
                }
                os << endl;
                cout << "wrote " << cnt << " colors to antbg.dat" << endl;
            }
            keymode = 0;
            colors.clear();
        } else keymode = 129;
    case KEY_H: //mouse move
        if(keymode == 128 || keymode == 129) {
            vuLock dlock(m_DataMutex);
            vector<float> col(m_BrowseData->getMValue(mx,my));
            for(vector<float>::const_iterator c=col.begin();
                c!=col.end(); c++)
                cout << " " << *c;
            cout << endl;
            colors.push_back(col);
        }
        if(m_DesignMode) {
            vuLock glock(m_GeomMutex);
            int HLNode = m_Geom->nearestNode(Point(mx,my));
            m_Geom->setHLNode(HLNode);
            setStatusText(string("node id: ") + toString(HLNode+1));
        } else m_Geom->setHLNode(-1);
        break;
    case KEY_Control_L: //mouse click
        if(!m_DesignMode) {
            vuLock glock(m_GeomMutex);
            m_Geom->translate(Point(mx,my)-m_Geom->getCenter());
        } else {
            vuLock glock(m_GeomMutex);
            if(m_Geom->getHLNode() >= 0) {
                m_Geom->getNode(m_Geom->getHLNode()).
                    toggleState(Node::ST_SELECT);
            }
#if 0
            cout<<"adding edge ["<<curpid+1
                <<" "<<cnid+1<<"]"<<endl;
            m_Geom->addEdge(curpid,cnid);
            m_Geom->prepareTorque();
#endif
        }
        break;
    case KEY_Return:
    {
        vuLock dlock(m_DataMutex);
        attachBrowseData();
    }
    break;
    case KEY_r: // r
    {
        //TRACE(m_Geom->getStdRadius());
        m_Geom->addNoise(m_Geom->phys.imgforce*0.05);
        break;
    }
    case KEY_d: // d
    {
        keymode = 3;
        cout << "smodel key: " << endl;
//          vuLock glock(m_GeomMutex);
//          duplicateState();
//          TRACE(m_CStates->size() << " models");
    }
    break;
    case KEY_space: // space
    {
        static bool ison = true;
        ison = !ison;
        toggleUpdateForAll(ison);
    }
    break;
    case KEY_s: // s
    {
        keymode = 1;
        cout << "searcher key: " << endl;
        break;
    }
    break;
    case KEY_S: //S
        m_DrawOwnState = !m_DrawOwnState;
        break;
    case KEY_q:
        setupSearch(SW_UB, m_Geom->getProperties());
        break;
    case KEY_a:
        setupSearch(SW_LB, m_Geom->getProperties());
        break;
    case KEY_A:
        setupSearch(SW_SPREAD, m_Geom->getProperties());
        break;
    case KEY_z:
        setupSearch(SW_SET);
        break;
    case KEY_Z:
        setupSearch();
        break;
    case KEY_W:
    {
        vuLock glock((vuMutex&)m_GeomMutex);
        *m_Geom = m_Search.getRepresentative();
        break;
    }
    case KEY_Right: //right
    {
        vuLock glock(m_GeomMutex);
        switchState((m_CurState+1)%m_CStates->size());
        cout << "quality of fit "<<(m_Geom->getQualityOfFit()) << endl;
        cout << " flags: " << hex << m_Geom->getFlags() << endl << dec;
    }
    break;
    case KEY_Left: //left
    {
        vuLock glock(m_GeomMutex);
        switchState((int(m_CurState)-1)%m_CStates->size());
        cout << "quality of fit "<<(m_Geom->getQualityOfFit()) << endl;
        cout << " flags: " << hex << m_Geom->getFlags() << endl << dec;
    }
    break;
    case KEY_l: //l
        if(m_Geom->getSelectedNodesN()) {
            m_Geom->adaptRestLengthSel(0.2, m_Geom->selMaskMat());
        } else m_Geom->adaptRestLength(0.2);
        break;
    case KEY_L:
    {
        vuLock glock(m_GeomMutex);
        m_SModel.load(m_SModel.getFilename());
        break;
    }
    case KEY_k: //k
        killCurrentState();
        break;
    case KEY_p: //p
        m_Geom->adaptProportion(0.3);
        break;
    case KEY_D: //D
    {
        vuLock glock(m_GeomMutex);
        float lqof,hqof;
        getQualityRange(lqof,hqof,__SHAPEWEIGHT);
        float qth = hqof*0.7;
        TRACE("quality th " <<qth);
#if 1
//#ifndef DISTRIBUTE_ALL_PROTOTYPES
//            Model *pt = m_Prototypes[m_CurPrototype];
        Model *pt = m_Geom;
        cout << "distributing " << pt->getName() << endl;
        pt->setShapeWeight(__SHAPEWEIGHT);
        int nm=0, ni=100;
        while(nm<50 && --ni)
            nm+=distributeModel(*pt,0,qth,true);
#else
        for(map<string,Model*>::iterator gi=m_Prototypes.begin();
            gi!=m_Prototypes.end(); gi++)
        {
            cout << "distributing " << gi->second->getName() << endl;
            gi->second->setShapeWeight(__SHAPEWEIGHT);
            int nm=0, ni=100;
            while(nm<50 && --ni)
                nm+=distributeModel(*gi->second,0,qth,true);
        }
#endif
        TRACE(m_CStates->size() << " models");
    }
    break;
    case KEY_R: //R
    {
        vuLock glock(m_GeomMutex);
        m_SModel.evalRelations();
//                  for(vector<Model*>::iterator m=m_CStates->begin();
//                      m!=m_CStates->end(); m++) {
//                      (*m)->addNoise(0.05);
//                         (*m)->setWinner(false);
//                     }
    }
    break;
    case KEY_K: //K
    {
        vuLock glock(m_GeomMutex);
        killAllStates();
    }
    break;
    case KEY_g: //g
    {
        MStructure& cs = m_SModel.getNextStruct(0);
        loadModel(cs.getModel().getFilename().c_str()); //"shape.dm"
        cs.setModel(*m_Geom);
    }
    break;
    case KEY_G: //G
    {
        MStructure& cs = m_SModel.getNextStruct(0);
        m_Geom->writeFile(cs.getModel().getFilename().c_str());
        break;
    }
    case KEY_v: //v
    {
        vuLock glock(m_GeomMutex);
        Node n((float)mx,(float)my);
        bool doubleclick=false;
        if(m_Geom->getNNodes()>0) {
            Node &ln = m_Geom->getNode(m_Geom->getNNodes()-1);
            doubleclick = ((n-ln).norm()<2);
        }
        if(doubleclick) {
            //cout << "doubleclick"<<endl;
            int lnid = m_Geom->getNNodes()-1;
            if(lnid>0) {
                m_Geom->addEdge(lnid-1,lnid);
                m_Geom->prepareTorque();
            }
        } else {
            n.attachSensor(m_Sensors.getSensor("S0"));
            m_Geom->setHLNode(m_Geom->addNode(n));
        }
    }
    break;
    case KEY_V: //V
    {
        vuLock glock(m_GeomMutex);
        m_Geom->removeNode(m_Geom->getHLNode());
    }
    break;
    case KEY_u:
    {
        m_DBSelector.getCurSpecies().setFlag(Species::SEL5);
        cout << m_DBSelector.getCurSpecies().getFlagString() << endl;
        break;
    }
    case KEY_U:
    {
//             //one update step
//          vuLock glock(m_GeomMutex);
//          for(vector<Model*>::iterator m = m_CStates->begin();
//              m!=m_CStates->end(); m++)
//          {
//              (*m)->updateParticles(m_TimeStep);
//          }
        m_DBSelector.getCurSpecies().unsetFlag(Species::SEL5);
        cout << m_DBSelector.getCurSpecies().getFlagString() << endl;
        break;
    }
    case KEY_1: //1
        //m_Geom->pushRotate(m_Geom->getCenter(), 1);
        m_Geom->rotate(M_PI*0.03);
        break;
    case KEY_2: //2
        //m_Geom->pushRotate(m_Geom->getCenter(), -1);
        m_Geom->rotate(-M_PI*0.03);
        break;
    case KEY_3:
        if(m_Geom->getSelectedNodesN()) {
            m_Geom->scaleSel(0.95, m_Geom->selMaskMat());
        } else m_Geom->scale(0.95, true);
        break;
    case KEY_4:
        if(m_Geom->getSelectedNodesN()) {
            m_Geom->scaleSel(1.05, m_Geom->selMaskMat());
        } else m_Geom->scale(1.05, true);
        break;
    case KEY_5: //5
        if(m_Geom->getHLNode()>=0) {
            m_Geom->getNode(m_Geom->getHLNode()).setPos(Point2D(mx,my));
            for(int e=0; e<m_Geom->getNEdges(); e++) {
                Edge& ed = m_Geom->getEdge(e);
                if(ed.from == m_Geom->getHLNode() ||
                   ed.to == m_Geom->getHLNode()) ed.adaptRestLength();
            }
        }
        break;
    case KEY_6:
    {
        vuLock glock(m_GeomMutex);
        sensor_ptr s = m_Sensors[m_Sensors.getSelectedSensor()];
        m_Geom->getNode(m_Geom->getHLNode()).attachSensor(s);
        break;
    }
    case KEY_7:
    {
        vuLock glock(m_GeomMutex);
        sensor_ptr zs = std::make_shared<ZeroSensor>();
        zs = m_Geom->getSensorCollection()->addSensor("zero", zs);
        Node n; n.setPos(m_Geom->getNode(5));
        //n.mass *= 0.1;
        n.attachSensor(zs);
        int nid = m_Geom->addNode(n);
        int eid = m_Geom->addEdge(18, nid);
        m_Geom->getEdge(eid).springconstant *= 1;
        m_Geom->getEdge(eid).restlength *= 1.5;
        m_Geom->getEdge(m_Geom->addEdge(4, nid)).springconstant *= 1;
        m_Geom->getEdge(m_Geom->addEdge(6, nid)).springconstant *= 1;
        m_Geom->prepareTorque();
        break;
    }
    case KEY_8:
    {
        vuLock glock(m_GeomMutex);
        MStructure& st = m_SModel.getNextStruct(0);
        (Searcher&)st.getSearcher() = m_Search;
        break;
    }
    case KEY_9:
    {
        vuLock glock(m_GeomMutex);
        MStructure& st = m_SModel.getNextStruct(0);
        m_Search = st.getSearcher();
        break;
    }
    case KEY_F1: //F1
        m_TimeScale*=2;
        setStatusText(string("current time scale is ")
                      +toString(m_TimeScale));
        break;
    case KEY_F2: //F2
        m_TimeScale*=0.5;
        setStatusText(string("current time scale is ")
                      +toString(m_TimeScale));
        break;
    case KEY_F3: //F3
        m_Done = !m_Done;
        break;
    case KEY_F4: //F4
    {
        vuLock glock(m_GeomMutex);
// determine range of qualities
        float lqof,hqof;
        getQualityRange(lqof,hqof,__SHAPEWEIGHT);
// remove bad ones
        float kill_quality_th = 0.05;
        float relq = (hqof-lqof)*kill_quality_th+lqof;
        //(hqof!=0.0) ? (m->getQualityOfFit()-lqof)/hqof :1;
        for(int mi=0; mi<(int)m_CStates->size()
                && (int)m_CStates->size()>1;)
        {
            Model *m = (*m_CStates)[mi];
            if(relq > m->getQualityOfFit()) {
                //TRACE(relq);
                killState(mi);
            }else mi++;
        }
        break;
    }
    case KEY_P:
    {
        Point mouse(mx,my);
        int mind=0; float md=1000000;
        int cind = 0;
        for(vector<Model*>::iterator m = m_CStates->begin();
            m!=m_CStates->end(); m++,cind++)
        {
            float d = ((*m)->getCenter()-mouse).norm2();
            if(!cind) md = d;
            else if(d<md) {
                md = d;
                mind = cind;
            }
        }
        vuLock glock(m_GeomMutex);
        switchState(mind);
        cout << "quality of fit "<<(m_Geom->getQualityOfFit()) << endl;
        float lv = m_Geom->getDeformation();
        cout << "deformation: " << lv << " mapped: "<<exp(-5*abs(lv))<<endl;
        cout << "sensors: "<< m_Geom->getSensorFit() << endl;
        dword nes = 0;
        cout << "edges: "<< m_Geom->getEdgeSensorFit(&nes) << endl;
        cout << "   nedgesensors: " << nes << endl;
        cout << " flags: " << hex << m_Geom->getFlags() << endl;
        break;
    }
    case KEY_0: //0
    {
        vuLock glock(m_GeomMutex);
// determine range of qualities (in terms of shape)
        float lqof,hqof,aqof;
        lqof=1;
        hqof=0;
        aqof=0;
        for(vector<Model*>::const_iterator m = m_CStates->begin();
            m!=m_CStates->end(); m++)
        {
            float lv = (*m)->getDeformation()*10;
            lv = exp(-lv*lv);
            if(lv>hqof) hqof=lv;
            if(lv<lqof) lqof=lv;
            aqof+=lv;
        }
        aqof /= m_CStates->size();
// remove bad ones
        //float kill_quality_th = 0.03;
        for(int mi=0; mi<(int)m_CStates->size()
                && (int)m_CStates->size()>1;)
        {
            Model *m = (*m_CStates)[mi];
            float lv = m->getDeformation()*10;
            lv = exp(-lv*lv);
            float relq = (hqof!=0.0) ? (lv-lqof)/hqof :1;
            //if(relq < kill_quality_th && !m->isWinner()) {
            if(lv < aqof && !m->isWinner()) {
                TRACE(relq);
                killState(mi);
            }else mi++;
        }
        break;
    }
    case KEY_w: //w
    {
        list<const Model*> winners;
        m_Search.getWinners( back_inserter( winners ) );
        if(!winners.empty()) {
            vuLock glock(m_GeomMutex);
            m_CStates->clear();
            for(list<const Model*>::iterator wpi = winners.begin();
                wpi != winners.end(); wpi++)
                duplicateState(*wpi);
            switchState(0);
            cout << "copied " << winners.size() << " winners" << endl;
        }
        break;
    }
    case KEY_t:
    {
        vuLock glock(m_GeomMutex);
        dword nesensors = m_Geom->createEdgeSensors(0.15);
        cout << "created " << nesensors << " edge sensors" << endl;
        break;
    }
    case KEY_e:
    { vuLock glock(m_GeomMutex); m_Geom->connectNodes(); break; }
    case KEY_E:
    { vuLock glock(m_GeomMutex); m_Geom->removeEdges(); break; }
    case KEY_n:
    {
        for(int i = 0; i<m_Geom->getNNodes(); i++)
            m_Geom->getNode(i).disableState(Node::ST_SELECT);
        break;
    }
    case KEY_N:
    {
        for(int i = 0; i<m_Geom->getNNodes(); i++)
            m_Geom->getNode(i).enableState(Node::ST_SELECT);
        break;
    }
    case KEY_c:
    {
        vuLock glock(m_GeomMutex);
        Node n((float)mx,(float)my);
        bool doubleclick=false;
        if(m_Geom->getNNodes()>0) {
            Node &ln = m_Geom->getNode(m_Geom->getNNodes()-1);
            doubleclick = ((n-ln).norm()<2);
        }
        if(doubleclick) {
            //cout << "doubleclick"<<endl;
            int lnid = m_Geom->getNNodes()-1;
            if(lnid>0) {
                m_Geom->addEdge(lnid-1,lnid);
                m_Geom->prepareTorque();
            }
        } else {
            n.attachSensor(m_Sensors.getSensor("S0"));
            m_Geom->setHLNode(m_Geom->addNode(n));
        }
        break;
    }
#ifndef SWIG
/*
  case KEY_o: // postscript output
  {
  string fname = "graph.ps";
  CAMgraphicsProcess  Gprocess; // declare a graphics process
  // declare a PostScript driver
  CAMpostScriptDriver Pdriver(fname.c_str());
  Gprocess.attachDriver(Pdriver); // Attach driver to process

  Gprocess.setAxisRange(0,m_BrowseData->getDim1Size(),0,
  m_BrowseData->getDim2Size());
  Gprocess.title("Deformable model"); // label the plot
  Gprocess.labelX(" X ");
  Gprocess.labelY(" Y ");
  m_Geom->drawPS(Gprocess);
  //Gprocess.plot(x,y,n);                       // do the plotting
  Gprocess.frame();                           // "frame" the plot

  Gprocess.detachDriver();                    // detach the driver
  cout << "wrote file " << fname << endl;
  break;
  }
*/
#endif
    case KEY_period: //.
        m_DesignMode = !m_DesignMode;
//            *m_Prototypes[m_Geom->getName()] = *m_Geom;
        break;
    case KEY_Page_Up://pgup
    {
        m_DBSelector.nextSelection(-1);
        loadSpecies(m_DBSelector.getCurSpecies());
        break;
    }
    case KEY_Page_Down://pgdn
    {
        m_DBSelector.nextSelection(1);
        loadSpecies(m_DBSelector.getCurSpecies());
        break;
    }
    case KEY_Home://home
    {
        m_DBSelector.nextSelector(-1, true, 4);
        loadSpecies(m_DBSelector.getCurSpecies());
        break;
    }
    case KEY_End://end
    {
        m_DBSelector.nextSelector(1, true, 4);
        loadSpecies(m_DBSelector.getCurSpecies());
        break;
    }
    case KEY_Insert:
    {
        cout << m_DBSelector.getCurSpecies();
        cout << "image resolution is " << m_BrowseData->getDim1Size()
             << ", " << m_BrowseData->getDim2Size() << endl;
        break;
    }
    case KEY_Escape:
    {
        cout << "current flags <"
             << m_DBSelector.getCurSpecies().getFlagString()
             << "> change to (type 'savedb' to save DB): ";
        string cmdline;
        getline(cin, cmdline);
        dword split = cmdline.find(' ');
        string command = cmdline.substr(0,split);
        string value;
        if(split != cmdline.npos) value = cmdline.substr(split+1);
        cout << "command <" << command << "> value <"<<value<<">"<< endl;
        doCommand(command,value,mx,my);
        break;
    }
    case KEY_m:
    {
        vuLock glock((vuMutex&)m_GeomMutex);
#if 0
//scan through all reference models
        if(!m_SModel.getStructs().empty()) {
            list<dword> ids;
            MStructure& st = m_SModel.getNextStruct(0);
            cout << "scanning " << st.getName() << endl;
            cout << "nids: "
                 << st.getRefModelIDs( back_inserter(ids) ) << endl;
            for(list<dword>::iterator mid = ids.begin();
                mid != ids.end(); mid++) {
                cout << *mid << endl;
                Dataset d;
                Model m(&d);
                if(st.getRefModel(*mid, m))
                    cout << m.getProperties() << endl;
            }
        }
#else
        MStructure& st = m_SModel.getNextStruct(0);
        if(*m_Geom) {
            cout << "setting reference model for species id "
                 << m_CSpecies.id << endl;
            st.setRefModel(m_CSpecies.id, *m_Geom);
        } else {
            cout << "setting reference frame for species id "
                 << m_CSpecies.id << endl;
            PropVec frameprop(m_BrowseData->getPropVecMM());
            DUMP(frameprop);
            st.setRefProp(m_CSpecies.id, frameprop);
        }
#endif
        break;
    }
    case KEY_M:
    {
        vuLock glock((vuMutex&)m_GeomMutex);
        MStructure& st = m_SModel.getNextStruct(0);
        if(st.getRefModel(m_CSpecies.id, *m_Geom)) {
            cout << "found reference model!" << endl;
//                 list<Model*> ml;
//                 if(getAllModels( back_inserter( ml ) )) {
//                     for(list<Model*>::iterator gi=ml.begin();
//                         gi != ml.end(); gi++)
//                         (*gi)->reattachSensors();
//                 }
//                 m_Search.reattachSensors();
//                 m_SModel.reattachSensors();
            m_Sensors.updateModels();
            PropVec prop;
            if(st.getRefProp(m_CSpecies.id, prop))
                DUMP(prop);
        } else cout << "could not find reference model" << endl;
        break;
    }
    case KEY_comma:
    {
        vuLock glock((vuMutex&)m_GeomMutex);
        MStructure& cs = m_SModel.getNextStruct();
        if(cs) cout << "current structure is: " << cs.getName() << endl;
//            if(!cs.getRefModel(m_CSpecies.id, *m_Geom))
        *m_Geom = cs.getModel();
        m_Geom->reattachSensors();
        break;
    }
    case KEY_slash:
    {
//show next interpretation
        vuLock glock(m_GeomMutex);
        m_SModel.showNextIP();
        map<string,StructPath>::const_iterator cp =
            m_SModel.getIPaths().find(m_SModel.getShownIP());
        float qual = 0.f;
        if(cp != m_SModel.getIPaths().end())
            qual = cp->second.getRelGoodness();
        cout << "showing interpretation " << m_SModel.getShownIP()
             << " (" << qual << ")" << endl;
        break;
    }
    case KEY_backslash:
    {
//show best interpretation
        vuLock glock(m_GeomMutex);
        m_SModel.setShownIP(string());
        cout << "switching to best interpretation" << endl;
        map<string,StructPath>::const_iterator cp =
            m_SModel.getIPaths().find(m_SModel.getShownIP());
        float qual = 0.f;
        if(cp != m_SModel.getIPaths().end())
            qual = cp->second.getRelGoodness();
        cout << "showing interpretation " << m_SModel.getShownIP()
             << " (" << qual << ")" << endl;
        break;
    }
    default:
        return false;
    }
    return true;
}

bool Brain::doCommand(const std::string& command, const std::string& value,
                      int mx, int my)
{
    if(command == "savedb") {
        ofstream os(m_DB.getFilename().c_str());
        if(os) {
            cout << "saving " << m_DB.getFilename();
            os << m_DB;
            os.close();
            cout << " done." << endl;
        } else cout << "Could not create " << m_DB.getFilename()<<endl;
    } else if(command == "scale0" || command == "scale1" ||
              command == "scaleh" || command == "scaled") {
        static int oldmx = -1;
        static float scale = 1;
        if(command == "scaleh") scale *= 0.5;
        else if(command == "scaled") scale *= 2;
        cout << "using scale " << scale << " mm" << endl;
        if(command == "scale0") oldmx = mx;
        else if(command == "scale1") {
            m_DBSelector.getCurSpecies()["scale"] =
                toString(abs(mx-oldmx)/scale);
            cout << m_DBSelector.getCurSpecies().getScale()
                 << " pixels per mm" << endl;
        }
    } else if(command == "flags") {
        m_DBSelector.getCurSpecies().readFlagString(value);
        cout << endl << "new flags are <"
             << m_DBSelector.getCurSpecies().getFlagString()
             << ">" << endl;
    } else if(command == "go") {
        dword id; fromString(value, id);
        //m_DB.select(Species(id));
        loadSpecies(m_DB[id]);
    } else if(command == "calstat") {
        MStructure& st = m_SModel.getNextStruct(0);
        st.buildAllStats();
    } else if(command == "selall") {
        dword nsel = m_DB.select(Species(), Species::CMP_GREATER);
        cout << "selected all " << nsel
             << " species in data base" << endl;
    } else if(command == "process") {
        MStructure& st = m_SModel.getNextStruct(0);
        if(!*m_Geom) {
            m_DBSelector.nextSelection(0);
            loadSpecies(m_DBSelector.getCurSpecies());
            dword firstid = 0;
            while(m_CSpecies.id != firstid) {
                if(!firstid) firstid = m_CSpecies.id;
                if(m_BrowseData->getPPMM()) {
                    cout << "setting reference frame for species id "
                         << m_CSpecies.id << endl;
                    PropVec frameprop(m_BrowseData->getPropVecMM());
                    DUMP(frameprop);
                    st.setRefProp(m_CSpecies.id, frameprop);
                }
                m_DBSelector.nextSelection(1);
                loadSpecies(m_DBSelector.getCurSpecies());
            }
        }
    } else if(command == "setdir") {
        if(value.empty()) {
            cout << "current dir node: "
                 << m_Geom->getDirNode().getIndex() << endl;
        } else {
            int dirnode; fromString(value, dirnode);
            MStructure& st = m_SModel.getNextStruct(0);
            list<dword> refmod;
            st.getRefModelIDs( back_inserter(refmod) );
            cout << m_DB.select(refmod) << " species selected" << endl;
            Model model; // not attached to anything
            for(list<dword>::const_iterator idi = refmod.begin();
                idi != refmod.end(); idi++) {
                model.readFile(
                    st.getInfoFilename(toString(*idi)).c_str());
                DUMP(model.getProperties());
                DUMP(model.getDirNode().getIndex());
                model.setDirNode(dirnode);
                DUMP(model.getProperties());
                DUMP(model.getDirNode().getIndex());
                st.setRefModel(*idi, model);
            }
        }
    } else if(command == "savesearcher") {
        string fname = value.empty() ? "searcher.cnf" : value;
        ofstream os(fname.c_str());
        getSearchPara().write(os, true);
    } else if(command == "hbs") {
        dword hbs; fromString(value, hbs);
        m_BrowseData->setHalveBeyondSize(hbs);
        cout << "image resolution is halved beyond a size of "
             << m_BrowseData->getHalveBeyondSize() << endl;
    } else if(command == "analysis") {
        intptr_t para=0; fromString(value, para);
        startThread(DO_ANALYSIS, (void*)para);
    } else if(command == "noise") {
        cout << "going for noise" << endl;
        float para=255.f; fromString(value, para);
        m_BrowseData->addNoise(para);
    } else {
        cout << "command not recognized" << endl;
        return false;
    }
    return true;
}
bool Brain::switchState(dword state) {
    if(state < m_CStates->size()) {
        m_CurState = state;
        m_Geom = (*m_CStates)[state];
        return true;
    }else {
        m_CurState = 0;
        m_Geom = (*m_CStates)[0];
        return false;
    }
}

dword Brain::duplicateState(const Model *model) {
    if(!model) model = m_Geom;
    m_Geom = new Model(*model);
    m_CStates->push_back(m_Geom);
    m_CurState = m_CStates->size()-1;
    return m_CurState;
/*
  m_CStates->push_back(new Model(*m_Geom));
  return m_CStates->size()-1;
  TRACE(m_CurState);
*/
}

void Brain::killState(dword state) {
    if(state>=m_CStates->size()) return;
    if(m_CStates->size() == 1) return;
    else {
        delete (*m_CStates)[state];
        m_CStates->erase(m_CStates->begin()+state);
    }
    if(state<=m_CurState) m_CurState--;
    switchState(m_CurState);
}

void Brain::killAllStates() {
    //done in sub-procedure vuLock glock(m_GeomMutex);
    while(m_CStates->size()>1) {
        if(m_CurState==0) killState(1);
        else killState(0);
    }
}

int Brain::distributeModel(const Model &model, int n, float qth, bool count)
{
    int cstate = m_CurState;
    int i;
    int cs=0;
    int dist = (int)model.getStdRadius()+1;
    int sx = m_Data->getDim1Size()/dist;
    int sy = m_Data->getDim2Size()/dist;
    if(n==0) {  // jittering
        int j;
        for(j=0; j<m_Data->getDim2Size(); j+=dist)
            for(i=0; i<m_Data->getDim1Size(); i+=dist)
            {
                duplicateState(&model);
                Point p(i,j);
                p += Point(frand(dist), frand(dist)); // jittering
                m_Geom->translate(p-m_Geom->getCenter());
                m_Geom->rotate(frand(2*M_PI));
                m_Geom->scale(max(fgauss01()*0.3f+1.f, 0.1f),true);
                m_Geom->setShapeWeight(__SHAPEWEIGHT);
                if(m_Geom->getQualityOfFit() < qth) {
                    killCurrentState();
                } else cs++;
            }
    } else { // monte carlo
        if(n<0)
            n = (int)(m_Data->getDim1Size()*m_Data->getDim2Size()/(dist*dist));
        for(i=0; i<n; i++) {
            duplicateState(&model);
            Point p(frand(m_Data->getDim1Size()),  // monte carlo
                    frand(m_Data->getDim2Size()));
            m_Geom->translate(p-m_Geom->getCenter()); //random position
            m_Geom->rotate(frand(2*M_PI)); //rand. orient.
            m_Geom->scale(max(fgauss01()*0.3f+1.f, 0.1f),true);
            m_Geom->setShapeWeight(__SHAPEWEIGHT);
            if(m_Geom->getQualityOfFit() < qth) {
                killCurrentState();
                if(count) i--;
            } else cs++;
        }
    }
    switchState(cstate);
    return cs;
}

void Brain::drawAllModels() const {
    vuLock glock((vuMutex&)m_GeomMutex);
    vuLock dlock((vuMutex&)m_DataMutex);
//draw winners
    m_Search.draw();
    m_SModel.draw();
    if(m_DrawOwnState) {
        if(m_CStates->empty()) return;
        float lqof,hqof;
        getQualityRange(lqof,hqof);
        if(lqof>0) lqof=0;
        //TRACE("qmin="<<lqof<<" qmax="<<hqof);
        if(!m_DesignMode) {
//draw instances
            glLineWidth(1.0f);
            for(vector<Model*>::const_iterator m = m_CStates->begin();
                m!=m_CStates->end(); m++)
            {
                if(m_Geom != *m) {
                    float alpha = (hqof!=0.0) ? ((*m)->getQualityOfFit()-lqof)/hqof :0;
                    alpha = alpha*0.8+0.2;
                    glColor4f(0.1,0.2,0.9,alpha);
                    (*m)->draw();
                }
            }

        } else {  // is design mode
            glColor3f(1,1,0);
            sglBitmapString("design mode",10,20);
        }

//draw selected one
        float alpha=0.0;
        if(hqof!=0.0)
            alpha = (m_Geom->getQualityOfFit()-lqof)/hqof;
        alpha = alpha*0.6+0.4;
        glEnable(GL_LINE_SMOOTH);    // looks nice but BIG performace killer
        //glColor4f(0.0f, 0.0f, 1.0f,alpha);
        glColor4f(0.0f, 0.0f, 0.0f,alpha);
        glLineWidth(5.0f);
        m_Geom->draw(m_DesignMode);
        glLineWidth(3.0f);
        glColor4f(1.0f, 1.0f, 1.0f,alpha);
        m_Geom->draw(m_DesignMode);
        glDisable(GL_LINE_SMOOTH);    // looks nice but BIG performace killer
        //cout << (m_Geom->getQualityOfFit()) << endl;

//draw names
#ifdef _DRAW_MODEL_NAMES_
        for(vector<Model*>::const_iterator m = m_CStates->begin();
            m!=m_CStates->end(); m++)
        {
            if((*m)->isWinner() || (*m) == m_Geom ) {
                Point c = (*m)->getCenter();
                stringstream descr;
                descr << (*m)->getName();// << " " << (*m)->getLiveliness();
                sglBitmapStringOutlined(descr.str().c_str(),(int)c.x,(int)c.y);
            }
        }
#endif
    } // of draw own state
    {
        glLineWidth(1.0f);
        glColor3f(1,1,1);
        map<string,StructPath>::const_iterator spath =
            m_SModel.getIPaths().find(m_SModel.getShownIP());
        if(spath != m_SModel.getIPaths().end()) {
            stringstream descr;
            descr << m_SModel.getShownIP() << " ";
            descr << spath->second.getRelGoodness();
            sglBitmapString(descr.str().c_str(),10,m_Data->getDim2Size()-20);
        }
//    sglBitmapString(m_CurPrototype.c_str(),10,m_Data->getDim2Size()-20);
    }
    if(m_Data->getFilename() != m_BrowseData->getFilename()) {
        glColor3f(1,1,0);
        sglBitmapString("Press [Enter] to attach sensors to image.",10,40);
    }
}

void Brain::evolve(float dt)
{
    if(m_Done || !m_Data->initialized()) return;

    static float t=0;
    t+=dt;

    m_SModel.stepSearch(dt);

    static float search_evolve_t = 0;
    if(t-search_evolve_t > getSearchPara().m_EvolveCycle) {
        m_Search.evolve();
        search_evolve_t = t;
    }
    m_Search.step(dt);

    return;

// determine range of qualities
    float lqof,hqof;
    getQualityRange(lqof,hqof);
    //if(lqof>0) lqof=0;

//remove bad ones
    static float kill_t = 0;
    if(t-kill_t > 6) {
        float kill_quality_th = 0.1;
        for(int mi=0; mi<(int)m_CStates->size()
                && (int)m_CStates->size()>1;)
        {
            Model *m = (*m_CStates)[mi];
            float relq = (hqof!=0.0) ? (m->getQualityOfFit()-lqof)/hqof :1;
            if(relq < kill_quality_th) {
                //TRACE(relq);
                killState(mi);
            }else mi++;
        }
        kill_t = t;
    }

//create some (not used yet)
    //TRACE("min "<<lqof<<" max "<<hqof);
    if(0 && hqof-lqof < 0.001 && m_Geom->getNNodes()>0) {
        static bool duplicated=false;
        if(duplicated && hqof-lqof>0) m_Done = true;
        else {
            duplicated = true;
            int cstate = m_CurState;
            int dist = (int)(1.5*m_Geom->getStdRadius());
            int i,j;
            for(j=dist/2;j<m_Data->getDim2Size(); j+=dist)
                for(i=dist/2;i<m_Data->getDim1Size(); i+=dist) {
                    duplicateState();
                    m_Geom->translate(Point(i,j)-m_Geom->getCenter());
                }
            switchState(cstate);
        }
    }

//randomize bad ones
    static float laze_t = 0;
    //static bool lazing = true;
    static float change_t = 0;
    if(t-change_t > 0.2)
    {
        if(t-laze_t < 1) {
            float change_quality_th = 0.7;
            for(vector<Model*>::iterator m=m_CStates->begin();
                m!=m_CStates->end();m++)
            {
                float relq = (hqof!=0.0) ? ((*m)->getQualityOfFit()-lqof)/hqof :1;
                //TRACE((*m)->getLiveliness());
                if(relq < change_quality_th && (*m)->getLiveliness()<1) {
                    (*m)->pushRotate((*m)->getCenter(), -1+frand(2));
                    (*m)->addNoise(0.05);
                }
            }
        } else if(t-laze_t > 3) laze_t = t;
        change_t = t;
    }
}

void Brain::getQualityRange(float &lqof, float &hqof, float shapeweight) const
{
    bool cw = (shapeweight!=-1);
    if(cw) (*m_CStates->begin())->setShapeWeight(shapeweight);
    lqof=hqof=(*m_CStates->begin())->getQualityOfFit();
    for(vector<Model*>::const_iterator m = m_CStates->begin();
        m!=m_CStates->end(); m++)
    {
        if(cw)(*m)->setShapeWeight(shapeweight);
        if((*m)->getQualityOfFit()>hqof) hqof=(*m)->getQualityOfFit();
        if((*m)->getQualityOfFit()<lqof) lqof=(*m)->getQualityOfFit();
    }
}

void Brain::setupSearch(enum SearchWhat what, const PropVec& prop)
{
    ExpectationMap& cem = m_Search.getExpectationMap();
    //ExpectationMap::EDistList& cdl=cem.getDistList();
    if(what == SW_SET && cem) {
        cem.setRepresentative(*m_Geom);
    } else {
        ExpectationMap em(*m_Geom);
        PropVec propl(0.), propu(0.);
        if(what != SW_RESET && cem) {
            propu = cem.getUB();
            propl = cem.getLB();
        } else {
            Point2D dims((float)m_Data->getDim1Size(),
                         (float)m_Data->getDim2Size());
            float scale = m_Geom->getStdRadius();
            setPropPos(propu, dims);
            setPropDir(propu, 2*M_PI);
            setPropScale(propu, scale*2);
            setPropScale(propl, scale*0.5);
        }
        if(what == SW_UB) propu = prop;
        else if(what == SW_LB) propl = prop;
        else if(what == SW_SPREAD) {
            float radius = getPropScale(prop);
            PropVec plusminus;
            setPropPos(plusminus, Point2D(radius, radius));
            setPropScale(plusminus, getPropScale(prop)*0.2);
            setPropDir(plusminus, M_PI*0.2);
            propu = prop+plusminus;
            propl = prop-plusminus;
        }
        ExpectationMap::correctLBUB(propl, propu);
        em.setLB(propl);
        em.setUB(propu);
        em.add(new EMDRect(propl,propu));
        m_Search.setExpectationMap(em);
    }
    DUMP(m_Search.getExpectationMap().getLB());
    DUMP(m_Search.getExpectationMap().getUB());
}
