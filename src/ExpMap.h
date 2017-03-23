#ifndef _EXPMAP_H_
#define _EXPMAP_H_
#include <list>
#include <map>
#include "Model.h"
#include "VVector.h"
#include "DMatrix.h"
#include "common.h"

class Winner {
public:
    enum ReservedID { WID_EMPTY = 0, WID_FIRST, WID_LAST = 0xfffffdff,
                      WID_FRAME, WID_BLOCKED, WID_FIRSTFREE};
    typedef       std::map<std::string,dword>                   BestRatings;
    typedef const std::map<std::string,dword>                   CBestRatings;
    typedef       std::map<std::string, std::map<dword,float> > Ratings;
    typedef const std::map<std::string, std::map<dword,float> > CRatings;

Winner(Model* m=NULL) : m_WinnerID(WID_EMPTY), m_BestRating(0.f)
    { setModel(m); }
    void setModel(Model* m);
    void setStructName(const std::string& sname) { m_StructName = sname; }
    void clearRatings();
    float rateBy(const std::string& creatorname,dword creatorid, float rating);
    float updateBestRating();
    const BestRatings::mapped_type&
        getBestRatingID(const std::string& sname) const
    { return m_BestRatings.find(sname)->second; }
    const Ratings::mapped_type& getRatingList(const std::string& sname) const
    { return m_Ratings.find(sname)->second; }
    const std::pair<dword,float> getBest(const std::string& sname) const;
    bool hasConnection(const std::string& sname) const
    { return m_Ratings.find(sname) != m_Ratings.end(); }
    float getConnection(const std::string& sname, dword wid) const;
public:
    Model*      m_Model;
    dword       m_WinnerID;
    std::string m_StructName;
    float       m_BestRating;
    Ratings     m_Ratings;
    BestRatings m_BestRatings;
};

class EMDistribution {
public:
    enum EDFlags { ED_NONE=0, ED_OLD=1 };
EMDistribution() : m_Integral(1.f), m_ShootCount(0), m_Flags(ED_NONE) {};
    virtual ~EMDistribution() {}
    virtual PropVec getPropVec() const
    { return PropVec(0.); }
    virtual void setIntegral(float integral)
    { m_Integral = integral; }
    virtual EMDistribution* copy() const {
        return new EMDistribution(*this);
    }
    virtual float ratePropVec(const PropVec& prop, Winner* winner=NULL) const
    { return 0; }
    void setCreator(const Winner& creator) {
        m_Creator = creator;
    }
    void setShootCount(dword scount) { m_ShootCount = scount; }
    dword getShootCount() const { return m_ShootCount; }
    bool hasFlags(dword flags) const { return m_Flags&flags; }
    void setFlags(dword flags) { m_Flags |= flags; }
    void unsetFlags(dword flags=0xffffffff) { m_Flags &= ~flags; }

    float               m_Integral;
    Winner              m_Creator;
    dword               m_ShootCount;
    dword               m_Flags;
};

class EMDRect : public EMDistribution {
public:
EMDRect(const PropVec &lb=PropVec(0.f),
        const PropVec &ub=PropVec(1.f))
    : m_LB(lb), m_UB(ub)
    {}
    virtual ~EMDRect() {}
    virtual EMDistribution* copy()  const
    { return (EMDistribution*) (new EMDRect(*this)); }
    PropVec getPropVec() const;
    float ratePropVec(const PropVec& prop, Winner* winner=NULL) const;
    PropVec& clamp(PropVec &v) { return v.clamp(m_LB, m_UB); }
    const PropVec& getUB() const { return m_UB; }
    const PropVec& getLB() const { return m_LB; }
    void setUB(const PropVec &ub) { m_UB = ub; }
    void setLB(const PropVec &lb) { m_LB = lb; }

    PropVec m_LB, m_UB;
};

class EMDGauss : public EMDistribution {
public:
EMDGauss(const PropVec &avg=PropVec(0.f),
         const PropVec &stdev=PropVec(1.f))
    : m_Avg(avg) { setStdev(stdev); }
    virtual ~EMDGauss() {};
    virtual EMDistribution* copy()  const
    { return (EMDistribution*) (new EMDGauss(*this)); }
    PropVec getPropVec() const;
    float ratePropVec(const PropVec& prop, Winner* winner=NULL) const;
    void setAvg(const PropVec& avg) { m_Avg = avg; }
    void setStdev(const PropVec& stdev);
protected:
    PropVec m_Avg, m_Stdev, m_StdevRate;
};

