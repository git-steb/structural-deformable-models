/* -*- C++ -*- */
#ifndef _BRAIN_H_
#define _BRAIN_H_


#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

#include <list>
#include <map>
#include <vector>

#include "vuThread.h"
#include "common.h"
#include "Model.h"
#include "SensorColl.h"
#include "SpeciesDB.h"
#include "Searcher.h"
#include "StructTable.h"
#include "Data.h"

#define TIMESTEP 0.02

class Brain : public vuThread {
 public:
    Brain();
    ~Brain();

    bool load(const char *bfile);
    bool loadModel(const char* filename);
    bool loadData(const char* filename, dword ppmm=0);
    bool loadDB(const char* filename);
    bool loadDBSelector(const char* filename);
    bool loadSpecies(const Species& spec);

    const Model& getModel() {return *m_Geom;}
    const Dataset& getData() const;
    Dataset& getSensorData();

    void attachBrowseData();

    void drawAllModels() const;
    bool triggerTest(int mx, int my, int what=0);
    bool doCommand(const std::string& command, const std::string& value,
                   int mx=0, int my=0);
    bool isDone() const { return m_Done; }

    //! some options for run(whatsup)
    enum RunWhat { DO_ANIMATE, DO_NOTHING, DO_ANALYSIS, DO_LAST};
    //! virtual from vuThread
    void run(int whatsup, void* data);
    
    //! duplicate state of current model
    //! \returns ID of copy
    dword duplicateState(const Model *model = NULL);
    
    bool switchState(dword newstate);
    void killState(dword state);
    void killAllStates();
    void killCurrentState() { killState(m_CurState); }
    void toggleUpdateForAll(bool onoff=true) { m_UpdateAllModels=onoff; }

    void evolve(float dt);

    void findWinners();

    const SensorCollection& getSensors() const { return m_Sensors; }

    void getQualityRange(float &lqof, float &hqof, float shapeweight=-1) const;
    int distributeModel(const Model &model, 
                        int n=-1, float qth=0, bool count=false);
    /** change container to prototype of name mname */
    bool changeModel(const std::string &mname);
    /** create a new instance 'all' containing copies of all instances.*/
    bool mergeInstances();
    template<class Iter>
    dword getAllModels( Iter iter );
    vuMutex& getGeomMutex() { return m_GeomMutex; }
protected:
    int getBindex(int bx, int by, bool &culled=*((bool*)NULL));
    enum SearchWhat {SW_UB, SW_LB, SW_RESET, SW_SET, SW_SPREAD };
    void setupSearch(enum SearchWhat what=SW_RESET, 
                     const PropVec& prop=PropVec());
protected:
    Dataset		m_Data;
    Dataset             m_BrowseData;
    SensorCollection	m_Sensors;
    Model		*m_Geom;
    std::vector<Model*>                         *m_CStates;
    std::map<std::string,Model*>                m_Prototypes;
    std::map<std::string, std::vector<Model*> > m_Instances;
    std::string         m_CurPrototype;
    dword		m_CurState;
    SpeciesDB           m_DB;
    DBSelector          m_DBSelector;
    Species             m_CSpecies;
    float		m_TimeStep;
    float		m_TimeScale;
    bool		m_UpdateAllModels;
    vuMutex		m_GeomMutex, m_DataMutex, m_RunMutex[DO_LAST];
    bool                m_ThreadActive[DO_LAST];
    bool		m_Done;
    bool		m_DesignMode;
    bool                m_ImmediateData;
    bool                m_DrawOwnState;
    Searcher            m_Search;
    std::list<Model>    m_Winners;
    StructTable         m_SModel;
};

template<class Iter>
inline dword Brain::getAllModels( Iter iter )
{
    dword nm=0;
    if(m_CStates)
        for(std::vector<Model*>::iterator mi = m_CStates->begin();
            mi != m_CStates->end(); mi++)
        {
            *iter++ = *mi;
            nm++;
        }
//     for(std::map<std::string, std::vector<Model*> >::iterator 
//         ti=m_Instances.begin(); ti != m_Instances.end(); ti++) {
//         for(std::vector<Model*>::iterator gi=ti->second.begin();
//             gi!=ti->second.end(); gi++)
//         {
//             *iter++ = *gi;
//             nm++;
//         }
//     }
//     for(std::map<std::string,Model*>::iterator gi=m_Prototypes.begin();
//         gi!=m_Prototypes.end(); gi++)
//     {
//         *iter++ = gi->second;
//         nm++;
//     }
    return nm;
}

#endif
