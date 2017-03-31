#include <GL/gl.h>
#include <fxkeys.h>
#include <functional>
#include <algorithm>
#include "ParseFile.h"
#include "Searcher.h"
#include "utils.h"
using namespace std;

//----------------------------------------------------------------------------
#define SRCH_CYCLE_T            1.0f
#define SRCH_MAXPOP             250

#define SRCH_NCLUSTERS          60
#define SRCH_NEW_SPAWNS         100
#define SRCH_NEW_SPAWNS_THRED   0.90f
#define SRCH_CLUSTERTH          0.6f
#define SRCH_CLUSTERDPOS        0.3f
#define SRCH_CLUSTERDMELT       0.05f
#define SRCH_CLUSTERDSCALE      0.3f
#define SRCH_CLUSTERDDIR        0.5f
#define SRCH_RELBINDIST         0.75f

#define SRCH_MUTATE_SPAWNS      0
#define SRCH_MUTATE_THRED       0.99f
#define SRCH_MUTATE_RATE        0.3f
#define SRCH_MUTATE_HL          30
#define SRCH_MUTATE_PDIR        0.5f
#define SRCH_MUTATE_PPOS        0.4f
#define SRCH_MUTATE_PNOISE      0.2f
#define SRCH_MUTATE_NOISERATE   0.1f
#define SRCH_MUTATE_PPROP       0.3f
#define SRCH_MUTATE_PROPRATE    0.5f

#define SRCH_RATINGTH           0.001f
#define SRCH_SHAPEWEIGHT        0.1f
#define SRCH_SCALESTD           1.f

#define PARA_PCTH               0.05f
#define PARA_PDIST              0
#define PARA_SOLVER             0
#define PARA_MAXSHOOT           100

//-----------------------------------------------------------------------------
namespace std {
    template <>
    struct greater<Model*>
    {
        bool operator()(Model* const& lhs, Model* const& rhs) const {
            if(!lhs || !rhs) return true;  //should we do that?
            return lhs->getQualityOfFit() > rhs->getQualityOfFit();
        }
    };
};

class NMerge : public binary_function<Model*, Model*, bool> {
public:
    NMerge( float maxd, enum Model::DistType dtype = Model::DIST_XYS) :
        m_DType(dtype), m_MaxD(maxd) {};
    bool operator() (const Model* lhs, const Model* rhs) const {
        if(lhs && rhs) {
            if(lhs == rhs) return true;
            else {
                float dist = lhs->distance(*rhs, m_DType);
//                 cout << lhs << " merge " << rhs << " dist = " << dist
//                      << " > " << m_MaxD << endl;
                return dist > m_MaxD;
            }
        } else return true;
    }
    const enum Model::DistType m_DType;
    const float m_MaxD;
};

//-----------------------------------------------------------------------------
Searcher::Searcher() : m_NPop(0), m_Time(0), m_Mode(MODE_PAUSE),
                       m_NextWinID(Winner::WID_FIRST),
                       m_ShapeWeight(SRCH_SHAPEWEIGHT)
{
    m_EvolveT = 0.0f;
    m_MutateRate = getSearchPara().m_MutateRate;
    m_ShapeWeight = getSearchPara().m_ShapeWeight;
}

Searcher::Searcher(const Searcher& rhs)
{
    operator=(rhs);
}

Searcher::~Searcher()
{
    clear();
}

Searcher& Searcher::operator=(const Searcher& rhs)
{
    if(&rhs == this) return *this;
    clear();
    m_Representative = rhs.m_Representative;
    m_AvgWinner =  rhs.m_AvgWinner;
    m_StdWinner =  rhs.m_StdWinner;
    m_EMap = rhs.m_EMap;
    for(list<Model*>::const_iterator gi=rhs.m_Population.begin();
        gi != rhs.m_Population.end(); gi++)
    {
        m_Population.push_back(new Model(**gi));
    }
    m_NPop = rhs.m_NPop;
    m_Bins = rhs.m_Bins;
    m_BinsX, m_BinsY = rhs.m_BinsX;
    m_BinDist = rhs.m_BinDist;
    m_Time = rhs.m_Time;
    m_PosMax = rhs.m_PosMax;
    m_PosMin = rhs.m_PosMin;
    m_PosRange = rhs.m_PosRange;
    m_Mode = rhs.m_Mode;
    m_EvolveT = rhs.m_EvolveT;
    m_MutateRate = rhs.m_MutateRate;
    m_NextWinID = rhs.m_NextWinID;
    m_Winners = m_Winners;
    m_ShapeWeight = rhs.m_ShapeWeight;
    updateWinList();
    return *this;
}

void Searcher::clear() {
    m_BinsX = m_BinsY = 0;
    m_Bins.clear();
    for(list<Model*>::iterator gi=m_Population.begin();
        gi != m_Population.end(); gi++)
    {
        delete *gi;
    }
    m_Population.clear();
    m_Representative.clear();
    m_EMap.clear();
    m_Winners.clear();
    m_NPop=0;
    m_Time=0;
    m_EvolveT=0;
}