class EMDXformer : public EMDistribution {
public:
EMDXformer(EMDistribution* input,
           const PropVec& origin=getIdentityPropTF(),
           const DMatrixf& tfmat=DMatrixf())
    : m_InputEMD(input), m_Origin(origin), m_Mean(0.f), m_Stdev(1.f) {
        assert(input != NULL);
        setTFMat(tfmat);
    }
/*     EMDXformer(EMDistribution* input, const DMatrixf& tfmat, */
/*                const PropVec& origin)  */
/*         : m_InputEMD(input), m_Origin(origin) {  */
/*         setTFMat(tfmat);  */
/*     }  */
    virtual ~EMDXformer() {
        delete m_InputEMD; m_InputEMD = NULL;
    }
    virtual EMDistribution* copy()  const {
        EMDXformer* cpy = new EMDXformer(*this);
        cpy->m_InputEMD = m_InputEMD->copy();
        return (EMDistribution*) cpy;
    }
    PropVec getPropVec() const;
    float ratePropVec(const PropVec& prop, Winner* winner=NULL) const;
    void setTFMat(const DMatrixf& mat);
    void setMean(const PropVec& mean) { m_Mean = mean; }
    void setStdev(const PropVec& stdev) { m_Stdev = stdev; }
protected:
    EMDistribution*             m_InputEMD;
    DMatrixf                    m_TFMat, m_ITFMat;
    PropVec                     m_Origin, m_Mean, m_Stdev;
};

class ExpectationMap : public EMDRect {
public:
    typedef std::map<dword,EMDistribution*> EDistributions;

    ExpectationMap() {}
    ExpectationMap(const Model &model)
    { setRepresentative(model); }
    ExpectationMap(const ExpectationMap &rhs) {
        operator=(rhs); }
    virtual ~ExpectationMap() { clear(); }
    ExpectationMap& operator=(const ExpectationMap &rhs);
    virtual EMDistribution* copy()  const;
    float ratePropVec(const PropVec& prop, Winner* winner=NULL) const;
    void setRepresentative(const Model &model);
    Model& getRepresentative() { return m_Representative; }
    void add(EMDistribution *ed);
    void add(EMDistribution *ed, dword wid);
    void add(const EDistributions &distlist);
    bool erase(dword wid);
    bool hasDist(dword wid) const;
    EMDistribution* getEDist(dword wid);
    const EMDistribution* getEDist(dword wid) const;
    EDistributions& getDistList() { return m_Distributions; }
    virtual PropVec getPropVec() const;
    Model* generateInstance() const;
    float updateIntegral();
    void clear(bool oldonly=false);
    void markAllOld();
    dword getShootCount(dword wid) const;
    operator bool() const { return !m_Distributions.empty(); }
    static void correctLBUB(PropVec& lb, PropVec& ub);

protected:
    Model                               m_Representative;
    EDistributions                      m_Distributions;
    std::map<float, EMDistribution*>    m_SortDist;
};

//-----------------------------------------------------------------------------
//affine or linear or no property vector transformation (homogeneous coord.)
inline PropVec& hgPropTF(const DMatrixf& tfmat, PropVec& prop)
{
    if(!tfmat.empty() && tfmat.sizeX() == tfmat.sizeY()) {
        DMatrixf propDM(1,PropVec::size(), prop.begin());
        if(tfmat.sizeX() == propDM.sizeY()) { // linear
            propDM = propDM.mulLeft(tfmat);
            prop = &*propDM.getData().begin();
        } else if(tfmat.sizeX() == propDM.sizeY()+1) {//affine (homogeneous)
            DMatrixf propDMH(1,propDM.sizeY()+1, 1.f);
            propDMH.setRange(0,0,propDM);
            propDMH = propDMH.mulLeft(tfmat);
            prop = &*propDMH.getData().begin();
        }
    }
    return prop;
}

#endif
