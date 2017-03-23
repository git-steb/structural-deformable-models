#ifndef _SEARCHER_H_
#define _SEARCHER_H_
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <functional>
#include <algorithm>
#include "ExpMap.h"
#include "Model.h"

class ParseFile;

/** Engine performing genetic search to determine a set of local winners.
 */
class Searcher {
    typedef std::list<Model*> MBin;
    typedef const std::list<Model*> CMBin;
public:
    Searcher();
    Searcher(const Searcher& rhs);
    ~Searcher();

    Searcher& operator=(const Searcher& rhs);
    bool step(float dt);
    void evolve();
    void startSearch(bool dostart=true);
    bool searchDone() const { return m_Mode == MODE_DONE; }
    template<class Iter> inline unsigned int getWinners( Iter iter ) const;
    std::map<dword,Winner>& getWinList() { return m_Winners; }
    const std::map<dword,Winner>& getWinList() const { return m_Winners; }
    float getBestQOF() const;
    float getLeastQOF() const;
    float relativeQOF(float qof) const;
    const Winner* getWinner(dword id) const;
    void setExpectationMap(const ExpectationMap &em);
    ExpectationMap& getExpectationMap() { return m_EMap; }
    const ExpectationMap& getExpectationMap() const { return m_EMap; }
    void clear();
    void draw() const;
    bool triggerTest(int mx, int my, int what);
    operator bool() const { return (bool)m_EMap; }
    const Model& getRepresentative() const { return m_Representative; }
    void reattachSensors();
    float getGeneration(const Model* model) const;
    void setShapeWeight(float shapeweight) { m_ShapeWeight = shapeweight; }
    float getShapeWeight() const { return m_ShapeWeight; }
protected:
    std::map<dword,Winner>& updateWinList();
    inline int getBindex(int bx, int by, bool &culled=*((bool*)NULL)) const;
    inline int getBindex(const PropVec &prop,bool& culled=*((bool*)NULL))const;
    inline int getBindex(const Model &model, bool &culled=*((bool*)NULL))const;
    //getQualityRange - min,max or avg,stdev?
    void getQualityRange(float &lqof, float &hqof, float shapeweight=-1) const;
    void findWinners();
    bool buildBins(float bindist=0.f);
    int buildClusters();
    void selectWinners();
    void cleanFlags();
    std::list<Model*>::iterator& remove(std::list<Model*>::iterator &mi);
    int add(Model* m);
    Model& mutate(Model& model, float rate=1) const;
    int distribute(int n=-1, float qth=0, bool count=false);
protected:
    Model                       m_Representative;
    PropVec                     m_AvgWinner, m_StdWinner;
    PropVec                     m_MinModel, m_MaxModel;
    ExpectationMap              m_EMap;
    std::list<Model*>           m_Population;
    int                         m_NPop;
    std::vector< MBin >         m_Bins;
    int                         m_BinsX, m_BinsY;
    float                       m_BinDist;
    std::list< MBin >           m_Clusters;
    int                         m_NClusters;
    MBin*                       m_CurCluster;
    float                       m_Time;
    Point                       m_PosMax, m_PosMin, m_PosRange;
    enum { MODE_PAUSE, MODE_RUN, MODE_DONE } SMode;
    dword                       m_Mode;
    float                       m_EvolveT;
    float                       m_MutateRate;
    std::map<dword,Winner>      m_Winners;
    dword                       m_NextWinID;
    float                       m_ShapeWeight;
};

//-----------------------------------------------------------------------------
template<class Iter>
inline unsigned int Searcher::getWinners( Iter iter) const
{
    unsigned int nwin=0;
    for(std::list<Model*>::const_iterator wi=m_Population.begin();
        wi != m_Population.end(); wi++)
    {
        if((*wi)->isWinner()) {
            *iter++ = *wi;
            nwin++;
        }
    }
    return nwin;
}