int Searcher::add(Model *m) {
    m->m_TimeStamp = m_Time;
    m->getFlags() = 0;
    m->setID(Winner::WID_EMPTY);
    m->setInstCount(1);
    m_Population.push_back(m);
    return ++m_NPop;
}

list<Model*>::iterator& Searcher::remove(list<Model*>::iterator &mi)
{
    if(!(*mi)->hasFlags(Model::ST_NODEL)) {
        list<Model*>::iterator ci = mi++;
        delete *ci;
        m_Population.erase(ci);
        m_NPop--;
    }
    return mi;
}

void Searcher::evolve()
{
    if(m_Mode == MODE_PAUSE) return;
//create, mutate, and remove
    int extrakill=0;
    float lqof,hqof;
    getQualityRange(lqof,hqof);
    double slowlive=0.0;
    if(m_NPop) {
        for(list<Model*>::const_iterator m = m_Population.begin();
            m!=m_Population.end(); m++)
        {
            //if((*m)->m_TimeStamp-m_Time > 4&&(*m)->getLiveliness()>slowlive)
            slowlive += (*m)->getLiveliness();
        }
        slowlive /= m_NPop;
        slowlive *= 0.5;
        DUMP(slowlive);
    }

    if(buildClusters()) {
        selectWinners();
        cleanFlags();
    }
    //float kill_quality_th = hqof*0.7;
    list<Model*>::iterator m=m_Population.begin();
    dword rank = 0;
    bool pastlast=false;
    int lamekill=0;
    m_Winners.clear();
    Model* lastmodel = !m_Population.empty() ? m_Population.back():NULL;
    while(m != m_Population.end() && !pastlast)
    {
        if(*m == lastmodel) pastlast=true;
        list<Model*>::iterator nm = m; nm++;
        //float relq = (hqof-lqof)*kill_quality_th+lqof;
        //(hqof!=0.0) ? ((*m)->getQualityOfFit()-lqof)/hqof :1;
        //if(relq > (*m)->getQualityOfFit() && m_NPop>1) {
        if(rank > getSearchPara().m_MaxPop) {
            if(m_NPop>1) { remove(m); extrakill++; }
        } else if((*m)->isWinner()) {
            PropVec propvec((*m)->getProperties());
            if(!(*m)->getID()) (*m)->setID(m_NextWinID++);
            Winner winner(*m);
            float rating = m_EMap.ratePropVec(propvec, &winner);
            if(rating < getSearchPara().m_RatingTH &&
               !(*m)->hasFlags(Model::ST_NODEL)) {
//                cout << "removing invalid winner " << endl;
//                     DUMP(rating);
//                     DUMP(propvec);
//                     DUMP(propvec.clamp(m_EMap.m_LB, m_EMap.m_UB));
//                     DUMP((*m)->getName());
                remove(m);
                m_NextWinID--;
            } else
            {
                m_Winners[winner.m_WinnerID] = winner;
//                 m_Winners.insert(pair<dword,Winner>(winner.m_WinnerID,
//                                                     winner));
//mutate winners
                float threduce = getSearchPara().m_MutateTHRed;
                float qth = (*m)->getQualityOfFit()*threduce;
                int cs=0;
                int misses=0;
                const int nspawns = getSearchPara().m_MutateSpawns;
                while(cs<nspawns) {
                    Model *newmod = &mutate(*(new Model(**m)),m_MutateRate
                                            *exp((m_Time/getSearchPara().m_MutateHL)
                                                 *(-M_LN2)));
                    if(newmod->getQualityOfFit() < qth) {
                        delete newmod;
                        misses++;
                        if(misses%nspawns==0) qth*=threduce;
                    } else {
                        add(newmod);
                        cs++;
                    }
                }
                if(misses>nspawns) DUMP(misses)<<" for mutation"<<endl;
                //nm = m; nm++; // ensure validity of iterator (?)
            }
        } else if((*m)->getLiveliness()<slowlive) {
            remove(m);
            lamekill++;
        } else if(m_NPop>1 && (*m)->isLooser() &&
                  (m_Time-(*m)->m_TimeStamp>getSearchPara().m_EvolveCycle*10))
        {
            remove(m);
        }
        m = nm;
        rank++;
    }
//    DUMP(extrakill);
//    DUMP(lamekill);
//Spawn new instances
    int cs = 0;
    float threduce = getSearchPara().m_NSpawnsTHRed;
    float qth = hqof*threduce; //DEBUG
    int misses = 0;
    const int nspawns = m_EMap && m_EMap.m_Integral > 0.00001f
        ? getSearchPara().m_NSpawns : 0;
    while(cs<nspawns) {
        Model *newmod = m_EMap.generateInstance();
        if(newmod->getQualityOfFit() < qth) {
            delete newmod;
            misses++;
            if(misses%nspawns==0) qth*=threduce;
        } else {
            //qth = newmod->getQualityOfFit();
            //newmod->m_TimeStamp = m_Time; //[Model::TS_CREATE]
            add(newmod);
            cs++;
        }
    }
    //if(misses>20) DUMP(misses) << " for distribution" << endl;
    m_Population.sort( std::greater<Model*>() );
    if(m_NPop > 0 && //(int)m_Winners.size() &&
       m_EMap.m_Integral < 0.0001f)
    {
        m_Mode = MODE_DONE;
    } else cout << m_Representative.getName() << " searching, integral = "
                << m_EMap.m_Integral << endl;
}

