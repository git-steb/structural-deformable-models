#ifndef _MSTRUCT_H_
#define _MSTRUCT_H_

#include <iostream>
#include <string>
#include <map>
#include <list>
#include <vector>
#include "common.h"
#include "ParseFile.h"
#include "Model.h"
#include "ExpMap.h"
#include "Searcher.h"
#include "FFind.h"

//----------------------------------------------------------------------------
class StructTable;
class MStructure;

//----------------------------------------------------------------------------
class SubStructure
{
 public:
    enum DistrType {EMD_PCA=0, EMD_GAUSS, EMD_RECT, EMD_RECTTF};
    enum Modes     {MODE_NONE=0, MODE_NOSPAWN=2, MODE_SHOWSTATS=4 };

    SubStructure(const std::string& subStructName="", 
                 const std::string& supStructName="",
                 const PropTF& transform=PropTF(0.f), 
                 int pivot=-1);
    void clear();
    void analyseTF(const DMatrixf& tfmat);
    EMDistribution* generateEMDist(const PropVec& wprop, float ppmm,
                                   bool inverse = false,
                                   dword dtype = EMD_RECT) const;
    void getLBUB(PropVec& lb, PropVec& ub, 
                 const PropVec& wprop, float ppmm, float radius = 3) const;
    bool read(ParseFile& is);
    friend std::ostream& operator<<(std::ostream& os, const SubStructure& ss);

    std::string         m_SubStructName, m_SupStructName;
    PropTF              m_Transform;
    int                 m_Pivot;
    bool                m_Polar;
    PropVec             m_Mean, m_Stdev;
    DMatrixf            m_PC, m_Sigma, m_IPC;
    float               m_RateWeight, m_RateTH;
    dword               m_Mode;
};

//----------------------------------------------------------------------------
class MStructure
{
    friend class StructTable;
 public:
    MStructure(const std::string& name="", StructTable* psTable=NULL);
    MStructure(const MStructure& rhs);
    ~MStructure();
    void clear();
    MStructure& operator=(const MStructure& rhs);

    void setName(const std::string& name);
    const std::string& getName() const { return m_Name; }
    void setSize(float size) { m_Size = size; }
    float getSize() const { return m_Size; }
    void setModel(const Model& model) { m_Model = model; }
    const Model& getModel() const { return m_Model; }
    Model& getModel() { return m_Model; }
    void setWeight(float weight) { m_Weight = weight; }
    float getWeight() const { return m_Weight; }
    void attachStructTable(StructTable* psTable) { m_PStructTable = psTable;}
    StructTable* getStructTable() { return m_PStructTable; }
    const Searcher& getSearcher() const { return m_Searcher; }
    const Winner* getWinner(dword wid) const;
    bool isFrame() const { return !m_Model; }
    operator bool() const { return !m_Name.empty(); }

    void addSubStruct(const SubStructure& substruct);
    const SubStructure& getSubStruct(const std::string sname) const
        { return m_SubStructures.find(sname)->second; }
    bool hasSubStruct(const std::string sname) const
        { return m_SubStructures.find(sname) != m_SubStructures.end(); }
    const SubStructure& getSupStruct(const std::string sname) const
        { return *m_SupStructures.find(sname)->second; }
    bool hasSupStruct(const std::string sname) const
        { return m_SupStructures.find(sname) != m_SupStructures.end(); }

    bool read(ParseFile &is);
    void write(std::ostream &os) const;
    friend ParseFile& operator>>(ParseFile& is, MStructure& st) 
        { st.read(is); return is; }
    friend std::ostream& operator<<(std::ostream& os, const MStructure& st) 
        { st.write(os); return os; }
    void showStats(const SubStructure& subs, std::ostream& os=std::cout) const;

    std::string getInfoFilename(const std::string& suffix) const;
    bool getRefModel(dword id, Model& model) const;
    void setRefModel(dword id, const Model& model);
    bool getRefProp(dword id, PropVec& prop) const;
    void setRefProp(dword id, const PropVec& prop);
    template<class Iter> dword getRefModelIDs(Iter iter) const;
    dword loadRefProp();
    void saveRefProp() const;

    void buildAllStats();
    bool buildMasterModel(float scscale=1.f);
    void rebuildExpMap();
    void verifyWinnerRating();
    const ExpectationMap& getExpMap() const 
        { return m_Searcher.getExpectationMap(); }
    bool stepSearch(float dt);

 protected:
    void refSubSuper(bool doclear=false);
    bool addExpectation(const SubStructure& subs, 
                        ExpectationMap& expmap, bool inverse=false) const;
 protected:
    StructTable*        m_PStructTable;
    std::string         m_Name;
    Model               m_Model;
    float               m_Size;         //!< stdradius in mm
    std::map<std::string, SubStructure>  m_SubStructures;
    std::map<std::string, SubStructure*> m_SupStructures;
    std::map<dword,PropVec>  m_RefProp;
    Searcher            m_Searcher;
    float               m_Weight;
    Winner              m_FrameWinner;
};

//------------------- INLINES ---------------------------------------------
template<class Iter> dword MStructure::getRefModelIDs(Iter iter) const {
    dword count = 0;
    if(!m_Model) {
        for(std::map<dword,PropVec>::const_iterator rp=m_RefProp.begin();
            rp!=m_RefProp.end(); rp++, count++)
            *iter++ = rp->first;
        return count;
    }
    std::string filemask = getInfoFilename("*");
    std::list<std::string> files;
    if(FFind::findFiles(filemask, back_inserter(files)))
    {
        for(std::list<std::string>::iterator cf=files.begin();
            cf != files.end(); cf++) {
            std::string suff = cf->substr(cf->rfind('_')+1);
            suff = suff.substr(0, suff.find('.'));
            dword id=0;
            fromString(suff, id);
            if(id) {
                *iter++ = id;
                count++;
            }
        }
    }
    return count;
}

#endif
