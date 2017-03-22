#include "ExpMap.h"
#include "DMatrixLinAlg.h"

ExpectationMap& ExpectationMap::operator=(const ExpectationMap &rhs) {
    if(&rhs == this) return *this;
    EMDRect::operator=(rhs);
    m_Representative = rhs.m_Representative;
    clear();
    add(rhs.m_Distributions);
    return *this;
}

EMDistribution* ExpectationMap::copy()  const
{
    return (EMDistribution*) new ExpectationMap(*this);
}

void ExpectationMap::setRepresentative(const Model &model)
{
    m_Representative = model;
}

void ExpectationMap::add(EMDistribution *ed)
{
    if(!ed) return;
    dword wid = ed->m_Creator.m_WinnerID;
    if(wid == Winner::WID_EMPTY) {
        if(!m_Distributions.empty()) wid = m_Distributions.rbegin()->first+1;
        if(wid < Winner::WID_FIRSTFREE) wid = Winner::WID_FIRSTFREE;
    }
    add(ed,wid);
}

void ExpectationMap::add(EMDistribution *ed, dword wid)
{
    if(!ed) return;
    erase(wid);
    m_Distributions[wid] = ed;
    updateIntegral();
}

void ExpectationMap::add(const EDistributions &distlist)
{
    for(EDistributions::const_iterator dl=distlist.begin();
        dl != distlist.end(); dl++)
    {
        add(dl->second->copy());
    }
}

bool ExpectationMap::erase(dword wid)
{
    EDistributions::iterator ed = m_Distributions.find(wid);
    if(ed == m_Distributions.end()) return false;
    if(ed->second) delete ed->second;
    m_Distributions.erase(ed);
    return true;
}

bool ExpectationMap::hasDist(dword wid) const
{
    return m_Distributions.find(wid) != m_Distributions.end();
}

EMDistribution* ExpectationMap::getEDist(dword wid)
{
    EDistributions::iterator ed = m_Distributions.find(wid);
    if(ed == m_Distributions.end()) return NULL;
    else return ed->second;
}

const EMDistribution* ExpectationMap::getEDist(dword wid) const
{
    EDistributions::const_iterator ed = m_Distributions.find(wid);
    if(ed == m_Distributions.end()) return NULL;
    else return ed->second;
}

dword ExpectationMap::getShootCount(dword wid) const
{
    const EMDistribution* ed = getEDist(wid);
    if(!ed) return 0;
    else return ed->getShootCount();
}

PropVec ExpectationMap::getPropVec() const
{
    if(m_SortDist.empty() || m_Integral==0.0) {
        if(!m_Distributions.empty()) {
            ((ExpectationMap*)this)->updateIntegral();
            if(m_SortDist.empty() || m_Integral==0.0)
                return getIdentityPropTF();
        } else return getIdentityPropTF();
    }
    float rint = frand(m_Integral);
    std::map<float, EMDistribution*>::const_iterator
        ed = m_SortDist.lower_bound(rint);
    if(ed == m_SortDist.end()) ed = m_SortDist.begin();
    ed->second->m_ShootCount++;
    PropVec prop = ed->second->getPropVec();
    setPropDir(prop, mapAngle2PI(getPropDir(prop),
                                 getPropDir(m_LB)));
    return prop.clamp(m_LB,m_UB);
    //return m_Distributions.begin()->getPropVec(); // is this fair?
}

float ExpectationMap::ratePropVec(const PropVec& prop, Winner* winner) const
{
    if(m_Distributions.empty()) return 0;
    PropVec pv(prop);
    setPropDir(pv, mapAngle2PI(getPropDir(prop), getPropDir(m_LB)));
    if(EMDRect::ratePropVec(pv) == 0.0) {
        return 0; //clamped?
    }

    float rate = 0.f;
    if(winner) {
        winner->clearRatings();
        for(EDistributions::const_iterator dl=m_Distributions.begin();
            dl != m_Distributions.end(); dl++)
        {
            float tr = dl->second->ratePropVec(prop);
            if(dl->second->m_Creator.m_WinnerID) {
                const std::string& creatorname = dl->second->m_Creator.m_StructName;
                dword creatorid = dl->second->m_Creator.m_WinnerID;
                winner->rateBy(creatorname, creatorid, tr);
            } else if(winner->m_BestRating < tr) winner->m_BestRating = tr;
        }
        rate = winner->m_BestRating;
    } else {
        for(EDistributions::const_iterator dl=m_Distributions.begin();
            dl != m_Distributions.end(); dl++)
        {
            float tr = dl->second->ratePropVec(prop);
            if(rate<tr) rate = tr;
        }
    }
    return rate;
}

void ExpectationMap::clear(bool oldonly)
{
    EDistributions::iterator dl=m_Distributions.begin();
    while(dl != m_Distributions.end())
    {
        EDistributions::iterator cd = dl++;
        if(cd->second)
        {
            if(!oldonly || cd->second->hasFlags(EMDistribution::ED_OLD)) {
                delete cd->second;
                if(oldonly) m_Distributions.erase(cd);
            }
        } else if(oldonly) m_Distributions.erase(cd);
    }
    if(!oldonly) m_Distributions.clear();
    if(oldonly) updateIntegral();
}

void ExpectationMap::markAllOld() {
    for(EDistributions::iterator dl=m_Distributions.begin();
        dl != m_Distributions.end(); dl++)
    {
        if(dl->second) dl->second->setFlags(EMDistribution::ED_OLD);
    }
}