bool Searcher::step(float dt)
{
    if(!m_EMap) return true;
    if(m_Mode == MODE_PAUSE) return false;
    else if(m_Mode == MODE_DONE) return true;

    m_Time+=dt;
    for(list<Model*>::iterator m=m_Population.begin();
        m != m_Population.end(); m++)
    {
        (*m)->updateParticles(dt);
    }
    return m_Mode==MODE_DONE; // not yet done
}

float Searcher::getGeneration(const Model* model) const {
    return (m_Time - model->m_TimeStamp) / getSearchPara().m_EvolveCycle;
}

Model& Searcher::mutate(Model& model, float rate) const
{
#define _PROP_MUTATION_
#ifdef _PROP_MUTATION_
    PropVec prop = model.getProperties();
    PropVec mutation(0.f);
    bool same=true;
    while(same) {
        if(FRAND1 < getSearchPara().m_MutatePDir){
            setPropDir(mutation,frand(2*M_PI));
            same = false;
        }
        if(FRAND1 < getSearchPara().m_MutatePPos) {
            Point pos(0,0);
            float radius = model.getStdRadius();//*rate;
            pos += Point2D(radius-frand(2*radius), radius-frand(2*radius));
            setPropPos(mutation,pos);
            same = false;
        }
        if(FRAND1 < getSearchPara().m_MutatePNoise) {
            model.addNoise(getSearchPara().m_MutateNoiseRate*rate);
            same = false;
        }
        if(FRAND1 < getSearchPara().m_MutatePProp) {
            model.adaptProportion(getSearchPara().m_MutatePropRate);
            same = false;
        }
    }
    model.adaptProperties(prop+(mutation*rate));
#else
    float radius=model.getStdRadius();
    model.push(Point2D(-radius+frand(2*radius),
                       -radius+frand(2*radius)));
    model.pushRotate((*m)->getCenter(), -2+frand(4));
    model.addNoise(0.1);
#endif
    return model;
}

void Searcher::setExpectationMap(const ExpectationMap &em)
{
    if(&em != &m_EMap) m_EMap = em;
    PropVec ub = m_EMap.getUB();
    PropVec lb = m_EMap.getLB();
    ExpectationMap::correctLBUB(lb,ub);
    m_EMap.setLB(lb); m_EMap.setUB(ub);
    m_PosMax = getPropPos(ub);
    m_PosMin = getPropPos(lb);
    //cout << "pos max=" << m_PosMax << "  pos min=" << m_PosMin << endl;
    m_PosRange = m_PosMax-m_PosMin;
    m_EMap.updateIntegral();

}

bool Searcher::buildBins(float bindist)
{
    if(m_EMap.getRepresentative().getNNodes() == 0) return false;
    else if(m_Representative.getNNodes() == 0) {
        m_Representative = m_EMap.getRepresentative();
        m_AvgWinner = m_Representative.getProperties();
        m_StdWinner = 0.f;
        m_MinModel = m_MaxModel = m_AvgWinner;
    }
//destroy bins
    m_BinsX = m_BinsY = 0;
    m_Bins.clear();
//initialize bins
    float radius = getPropScale(m_MinModel);
    //m_Representative.getStdRadius();
    if(m_Population.empty() || m_PosRange.sum()==0 || radius<0.001)
        return false;

    m_BinDist = bindist != 0.f ? bindist : radius*getSearchPara().m_RelBinDist;
    m_BinsX = (int)(m_PosRange.x/m_BinDist)+1;
    m_BinsY = (int)(m_PosRange.y/m_BinDist)+1;
    m_Bins.resize(m_BinsX*m_BinsY);
//          cout << m_PosRange << endl;
//          TRACE("created " << m_BinsX << " in X and " << m_BinsY
//                << " in Y direction.");
//fill bins
    for(list<Model*>::iterator model = m_Population.begin();
        model != m_Population.end(); model++)
    {
        (*model)->setOldState();
        (*model)->unsetFlags(Model::ST_MEMBER);
        (*model)->setShapeWeight(m_ShapeWeight);
        Point c = (*model)->getCenter();
        int bx = (int)(c.x/m_BinDist);
        int by = (int)(c.y/m_BinDist);
#define MAXRAD 0
#if (MAXRAD>0)
        for(int px=bx-MAXRAD; px<=bx+MAXRAD; px++) {
            if(px>=0 && px<=m_BinsX) {
                for(int py=by-MAXRAD; py<=by+MAXRAD; py++) {
                    bool culled;
                    int index = getBindex(px,py,culled);
                    if(!culled)
                        m_Bins[index].push_back(*model);
                }
            }
        }
#else
        bool culled;
        int index = getBindex(bx,by,culled);
        //if(!culled)
        m_Bins[index].push_back(*model);
#endif
#undef MAXRAD
    }
    return true;
}