inline int Searcher::getBindex(int bx, int by, bool &culled) const
{
    bool tb;
    bool &cf = &culled != NULL ? culled : tb; // redirect to temporary bool?
    cf = false;
    if(bx<0) { bx=0; cf = true; }
    else if(bx>=m_BinsX) { bx=m_BinsX-1; cf = true; }
    if(by<0) { by=0; cf = true; }
    else if(by>=m_BinsY) { by=m_BinsY-1; cf = true; }
    return by*m_BinsX+bx;
}

inline int Searcher::getBindex(const PropVec &prop, bool &culled) const
{
    return getBindex((int)prop[0], (int)prop[1], culled);
}

inline int Searcher::getBindex(const Model &model, bool &culled) const
{
    //PropVec prop=model.getProperties();
    PropVec prop(0.);
    setPropPos(prop, model.getCenter());
    return getBindex(prop, culled);
}

inline float Searcher::getBestQOF() const
{
    return m_Population.empty()? 1.f
        :(*m_Population.begin())->getQualityOfFit();
}

inline float Searcher::getLeastQOF() const
{
    return m_Population.empty()? 0.f
        :(*m_Population.rbegin())->getQualityOfFit();
}

inline float Searcher::relativeQOF(float qof) const
{
    const float mq = getBestQOF(), lq = getLeastQOF();
    float range = mq-lq; if(range <= 0.f) range = 1.f;
    return (qof-lq)/range;
}

//----------------------------------------------------------------------------
class SearcherParams {
public:
    SearcherParams();
    std::ostream& write(std::ostream& os, bool showcomment=false) const;
    bool read(ParseFile& is);
    friend std::ostream& operator<<(std::ostream& os,
                                    const SearcherParams& rhs)
    { return rhs.write(os); }
    friend std::istream& operator>>(std::istream& is, SearcherParams& rhs)
    { ParseFile pf(is); rhs.read(pf); return is; }
    static SearcherParams global;
public:
    dword m_NSpawns;            //!< number of new spawned shapes at each cycle
    float m_NSpawnsTHRed;       //!< threshold reduction factor for new spawns
    dword m_MaxPop;             //!< maximum population count
    dword m_NClusters;          //!< number of clusters (winners) determined at each cycle
    float m_ClusterTH;          //!< least quality of cluster relative to the best
    float m_ClusterDPos;        //!< maximum distance of shapes in a cluster (relative to shape radius)
    float m_ClusterDScale;      //!< maximum relative difference in scale to cluster representative
    float m_ClusterDDir;        //!< maximum difference in direction in $\pi$
    float m_ClusterMeltDist;    //!< maximum Hausdorff distance in mm for melting two shapes
    float m_RelBinDist;         //!< distance of bins relative to representative radius

    float m_EvolveCycle;        //!< time in seconds for one evolution cycle
    float m_MutateRate;         //!< initial mutation rate
    float m_MutateHL;           //!< half-life of mutation rate in seconds
    float m_MutateTHRed;        //!< threshold reduction factor for mutated spawns
    float m_ScaleStd;           //!< scale stdev of link
    dword m_MutateSpawns;       //!< number of spawns when mutating a shape
    float m_MutatePDir;         //!< probability of mutation of orientation
    float m_MutatePPos;         //!< probability of mutation of position
    float m_MutatePNoise;       //!< probability of mutation of node positions
    float m_MutateNoiseRate;    //!< distortion ratio for node positions
    float m_MutatePProp;        //!< probability of mutation of proportion
    float m_MutatePropRate;     //!< rate of adaption of proportion

    float m_ShapeWeight;        //!< influence of shape deformation on QOF

    float m_RatingTH;           //!< least allowed rating for a winner
    float m_PCTH;               //!< threshold for principle components
    int   m_ParaSolver;         //!< method for solving ODE, 0-gauss, 1-midpnt
    int   m_PDist;              //!< type of probability distr. to represent structural relationships
    int   m_MaxShoot;           //!< maximum number of spawns from an exp map
};

inline SearcherParams& getSearchPara() { return SearcherParams::global; }

#endif
