#ifndef _STRUCTABLE_H_
#define _STRUCTABLE_H_

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <list>
#include <vector>
#include "common.h"
#include "ParseFile.h"
#include "Model.h"
#include "ExpMap.h"

#include "MStruct.h"

class Brain;
class StructTable;

//----------------------------------------------------------------------------
class StructPath : public std::map<std::string,dword>
{
public:
    StructPath();
    StructPath(const StructPath& rhs);
    ~StructPath();
    StructPath& operator=(const StructPath& rhs);
    void clear();
    Winner* getWinner(MStructure& struc);
    const Winner* getWinner(const MStructure& struc) const;
    float getGoodness() const { return m_Goodness; }
    void protectWinners(StructTable& structs);
    void unprotectWinners(StructTable& structs);
    void blockOtherPaths(const std::set<std::string>& freepath,
                         const StructTable& structs);
    std::ostream& print(std::ostream& os, const StructTable& structs) const;
    float getRelGoodness() const
    { return m_Maxness != 0.f ? m_Goodness/m_Maxness : 0.f; }
    StructPath branch() const;
    void merge(const StructPath& mp);
public:
    float       m_Goodness;             //!< goodness of path
    float       m_Maxness;              //!< maximum goodness
protected:
    bool        m_WinnersProtected;
};

//----------------------------------------------------------------------------
class StructTable {
public:
    typedef std::map<std::string, std::set<std::string> > Interpretations;
    typedef std::map<dword, StructPath>                   NodePaths;
public:
    StructTable(Brain& brain, const std::string& filename=std::string());
    ~StructTable();
    void clear();
    void attachBrain(Brain& brain) { m_Brain = &brain; }
    dataset_ptr getDataset();
    SensorCollection* getSensors();
    void reattachSensors();
    void adaptDataScale();
    bool load(const std::string& filename);
    bool read(ParseFile& is);
    void write(std::ostream &os) const;
    friend ParseFile& operator>>(ParseFile& is, StructTable& st)
    { st.read(is); return is; }
    friend std::ostream& operator<<(std::ostream& os, const StructTable& st)
    { st.write(os); return os; }
    const std::string& getFilename() const { return m_FileName; }
    const std::string& getPath() const { return m_Path; }
    std::map<std::string, MStructure>& getStructs() { return m_Structs; }
    const std::map<std::string, MStructure>& getStructs() const
    { return m_Structs; }
    bool hasStructure(const std::string& sname) const;
    const MStructure& getStructure(const std::string& sname) const;
    MStructure& getNextStruct(int dir=1, bool wrap=true);
    void evalRelations();
    void rebuildExpMaps();
    void draw() const;
    void startSearch(bool dostart=true);
    int searchDone() const {
        if(m_State&ST_DONE) return 1;
        if(m_State&ST_TIMEOUT) return -1; else return 0;
    }
    bool stepSearch(float dt);
    bool triggerTest(int mx, int my, int what);
    static DMatrixf& adjustByAvgDir(DMatrixf& mat);
    float findBestConnection();
    bool findBestConnection(StructPath& path,
                            const Winner& winner) const;
    const StructPath& getBestPath() const { return m_BestPath; }
    const std::string& getBestInterpretation() const { return m_BestIP; }
    const std::map<std::string,StructPath>& getIPaths() const
    { return m_IPaths; }
    void setShownIP(const std::string& curip) const
    { m_CurIP = curip; }
    const std::string& getShownIP() const
    { return m_CurIP.empty() ? m_BestIP : m_CurIP; }
    bool showNextIP(int dir=1) const;
protected:
    void connectSubSuper();
    bool readInterpretations(ParseFile& is);

protected:
    enum TimeMarks { TM_UPDEM, TM_TIMEOUT, TM_LAST };
    enum StateFlags { ST_RUN=1, ST_RESULT=2, ST_DONE=4, ST_TIMEOUT=16 };
    std::map<std::string, MStructure>   m_Structs;
    std::string                         m_FileName;
    std::string                         m_Path;
    Brain*                              m_Brain;
    std::string                         m_CurStruct;
    StructPath                          m_BestPath;
    float                               m_Time;
    dword                               m_State;
    std::vector<float>                  m_TMarks;
    Interpretations                     m_Interpretations;
    std::map<std::string,StructPath>    m_IPaths;
    std::string                         m_BestIP;
    mutable std::string                 m_CurIP;
    mutable std::map<std::string,NodePaths>     m_NodePaths;
};

#endif