int Searcher::buildClusters()
{
    m_Clusters.clear();
    m_NClusters = 0;
    if(!buildBins()) return 0;
    list<Model*>::iterator model = m_Population.begin();
    float cqth = (*model)->getQualityOfFit()*getSearchPara().m_ClusterTH;
    float m_MergeDistMM = getSearchPara().m_ClusterMeltDist;
    float m_MergeDist;
    if(m_Representative.getDataScale()>0)
        m_MergeDist = m_Representative.getDataScale()*m_MergeDistMM;
    else m_MergeDist = m_MergeDistMM*100;
    int nmelt = 0, nmerge = 0;
    int maxclusters = (int)getSearchPara().m_NClusters;
    if(maxclusters == 0) maxclusters = numeric_limits<int>::max();
    while(model != m_Population.end()
          && (*model)->getQualityOfFit()>cqth
          && m_NClusters < maxclusters)
    {
        if(!(*model)->hasFlags(Model::ST_MEMBER)) {
//create new cluster from model
            m_Clusters.push_back(MBin());
            m_CurCluster = &m_Clusters.back();
            m_CurCluster->push_back(*model);
            (*model)->setFlags(Model::ST_MEMBER);
            PropVec cprop = (*model)->getPropertiesMM();
            float mdrot = M_PI*getSearchPara().m_ClusterDDir;
            float mdscale = getSearchPara().m_ClusterDScale;
            float mdpos = getPropScale(cprop)*getSearchPara().m_ClusterDPos;
            m_NClusters++;
//scan bins for other cluster members
            Point c = (*model)->getCenter();
            int bx = (int)(c.x/m_BinDist);
            int by = (int)(c.y/m_BinDist);
            int binradius = (int)ceil(mdpos/m_BinDist);
            for(int px=bx-binradius; px<=bx+binradius; px++) {
                if(px>=0 && px<=m_BinsX) {
                    for(int py=by-binradius; py<=by+binradius; py++) {
                        bool culled;
                        int index = getBindex(px,py,culled);
                        if(!culled) {
                            MBin::iterator be = m_Bins[index].begin();
                            while(be != m_Bins[index].end()) {
                                MBin::iterator nextbe = be;
                                nextbe++;
                                if(!(*be)->hasFlags(Model::ST_MEMBER)) {
                                    PropVec prop = (*be)->getPropertiesMM();
                                    //PropVec pdiff = getPropTF(cprop, prop);
                                    float dpos = (getPropPos(cprop)-
                                                  getPropPos(prop)).norm();
                                    float drot = mapAnglePI(getPropDir(cprop)-
                                                            getPropDir(prop));
                                    drot = fabs(drot);
                                    float dscale = getPropScale(prop)/
                                        getPropScale(cprop);
                                    if(dscale>1) dscale = 1-1/dscale;
                                    else dscale = 1-dscale;
                                    if(dpos < mdpos && drot < mdrot &&
                                       dscale < mdscale) {
                                        if(dpos < m_MergeDistMM &&
                                           (*model)->distance(
                                               **be,Model::DIST_CPOINTS)
                                           < m_MergeDist)
                                        {
                                            //too close --> melt
                                            (*model)->mergeModel(**be);
                                            nmelt++;
                                        } else {
                                            m_CurCluster->push_back(*be);
                                            nmerge++;
                                        }
                                        (*be)->setFlags(Model::ST_MEMBER);
                                        m_Bins[index].erase(be);
                                    }
                                } else m_Bins[index].erase(be);
                                be = nextbe;
                            }
                        }
                    }
                }
            }
//             DUMP(cprop);
//             DUMP((*model)->getQualityOfFit());
//             DUMP(m_CurCluster->size());
        }
        model++;
    }
//    DUMP(nmelt);
//    DUMP(m_MergeDist);
//    DUMP(nmerge);
//    DUMP(m_NClusters);
    return m_NClusters;
}

void Searcher::selectWinners()
{
    dword m_NBinWinners = 1;
    for(list< MBin >::iterator cluster = m_Clusters.begin();
        cluster != m_Clusters.end(); cluster++) {
        if(!cluster->empty()) {
            MBin::iterator crep = cluster->begin();
//select winners
            if(!(*crep)->hasFlags(Model::ST_DEL)) {
                (*crep)->setOldState(false);
                if(!(*crep)->isWinner()) {
//                     float time = m_Time;
//                     if((*crep)->m_TimeStamp[Model::TS_WIN]!=0)
//                         time = (*crep)->m_TimeStamp[Model::TS_WIN];
                    (*crep)->setWinner(true); //, time);
                }
                (*crep)->setLooser(false);
                crep++;
            } else  TRACE("DELETED CLUSTER WINNER????????");
//select rest as loosers
            while(crep != cluster->end()) {
                //if((*crep)->isOldState()) {
                (*crep)->setOldState(false);
                if((*crep)->isWinner()) (*crep)->setWinner(false);
                if(!(*crep)->isLooser()) {
                    (*crep)->setLooser(true, m_Time);
                }
                crep++;
            }
        }
    } // for each cluster
}