float ExpectationMap::updateIntegral()
{
    m_Integral = 0.f;
    m_SortDist.clear();
    for(EDistributions::const_iterator dl=m_Distributions.begin();
        dl != m_Distributions.end(); dl++)
    {
        if(dl->second) {
            if(dl->second->m_Integral > 0.00001) {
                m_Integral += dl->second->m_Integral;
                m_SortDist[m_Integral] = dl->second;
            }
        }
    }
    return m_Integral;
}

Model* ExpectationMap::generateInstance() const
{
    Model *model = new Model(m_Representative);
    PropVec v = getPropVec();
    model->adaptProperties(v);
    return model;
}

#define MIN_SCALE       0.0001
void ExpectationMap::correctLBUB(PropVec& lb, PropVec& ub)
{
    float la = mapAngle2PI(getPropDir(lb));
    float ua = mapAngle2PI(getPropDir(ub), la);
    if(ua-la < 0.00001) ua += 2*M_PI;
    setPropDir(lb, la); setPropDir(ub, ua);
    PropVec cub(ub); ub.clampLB(lb); lb.clampUB(cub); //conditional swap
    float ls = getPropScale(lb);
    if(ls < MIN_SCALE) setPropScale(lb, MIN_SCALE);
}

//----------------------------------------------------------------------------
PropVec EMDRect::getPropVec() const
{
    PropVec pv;
    for(unsigned int i=0; i<PropVec::size(); i++)
        pv[i] = frand(m_UB[i]-m_LB[i])+m_LB[i];
    return pv;
}

float EMDRect::ratePropVec(const PropVec& prop, Winner* winner) const
{
    PropVec clampvec(prop);
    clampvec.clamp(m_LB, m_UB);
    if((clampvec-prop).norm2() < 0.0001) return 1;
    else return 0;
}

//----------------------------------------------------------------------------
PropVec EMDGauss::getPropVec() const
{
    PropVec pv;
    for(unsigned int i=0; i<PropVec::size(); i++)
        pv[i] = fgauss01()*m_Stdev[i] + m_Avg[i];
    return pv;
}

float EMDGauss::ratePropVec(const PropVec& prop, Winner* winner) const
{
    return gauss2(((prop-m_Avg)/=m_StdevRate).norm2(),1)*M_SQRT2PI;
}

void EMDGauss::setStdev(const PropVec& stdev)
{
    m_Stdev = stdev;
    for(dword i = 0; i<stdev.size(); i++) {
        if(stdev[i]>0.0001) m_StdevRate[i] = stdev[i];
        else m_StdevRate[i] = std::numeric_limits<float>::max();
    }
}

//----------------------------------------------------------------------------
PropVec EMDXformer::getPropVec() const
{
    PropVec propPV = m_InputEMD->getPropVec();
    hgPropTF(m_TFMat, propPV);
    propPV *= m_Stdev;
    propPV += m_Mean;
    PropVec pv(m_Origin);
    fwdPropTF(pv, propPV);
    return pv;
}

float EMDXformer::ratePropVec(const PropVec& prop, Winner* winner) const
{
//inverse transform
    PropVec tfprop(getPropTF(m_Origin, prop));
    tfprop -= m_Mean;
    setPropDir(tfprop, mapAnglePI(getPropDir(tfprop)));
    tfprop /= m_Stdev;
    hgPropTF(m_ITFMat, tfprop);
    return m_InputEMD->ratePropVec(tfprop);
}

void EMDXformer::setTFMat(const DMatrixf& mat)
{
    if(mat.sizeX() == mat.sizeY() &&
       (mat.sizeX() == PropVec::size() ||
        mat.sizeX() == PropVec::size()+1)) {
        m_TFMat = mat;
        m_ITFMat = dmutil::inverse(m_TFMat);
    } else {
        m_TFMat.clear();
        m_ITFMat.clear();
    }
}

//----------------------------------------------------------------------------
void Winner::setModel(Model* m)
{
    m_Model = m;
    m_WinnerID = m ? m->getID() : Winner::WID_EMPTY;
    m_StructName = m ? m->getName() : "";
}

void Winner::clearRatings()
{
    m_BestRating = 0.0f;
    m_BestRatings.clear();
    m_Ratings.clear();
}

float Winner::rateBy(const std::string& creatorname, dword creatorid, float rating)
{
    m_Ratings[creatorname][creatorid] = rating;
    dword& brid = m_BestRatings[creatorname];
    if(!brid || m_Ratings[creatorname][brid] < rating) {
        brid = creatorid;
        if(m_BestRating < rating) m_BestRating = rating;
    }
    return m_BestRating;
}

const std::pair<dword,float> Winner::getBest(const std::string& sname) const
{
    const std::map<dword,float>& rl = getRatingList(sname);
    dword bid = getBestRatingID(sname);
    return *rl.find(bid);
}

float Winner::getConnection(const std::string& sname, dword wid) const
{
    if(!hasConnection(sname)) return -1.f;
    const std::map<dword,float>& rl = getRatingList(sname);
    std::map<dword,float>::const_iterator rt = rl.find(wid);
    if(rt != rl.end()) return rt->second;
    else return -1.f;
}

float Winner::updateBestRating()
{
    m_BestRating = 0.f;
    m_BestRatings.clear();
    for(Ratings::const_iterator refstruct = m_Ratings.begin();
        refstruct != m_Ratings.end(); refstruct++)
    {
        dword& brid = m_BestRatings[refstruct->first];
        float bestrating = 0.f; //m_Ratings[refstruct->first][brid];
        for(std::map<dword,float>::const_iterator wr= refstruct->second.begin();
            wr != refstruct->second.end(); wr++)
        { // for each rating wr
            if(wr->second > bestrating) {
                bestrating = wr->second;
                brid = wr->first;
            }
        }
        if(m_BestRating < bestrating) m_BestRating = bestrating;
    }
    return m_BestRating;
}