void Searcher::cleanFlags()
{
//remove loosers -- no, just set flags properly
    int killcount = 0;
    int wincount = 0;
    int mergekill=0;
    int lowkill=0;
    Model *prep = *m_Population.begin();
    m_AvgWinner = 0.f;
    m_StdWinner = 0.f;
    m_MinModel = m_MaxModel = prep->getProperties();
    list<Model*>::iterator model = m_Population.begin();
    while(model != m_Population.end())
    {
        list<Model*>::iterator nm = model; nm++;
        if((*model)->hasFlags(Model::ST_NODEL)) {
            (*model)->setFlags(Model::ST_WINNER|Model::ST_MEMBER);
            (*model)->unsetFlags(Model::ST_LOOSER|Model::ST_OLDSTATE
                                 |Model::ST_DEL);
        }
        if(!(*model)->hasFlags(Model::ST_MEMBER)) {
            remove(model);
            lowkill++;
        } else if((*model)->hasFlags(Model::ST_DEL)) {
            //(*model)->unsetFlags(Model::ST_DEL);
            remove(model);
            mergekill++;
        } else {
            m_MinModel.clampUB((*model)->getProperties());
            m_MaxModel.clampLB((*model)->getProperties());
            if((*model)->isOldState()) {
                (*model)->unsetFlags(Model::ST_WINNER|Model::ST_LOOSER
                                     |Model::ST_OLDSTATE);
                //(*model)->m_TimeStamp[Model::TS_WIN] = 0.0;
            } else if((*model)->isWinner()) {
                if(prep->getQualityOfFit() < (*model)->getQualityOfFit())
                    prep = *model;
                PropVec winprop = (*model)->getProperties();
                m_AvgWinner += winprop;
                m_StdWinner += (winprop *= winprop);
                wincount++;
            } else { //if((*model)->isLooser()) {
                if(!(*model)->isLooser())
                    (*model)->setLooser(true, m_Time);
                //(*model)->m_TimeStamp[Model::TS_WIN] = 0.0;
                killcount++;
            }
        }
        model = nm;
    }
    m_Representative = *prep;
    if(!wincount) {
        m_AvgWinner = m_Representative.getProperties();
        m_StdWinner = 0.f;
    } else {
        m_AvgWinner /= (float)(wincount);
        m_StdWinner /= (float)(wincount);
        m_StdWinner -= (m_AvgWinner*m_AvgWinner);
        m_StdWinner.sqrtEach();
    }
    DUMP(mergekill);
    DUMP(lowkill);
    cout << "representative age: "
         << m_Time-m_Representative.m_TimeStamp << " s" //[Model::TS_WIN]
         << " (search time "<<m_Time<<" s)"<<endl;
    cout << killcount<<" loosers and " << wincount << " winners."<<endl;
}

void Searcher::reattachSensors()
{
    m_EMap.getRepresentative().reattachSensors();
    m_Representative.reattachSensors();
    for(list<Model*>::iterator model = m_Population.begin();
        model != m_Population.end(); model++)
    {
        (*model)->reattachSensors();
    }
}

int Searcher::distribute(int n, float qth, bool count)
/* int n=-1, float qth=0, bool count=false) */
{
    //this function is unused
    if(m_EMap.getRepresentative().getNNodes() == 0) return -1;
    else if(m_Representative.getNNodes() == 0) {
        m_Representative = m_EMap.getRepresentative();
        m_AvgWinner = m_Representative.getProperties();
        m_StdWinner = 0.f;
    }
    for(int i=0; i<n; i++) {
        Model *newmod = m_EMap.generateInstance();
        if(newmod->getQualityOfFit() < qth) {
            delete newmod;
        } else {
            //newmod->m_TimeStamp = m_Time; //[Model::TS_CREATE]
            add(newmod);
        }
    }
    return n;
}

void Searcher::getQualityRange(float &lqof, float &hqof,
                               float shapeweight) const
{
    if(m_Population.empty()) {
        lqof = hqof = 0;
        return;
    }
    bool cw = (shapeweight!=-1);
    if(cw) (*m_Population.begin())->setShapeWeight(shapeweight);
    lqof=hqof=(*m_Population.begin())->getQualityOfFit();
    for(list<Model*>::const_iterator m = m_Population.begin();
        m!=m_Population.end(); m++)
    {
        if(cw) (*m)->setShapeWeight(shapeweight);
        if((*m)->getQualityOfFit()>hqof) hqof=(*m)->getQualityOfFit();
        if((*m)->getQualityOfFit()<lqof) lqof=(*m)->getQualityOfFit();
    }
}

const Winner* Searcher::getWinner(dword id) const
{
    map<dword,Winner>::const_iterator wi = m_Winners.find(id);
    if(wi == m_Winners.end()) return NULL;
    else return &wi->second;
}

std::map<dword,Winner>& Searcher::updateWinList()
{
    m_Winners.clear();
    for(list<Model*>::iterator m = m_Population.begin();
        m!=m_Population.end(); m++)
    {
        if((*m)->isWinner())
            m_Winners[(*m)->getID()] = Winner(*m);
    }
    return m_Winners;
}

void Searcher::draw() const
{
    glLineWidth(1.0f);
    for(list<Model*>::const_iterator m = m_Population.begin();
        m!=m_Population.end(); m++)
    {
        if((*m)->isWinner()) {
            glColor4f(0.1,0.9,0.0,0.7);
        } else if((*m)->isLooser()) {
            glColor4f(0.0,0.0,1.0,0.3);
        } else {
            glColor4f(0.9,0.9,0.0,0.3);
        }
        (*m)->draw();
    }
    glLineWidth(3.0f);
    glColor4f(0.1,0.9,0.0,1.0);
    for(list<Model*>::const_iterator m = m_Population.begin();
        m!=m_Population.end(); m++)
    {
        if((*m)->isWinner())// && (m_Time-(*m)->m_TimeStamp[Model::TS_WIN])>6)
            (*m)->draw();
    }
    if(!m_Winners.empty()) {
        glLineWidth(3.0f);
        glColor4f(1.0f, 0.0f, 0.5f,1.0f);
        m_Representative.draw();
        glLineWidth(1.0f);
    }
}

void Searcher::startSearch(bool dostart)
{
    if(dostart) {
        cout << "starting searcher..." << endl;
        clear();
        m_Mode = MODE_RUN;
        updateWinList();
    } else m_Mode = MODE_PAUSE;
}

bool Searcher::triggerTest(int mx, int my, int what)
{
    switch(what)
    {
    case KEY_r:
        clear();
        break;
    case KEY_s:
        if(m_Mode == MODE_PAUSE || m_Mode == MODE_DONE) {
            m_Mode = MODE_RUN;
            cout << "run" << endl;
        } else {
            m_Mode = MODE_PAUSE;
            cout << "pause" << endl;
        }
        break;
    case KEY_h:
        cout << m_Representative.getName() <<
            (m_Mode & (MODE_DONE|MODE_PAUSE)
             ? "is done " : "is busy ") << m_EMap.m_Integral << endl;
        m_Mode = MODE_PAUSE;
        cout << "now set to rest" << endl;
        break;
    case KEY_w:
    {
        list<Model*>::iterator model = m_Population.begin();
        while(model != m_Population.end())
        {
            list<Model*>::iterator nm = model; nm++;
            if(!(*model)->isWinner()) {
                remove(model);
            } else {
                cout << "winner at " << (*model)->getCenter() << " age "
                     << m_Time-(*model)->m_TimeStamp<<" s "
                     << "quality= " << (*model)->getQualityOfFit()
                     << " nmerges = " << (*model)->getInstCount()
                     << " ID = " << (*model)->getID()
                     << endl;
            }
            model = nm;
        }
    }
    break;
    case KEY_F4:
    {
        float lqof,hqof;
        getQualityRange(lqof,hqof);
        float qth = (lqof+hqof)*0.5;
        list<Model*>::iterator model = m_Population.begin();
        while(model != m_Population.end())
        {
            list<Model*>::iterator nm = model; nm++;
            if((*model)->getQualityOfFit() < qth) {
                remove(model);
            }
            model = nm;
        }
    }
    break;
    default:
        return false;
    }
    updateWinList();
    return true;
}

//----------------------------------------------------------------------------
SearcherParams SearcherParams::global;

SearcherParams::SearcherParams()
{
    m_EvolveCycle = SRCH_CYCLE_T;

    m_MaxPop = SRCH_MAXPOP;
    m_NSpawns = SRCH_NEW_SPAWNS;
    m_NSpawnsTHRed = SRCH_NEW_SPAWNS_THRED;

    m_NClusters = SRCH_NCLUSTERS;
    m_ClusterTH = SRCH_CLUSTERTH;
    m_ClusterDPos = SRCH_CLUSTERDPOS;
    m_ClusterDScale = SRCH_CLUSTERDSCALE;
    m_ClusterDDir = SRCH_CLUSTERDDIR;
    m_ClusterMeltDist = SRCH_CLUSTERDMELT;
    m_RelBinDist = SRCH_RELBINDIST;

    m_MutateRate = SRCH_MUTATE_RATE;
    m_MutateHL = SRCH_MUTATE_HL;
    m_MutateSpawns = SRCH_MUTATE_SPAWNS;
    m_MutateTHRed = SRCH_MUTATE_THRED;
    m_MutatePDir = SRCH_MUTATE_PDIR;
    m_MutatePPos = SRCH_MUTATE_PPOS;
    m_MutatePNoise = SRCH_MUTATE_PNOISE;
    m_MutateNoiseRate = SRCH_MUTATE_NOISERATE;
    m_MutatePProp = SRCH_MUTATE_PPROP;
    m_MutatePropRate = SRCH_MUTATE_PROPRATE;
    m_RatingTH = SRCH_RATINGTH;
    m_ScaleStd = SRCH_SCALESTD;
    m_PCTH = PARA_PCTH;
    m_PDist = PARA_PDIST;
    m_MaxShoot = PARA_MAXSHOOT;

    m_ShapeWeight = SRCH_SHAPEWEIGHT;
}

ostream& SearcherParams::write(ostream& os, bool showcomment) const
{
    os << "searcher" << endl;
    if(m_EvolveCycle != SRCH_CYCLE_T)
        os<<"  evolvecycle     "<<m_EvolveCycle<<endl;
    else if(showcomment) os<<"  # evolvecycle     "<<m_EvolveCycle<<endl;
    if(m_MaxPop != SRCH_MAXPOP)
        os<<"  maxpop          "<< m_MaxPop<<endl;
    else if(showcomment) os<<"  # maxpop          "<<m_MaxPop<<endl;
    if(m_NSpawns != SRCH_NEW_SPAWNS)
        os<<"  nspawns         "<< m_NSpawns<<endl;
    else if(showcomment) os<<"  # nspawns         "<<m_NSpawns<<endl;
    if(m_NSpawnsTHRed != SRCH_NEW_SPAWNS_THRED)
        os<<"  nspawnsthred    "<< m_NSpawnsTHRed<<endl;
    else if(showcomment) os<<"  # nspawnsthred    "<<m_NSpawnsTHRed<<endl;
    if(m_NClusters != SRCH_NCLUSTERS)
        os<<"  nclusters       "<< m_NClusters<<endl;
    else if(showcomment) os<<"  # nclusters       "<<m_NClusters<<endl;
    if(m_ClusterTH != SRCH_CLUSTERTH)
        os<<"  clusterth       "<< m_ClusterTH<<endl;
    else if(showcomment) os<<"  # clusterth       "<<m_ClusterTH<<endl;
    if(m_ClusterDPos != SRCH_CLUSTERDPOS)
        os<<"  clusterdpos     "<< m_ClusterDPos<<endl;
    else if(showcomment) os<<"  # clusterdpos     "<<m_ClusterDPos<<endl;
    if(m_ClusterDScale != SRCH_CLUSTERDSCALE)
        os<<"  clusterdscale   "<< m_ClusterDScale<<endl;
    else if(showcomment) os<<"  # clusterdscale   "<<m_ClusterDScale<<endl;
    if(m_ClusterDDir != SRCH_CLUSTERDDIR)
        os<<"  clusterddir     "<< m_ClusterDDir<<endl;
    else if(showcomment) os<<"  # clusterddir     "<<m_ClusterDDir<<endl;
    if(m_ClusterMeltDist != SRCH_CLUSTERDMELT)
        os<<"  clustermeltdist "<< m_ClusterMeltDist<<endl;
    else if(showcomment) os<<"  # clustermeltdist "<<m_ClusterMeltDist<<endl;
    if(m_RelBinDist != SRCH_RELBINDIST)
        os<<"  relbindist      "<< m_RelBinDist<<endl;
    else if(showcomment) os<<"  # relbindist      "<<m_RelBinDist<<endl;
    if(m_MutateRate != SRCH_MUTATE_RATE)
        os<<"  mutaterate      "<< m_MutateRate<<endl;
    else if(showcomment) os<<"  # mutaterate      "<<m_MutateRate<<endl;
    if(m_MutateHL != SRCH_MUTATE_HL)
        os<<"  mutatehl        "<< m_MutateHL<<endl;
    else if(showcomment) os<<"  # mutatehl        "<<m_MutateHL<<endl;
    if(m_MutateSpawns != SRCH_MUTATE_SPAWNS)
        os<<"  mutatespawns    "<< m_MutateSpawns<<endl;
    else if(showcomment) os<<"  # mutatespawns    "<<m_MutateSpawns<<endl;
    if(m_MutateTHRed != SRCH_MUTATE_THRED)
        os<<"  mutatethred     "<< m_MutateTHRed<<endl;
    else if(showcomment) os<<"  # mutatethred     "<<m_MutateTHRed<<endl;
    if(m_MutatePDir != SRCH_MUTATE_PDIR)
        os<<"  mutatepdir      "<< m_MutatePDir<<endl;
    else if(showcomment) os<<"  # mutatepdir      "<<m_MutatePDir<<endl;
    if(m_MutatePPos != SRCH_MUTATE_PPOS)
        os<<"  mutateppos      "<< m_MutatePPos<<endl;
    else if(showcomment) os<<"  # mutateppos      "<<m_MutatePPos<<endl;
    if(m_MutatePNoise != SRCH_MUTATE_PNOISE)
        os<<"  mutatepnoise    "<< m_MutatePNoise<<endl;
    else if(showcomment) os<<"  # mutatepnoise    "<<m_MutatePNoise<<endl;
    if(m_MutateNoiseRate != SRCH_MUTATE_NOISERATE)
        os<<"  mutatenoiserate "<< m_MutateNoiseRate<<endl;
    else if(showcomment) os<<"  # mutatenoiserate "<<m_MutateNoiseRate<<endl;
    if(m_MutatePProp != SRCH_MUTATE_PPROP)
        os<<"  mutatepprop     "<< m_MutatePProp<<endl;
    else if(showcomment) os<<"  # mutatepprop     "<<m_MutatePProp<<endl;
    if(m_MutatePropRate != SRCH_MUTATE_PROPRATE)
        os<<"  mutateproprate  "<< m_MutatePropRate<<endl;
    else if(showcomment) os<<"  # mutateproprate  "<<m_MutatePropRate<<endl;
    if(m_ShapeWeight != SRCH_SHAPEWEIGHT)
        os<<"  shapeweight     "<< m_ShapeWeight<<endl;
    else if(showcomment) os<<"  # shapeweight     "<<m_ShapeWeight<<endl;
    if(m_ParaSolver != PARA_SOLVER)
        os<<"  solver          "<< m_ParaSolver<<endl;
    else if(showcomment) os<<"  # solver     "<<m_ParaSolver<<endl;
    if(m_RatingTH != SRCH_RATINGTH)
        os<<"  ratingth        "<< m_RatingTH<<endl;
    else if(showcomment) os<<"  # ratingth     "<<m_RatingTH<<endl;
    if(m_PCTH != PARA_PCTH)
        os<<"  pcth            "<< m_PCTH<<endl;
    else if(showcomment) os<<"  # pcth         "<<m_PCTH<<endl;
    if(m_PDist != PARA_PDIST)
        os<<"  pdist           "<< m_PDist<<endl;
    else if(showcomment) os<<"  # pdist        "<<m_PDist<<endl;
    if(m_ScaleStd != SRCH_SCALESTD)
        os<<"  scalestd        "<< m_ScaleStd<<endl;
    else if(showcomment) os<<"  # scalestd        "<<m_ScaleStd<<endl;
    if(m_MaxShoot != PARA_MAXSHOOT)
        os<<"  maxshoot        "<< m_MaxShoot <<endl;
    else if(showcomment) os<<"  # maxshoot        "<<m_MaxShoot<<endl;
    //if( != ) os << " = " <<  << endl;
    os << "end" << endl;
    return os;
}

bool SearcherParams::read(ParseFile& is)
{
    bool issearcher = false, goon=true;
    while(goon && is.getNextLine()) {
        if(is.getKey() == "searcher") issearcher = true;
        else if(!issearcher) {
            is.pushLine();
            return false;
        } else if(is.getKey() == "evolvecycle") {
            fromString(is.getValue(), m_EvolveCycle);
        } else if(is.getKey() == "maxpop") {
            fromString(is.getValue(), m_MaxPop);
        } else if(is.getKey() == "nspawns") {
            fromString(is.getValue(), m_NSpawns);
        } else if(is.getKey() == "nspawnsthred") {
            fromString(is.getValue(), m_NSpawnsTHRed);
        } else if(is.getKey() == "nclusters") {
            fromString(is.getValue(), m_NClusters);
        } else if(is.getKey() == "clusterth") {
            fromString(is.getValue(), m_ClusterTH);
        } else if(is.getKey() == "clusterdpos") {
            fromString(is.getValue(), m_ClusterDPos);
        } else if(is.getKey() == "clusterdscale") {
            fromString(is.getValue(), m_ClusterDScale);
        } else if(is.getKey() == "clusterddir") {
            fromString(is.getValue(), m_ClusterDDir);
        } else if(is.getKey() == "clustermeltdist") {
            fromString(is.getValue(), m_ClusterMeltDist);
        } else if(is.getKey() == "relbindist") {
            fromString(is.getValue(), m_RelBinDist);
        } else if(is.getKey() == "mutaterate") {
            fromString(is.getValue(), m_MutateRate);
        } else if(is.getKey() == "mutatehl") {
            fromString(is.getValue(), m_MutateHL);
        } else if(is.getKey() == "mutatespawns") {
            fromString(is.getValue(), m_MutateSpawns);
        } else if(is.getKey() == "mutatethred") {
            fromString(is.getValue(), m_MutateTHRed);
        } else if(is.getKey() == "mutatepdir") {
            fromString(is.getValue(), m_MutatePDir);
        } else if(is.getKey() == "mutateppos") {
            fromString(is.getValue(), m_MutatePPos);
        } else if(is.getKey() == "mutatepnoise") {
            fromString(is.getValue(), m_MutatePNoise);
        } else if(is.getKey() == "mutatenoiserate") {
            fromString(is.getValue(), m_MutateNoiseRate);
        } else if(is.getKey() == "mutatepprop") {
            fromString(is.getValue(), m_MutatePProp);
        } else if(is.getKey() == "mutateproprate") {
            fromString(is.getValue(), m_MutatePropRate);
        } else if(is.getKey() == "shapeweight") {
            fromString(is.getValue(), m_ShapeWeight);
        } else if(is.getKey() == "solver") {
            fromString(is.getValue(), m_ParaSolver);
        } else if(is.getKey() == "ratingth") {
            fromString(is.getValue(), m_RatingTH);
        } else if(is.getKey() == "pcth") {
            fromString(is.getValue(), m_PCTH);
        } else if(is.getKey() == "pdist") {
            fromString(is.getValue(), m_PDist);
        } else if(is.getKey() == "scalestd") {
            fromString(is.getValue(), m_ScaleStd);
        } else if(is.getKey() == "maxshoot") {
            fromString(is.getValue(), m_MaxShoot);
        } else if(is.getKey() == "end") goon = false;
    }
    return true;
}
