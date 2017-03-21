#include <fstream>
#include <set>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <fxkeys.h>
#include "Brain.h"
#include "StructTable.h"
#include "MStruct.h"
#include "DMatrix.h"
#include "DMatrixUtil.h"
#include "DMatrixLinAlg.h"
#include "utils.h"

using namespace std;

//----------------------------------------------------------------------------
#define SUBSTRUCT_RATEWEIGHT   1.0f
#define STRUCT_SHAPEWEIGHT     0.1f
#define SUBSTRUCT_RATETH       0.0f

//--------------------------- MStructure ------------------------------------

MStructure::MStructure(const string& name, StructTable* psTable) 
    : m_PStructTable(psTable), m_Size(1.0f), m_Weight(1.0f)
{
    setName(name);
    if(m_PStructTable) {
        m_Model.attachDataset(m_PStructTable->getDataset());
        m_Model.mergeSensorCollection(m_PStructTable->getSensors());
    }
}

MStructure::MStructure(const MStructure& rhs) 
{ 
    operator=(rhs); 
}

MStructure::~MStructure() 
{
    if(!m_RefProp.empty()) saveRefProp();
    clear();
}

MStructure& MStructure::operator=(const MStructure& rhs)
{
    if(this != &rhs) {
        m_Name = rhs.m_Name;
        m_Model = rhs.m_Model;
        m_Size = rhs.m_Size;
        m_PStructTable = rhs.m_PStructTable;
        m_SubStructures = rhs.m_SubStructures;
        m_SupStructures = rhs.m_SupStructures;
        m_RefProp = rhs.m_RefProp;
        m_Weight = rhs.m_Weight;
        m_FrameWinner = rhs.m_FrameWinner;
    }
    return *this;
}

void MStructure::clear()
{
    m_Name.clear(); 
    m_Model.clear(); 
    m_Size = 1.0f;
    m_Weight = 1.0f;
    m_SupStructures.clear();
    m_SubStructures.clear();
    m_FrameWinner.m_StructName.clear();
    m_RefProp.clear();
}

void MStructure::setName(const string& name)
{ 
    m_Name = name;
    m_FrameWinner.m_StructName = name; //just in case it is used
    m_FrameWinner.m_WinnerID = Winner::WID_FRAME;
}

const Winner* MStructure::getWinner(dword wid) const
{
    if(isFrame() && wid==Winner::WID_FRAME) 
        return &m_FrameWinner;
    else return m_Searcher.getWinner(wid);
}

void MStructure::addSubStruct(const SubStructure& substruct)
{
    m_SubStructures[substruct.m_SubStructName] = substruct;
    m_SubStructures[substruct.m_SubStructName].m_SupStructName = m_Name;
    //super structure references are updated in connectSubSuper()
}

bool MStructure::read(ParseFile &is)
{
    clear();
    bool readon = true;
    SubStructure substruct;
    while(readon && is.getNextLine()) {
        const string& key = is.getKey();
        const string& value = is.getValue();
        if(key == "struct") {
            if(!m_Name.empty()) is.setParseError(
                "struct inside struct. (rather use substruct?)");
            else {
                setName(value.c_str());
                loadRefProp();
            }
        } else if(m_Name.empty()) { 
            is.pushLine(); return false;
        } else if(key == "end") readon = false;
        else if(key == "model") {
            string filename = is.getPath()+is.getValue();
            if(!m_Model.readFile(filename.c_str())) {
                is.setParseError(string("couldn't load model ")+filename);
            }
        } else if(key == "size") {
            istringstream(value) >> m_Size;
        } else if(key == "weight") {
            fromString(value, m_Weight);
        } else if(key == "shapeweight") {
            float shapeweight=STRUCT_SHAPEWEIGHT;
            fromString(value, shapeweight);
            m_Searcher.setShapeWeight(shapeweight);
        } else if(substruct.read(is)) addSubStruct(substruct);
        else is.setParseError(string("(parsing struct) unknown token ") + key);
    }
    if(is.error()) return false;
    return true;
}

void MStructure::write(ostream &os) const
{
    if(!(*this)) return;
    os << "struct " << m_Name << endl;
    if(!m_Model.getFilename().empty()) {
        string filename = m_Model.getFilename();
        if(m_PStructTable) {
            if(filename.substr(0,m_PStructTable->getPath().size()) 
               == m_PStructTable->getPath())
                filename = filename.substr(m_PStructTable->getPath().size());
        }
        os << "  model " << filename << endl;
    }
    if(m_Weight != 1.f) os << "  weight " << m_Weight << endl;
    if(m_Searcher.getShapeWeight() != STRUCT_SHAPEWEIGHT)
        os << "  shapeweight " << m_Searcher.getShapeWeight() << endl;
    os << "  size " << m_Size << endl;
    for(map<string,SubStructure>::const_iterator si = m_SubStructures.begin();
        si != m_SubStructures.end(); si++)
    {
        os << si->second;
    }
    os << "end    # of struct" << endl << endl;
}

string MStructure::getInfoFilename(const string& suffix) const {
    string fn;
    if(m_PStructTable) {
      fn = m_PStructTable->getFilename();
      fn = fn.substr(0, fn.rfind('.'));
    } else fn = "struct";
    fn = fn + "_" + m_Name + "_" + suffix + ".dm";
    return fn;
}

bool MStructure::getRefModel(dword id, Model& model) const
{
    string filename = getInfoFilename(toString(id));
    bool readf = model.readFile(filename.c_str());
    return readf;
}

bool MStructure::getRefProp(dword id, PropVec& prop) const
{
    map<dword,PropVec>::const_iterator rp = m_RefProp.find(id);
    if(rp != m_RefProp.end()) {
        prop = rp->second;
        return true;
    } else return false;
}

void MStructure::setRefProp(dword id, const PropVec& prop)
{
    m_RefProp[id] = prop;
}

void MStructure::setRefModel(dword id, const Model& model)
{
    PropVec pv(model.getProperties());
    model.convertPropToMM(pv);
    m_RefProp[id] = pv;
    //save model file for geometry internal statistics
    string filename = getInfoFilename(toString(id));
    model.writeFile(filename.c_str());
}

dword MStructure::loadRefProp()
{
    if(!*this) return 0;
    string filename(getInfoFilename("pvec"));
    ParseFile pf(filename);
    if(pf) {
        m_RefProp.clear();
        while(pf.getNextLine()) {
            dword id=0;
            fromString(pf.getKey(),id);
            if(id) {
                PropVec prop(0.0);
                fromString(pf.getValue(), prop);
                m_RefProp[id] = prop;
            }
        }
        return m_RefProp.size();
    } return 0;
}

void MStructure::saveRefProp() const 
{
    if(!*this) return;
    string filename = getInfoFilename("pvec");
    ofstream os(filename.c_str());
    if(os) 
        for(map<dword,PropVec>::const_iterator rp=m_RefProp.begin();
            rp != m_RefProp.end(); rp++)
        {
            if(rp->first)
                os << rp->first << " " << rp->second << endl;
        }
}

void MStructure::buildAllStats()
{
    set<dword> refmod;
    getRefModelIDs( insert_iterator<set<dword> >(refmod, refmod.begin()) );
    Model model; // not attached to anything
    //ofstream infile("info.dat");
    for(map<string,SubStructure>::iterator si = m_SubStructures.begin();
        si != m_SubStructures.end(); si++)
    {
        SubStructure& subs = si->second;
        MStructure& st = m_PStructTable->getStructs()[subs.m_SubStructName];
        set<dword> strefmod; st.getRefModelIDs(
            insert_iterator<set<dword> >(strefmod, strefmod.begin()) );
        list<dword> comod;
        set_intersection( refmod.begin(), refmod.end(), 
                          strefmod.begin(), strefmod.end(), 
                          back_inserter(comod) );
        dword ncomod = comod.size();
        DMatrixf tfmat(PropVec::size(), ncomod);
        dword rowind = 0;
        for(list<dword>::const_iterator idi = comod.begin();
            idi != comod.end(); idi++, rowind++) 
        {
            Point2D pivot(0,0); bool usepivot = false;
            if(subs.m_Pivot>=0 && m_Model.getNNodes()>subs.m_Pivot) {
                if(getRefModel(*idi, model)) {  // find pivot
                    pivot = model.getNode(subs.m_Pivot);
                    pivot -= model.getCenter();
                    model.convertPointToMM(pivot);
                    usepivot = true;
                } else cout << "error: could not find reference model "
                            << getInfoFilename(toString(*idi)) << endl;
            }
            //compute transformation
            PropVec prop, ssprop; 
            if(getRefProp(*idi, prop) && st.getRefProp(*idi, ssprop)) {
                setPropPos(prop, getPropPos(prop) + pivot);
                PropTF sstf = getPropTF(prop, ssprop);
                DMatrixf tfvec(PropVec::size(), 1, sstf.begin());
                tfmat.setRange(0,rowind,tfvec);
            } else cerr << "error getting propvec for " << m_Name 
                        << " or " << st.m_Name << " for species " 
                        << *idi << endl;
        }
        subs.analyseTF(tfmat);
        if(subs.m_Mode&SubStructure::MODE_SHOWSTATS) 
            showStats(subs);
    }
    buildMasterModel();
}

bool MStructure::buildMasterModel(float scscale)
{
    if(!m_Model) return false;
    list<dword> refmod;
    dword nrefmod = getRefModelIDs( back_inserter(refmod) );
//        insert_iterator<list<dword> >(refmod, refmod.begin()) );
    Model model; // not attached to anything
    DMatrixf rlen(1, m_Model.getNEdges(), 0.f);
    DMatrixf lenrat(1, m_Model.getNEdges(), 0.f);
    dword nmod = 0;
    for(list<dword>::const_iterator idi = refmod.begin();
        idi != refmod.end(); idi++) 
    {
        if(getRefModel(*idi, model)) {  // get model instance
            if(model.getNEdges() == m_Model.getNEdges()) {
                for(int i=0;i<model.getNEdges();i++) {
                    const Edge& se = model.getEdge(i);
                    //Edge& me = m_Model.getEdge(i);
                    float rl = se.restlength/model.getDataScale();
                    float lr = se.lengthRatio();
                    if(lr>1) lr = 1/lr;
                    lr = 1-lr;
                    rlen.at(0,i) += rl;
                    lenrat.at(0,i) += lr;
                }
                nmod++;
            }
        }
    }
    if(nmod>1) {        // setup spring constants according to statistics
        float scmax = ParticleParam::global.springconst;
        float minlr = 1/scmax;
        int i;
        float refallrl=0, nallrl=0;
        for(int i=0;i<m_Model.getNEdges();i++) {
            Edge& me = m_Model.getEdge(i);
            float rl = rlen.at(0,i) /= nmod;
            lenrat.at(0,i) /= nmod;
            nallrl += rl;
            refallrl += me.restlength;
        }
        nallrl *= m_Model.getDataScale();
        m_Model.scale(nallrl/refallrl,true);
        for(int i=0;i<m_Model.getNEdges();i++) {
            Edge& me = m_Model.getEdge(i);
            float rl = rlen.at(0,i);
            float sc = lenrat.at(0,i);
            sc *= scscale;
            //DUMP(sc);
            if(sc>minlr) sc = 1/sc; 
            else sc = scmax;
            //cout << "springconst = " << sc << endl;
            me.restlength = rl*m_Model.getDataScale();
            me.springconstant = sc;
        }
#define SHAPEADAPTTIMESTEP 0.01
        float imgfscale = m_Model.phys.imgforce;
        m_Model.phys.imgforce = 0;              //disable image influence
        m_Model.updateParticles(SHAPEADAPTTIMESTEP);
        float liveliness = 0;
        dword timeout = 100;
        while(abs(liveliness-m_Model.getLiveliness())>0.01 && timeout--) {
            liveliness = m_Model.getLiveliness();
            m_Model.updateParticles(SHAPEADAPTTIMESTEP);
            m_Model.updateParticles(SHAPEADAPTTIMESTEP);
            m_Model.updateParticles(SHAPEADAPTTIMESTEP);
            m_Model.updateParticles(SHAPEADAPTTIMESTEP);
            m_Model.updateParticles(SHAPEADAPTTIMESTEP);
        }
        m_Model.phys.imgforce = imgfscale;
        if(!timeout) cout << "timeout reached while calming model" << endl;
#undef SHAPEADAPTTIMESTEP
    }
    return true;
}

void MStructure::refSubSuper(bool doclear)
{
    if(doclear) {  // just clear own list
        m_SupStructures.clear();
        return;
    }
    for(map<string,SubStructure>::iterator si = m_SubStructures.begin();
        si != m_SubStructures.end(); si++)
    {
        MStructure& st = m_PStructTable->getStructs()[si->first];
        st.m_SupStructures[m_Name] = &si->second;
    }
}

bool MStructure::addExpectation(const SubStructure& subs, 
                                ExpectationMap& expmap, bool backwards) const 
{
// if inverse look backwards to super structure
//     MStructure& st = m_PStructTable->getStructs()
//         [backwards ? subs.m_SupStructName : subs.m_SubStructName];
    if(isFrame()) {
        //cout << "is a frame" << endl;
        float ppmm = (float)m_PStructTable->getDataset()->getPPMM();
        PropVec frameprop(m_PStructTable->getDataset()->getPropVec());
        if(!(subs.m_Mode&SubStructure::MODE_NOSPAWN)) {
            EMDistribution* emdist = 
                subs.generateEMDist(frameprop,ppmm,backwards, 
                                    getSearchPara().m_PDist);
            emdist->setCreator(m_FrameWinner);
            EMDistribution* olddist=expmap.getEDist(m_FrameWinner.m_WinnerID);
            if(olddist) emdist->setShootCount(olddist->getShootCount());
            float exhaust = 
                float(emdist->getShootCount()) / getSearchPara().m_MaxShoot;
            if(exhaust>1.f) exhaust = 0.f;
            else exhaust = 1.f-exhaust;
            emdist->setIntegral(exhaust); //1.f was default
            expmap.add(emdist);
        }
        PropVec lb, ub;
        subs.getLBUB(lb, ub, frameprop, ppmm);
        ExpectationMap::correctLBUB(lb, ub);
        expmap.setLB(lb);
        expmap.setUB(ub);
        return true;
    }
//     list<const Model*> winners;
//     m_Searcher.getWinners( back_inserter(winners) );
    if(subs.m_Mode&SubStructure::MODE_NOSPAWN) return true;
    const map<dword,Winner>& winners = m_Searcher.getWinList();
    dword nwinners = winners.size();
    if(!nwinners) return false;
    DMatrixf qof(1, nwinners);
    map<dword,Winner>::const_iterator wi = winners.begin();
    for(dword i=0; i<nwinners; i++, wi++) 
        qof.at(0,i) = wi->second.m_Model->getQualityOfFit();
    float avgqof = dmutil::avg(qof).at(0,0);
    float stdqof = dmutil::stdev(qof).at(0,0);
    float maxqof = qof.max(), minqof = qof.min();
    if(maxqof<0.0000001) return false; //DEBUG
    const Winner* thebest = NULL;
    float bestrating = 0;
    dword maxshoot = 0;
    for(wi = winners.begin(); wi!= winners.end(); wi++) {
        const Winner& winner = wi->second;
        if(m_Searcher.getGeneration(winner.m_Model) > 2) {
            float wqof = winner.m_Model->getQualityOfFit();
#ifdef _SPAWN_FROM_BEST_
            float rating = wqof/maxqof + subs.m_RateWeight*winner.m_BestRating;
            if(rating > bestrating) {
                thebest = &winner;
                bestrating = rating;
            }
        }
    }
    if(thebest) {
        {
            const Winner& winner = *thebest;
            float wqof = winner.m_Model->getQualityOfFit();
#endif
            float ppmm = (float)winner.m_Model->getDataScale();
            PropVec wprop = winner.m_Model->getProperties();
            if(subs.m_Pivot>=0) setPropPos(wprop, winner.m_Model
                                           ->getNode(subs.m_Pivot));
            EMDistribution* emdist = subs.generateEMDist
                (wprop,ppmm,backwards,getSearchPara().m_PDist);
            emdist->setCreator(winner);
            EMDistribution* olddist=expmap.getEDist(winner.m_WinnerID);
            if(olddist) emdist->setShootCount(olddist->getShootCount());
            if(maxshoot < emdist->getShootCount()) 
                maxshoot = emdist->getShootCount();
            float exhaust = 
                float(emdist->getShootCount()) / getSearchPara().m_MaxShoot;
            if(exhaust>1.f) exhaust = 0.f;
            else exhaust = 1.f-exhaust;
            emdist->setIntegral((wqof/maxqof)*exhaust);
            expmap.add(emdist);
            PropVec lb, ub;
            subs.getLBUB(lb, ub, wprop, ppmm);
            ExpectationMap::correctLBUB(lb, ub);
            PropVec olb = expmap.getLB(), oub = expmap.getUB();
            setPropScale(lb, getPropScale(olb));
            setPropScale(ub, getPropScale(oub));
            ExpectationMap::correctLBUB(lb, olb);
            ExpectationMap::correctLBUB(oub, ub);
            expmap.setLB(lb);
            expmap.setUB(ub);
        }
    }
    if(thebest && false) {
        PropVec lb, ub;
        float ppmm = (float)thebest->m_Model->getDataScale();
        subs.getLBUB(lb, ub, thebest->m_Model->getProperties(), ppmm);
        ExpectationMap::correctLBUB(lb, ub);
        PropVec olb = expmap.getLB(), oub = expmap.getUB();
        setPropScale(olb, getPropScale(lb));
        setPropScale(oub, getPropScale(ub));
        expmap.setLB(olb);
        expmap.setUB(oub);
    }
    DUMP(maxshoot << " --------------------" );
    return true;
}

void MStructure::rebuildExpMap()
{
    if(!m_Model) return;
    //cout << "rebuild expmap for " << m_Name << endl;
    ExpectationMap& expmap = m_Searcher.getExpectationMap();
    expmap.markAllOld();
    expmap.setRepresentative(m_Model);
    expmap.getRepresentative().setName(m_Name);
//calculate upper and lower bounds from dataset (and avg Model)
    PropVec propl(0.), propu(0.);
    Point2D dims((float)m_PStructTable->getDataset()->getDim1Size(),
                 (float)m_PStructTable->getDataset()->getDim2Size());
    float scale = getPropScale(m_Model.getPropertiesMM());
    scale *= m_PStructTable->getDataset()->getPPMM();
    setPropPos(propu, dims);
    setPropDir(propu, 2*M_PI);
    setPropScale(propu, scale*1.1);  // is that good?
    setPropScale(propl, scale*0.9);
    ExpectationMap::correctLBUB(propl, propu);
    expmap.setLB(propl);
    expmap.setUB(propu);
#ifdef _USE_BACKGROUND_DIST_
//add a background distribution over entire allowed search range
    EMDRect* drect = new EMDRect(propl,propu);
    //drect->setIntegral(0.2); keep it 1 (like the winner)
    expmap.add(drect); //DEBUG
#endif
//add expectation from connected super structures
    for(map<string,SubStructure*>::iterator si = m_SupStructures.begin();
        si != m_SupStructures.end(); si++)
    {
        MStructure& st = m_PStructTable->getStructs()[si->first];
        // [si->second->m_SupStructName]
        st.addExpectation(*si->second, expmap);
    }
#if DO_INVERSE_TRANSFORM
//add expectation backwards from sub structures
    for(map<string,SubStructure>::iterator si = m_SubStructures.begin();
        si != m_SubStructures.end(); si++)
    {
        MStructure& st = m_PStructTable->getStructs()[si->first];
        st.addExpectation(si->second, expmap, true);
    }
#endif
    expmap.clear(true);         // remove old ones only
    m_Searcher.setExpectationMap(expmap);
    DUMP(m_Name<<" "<<expmap.m_Integral);
}

void MStructure::verifyWinnerRating()
{
    int okcount = 0;
    map<dword,Winner>& wl = m_Searcher.getWinList();
    for(map<dword,Winner>::iterator wi = wl.begin(); wi!=wl.end(); wi++)
    { // for each Winner wi
        for(Winner::Ratings::iterator refstruct = wi->second.m_Ratings.begin();
            refstruct != wi->second.m_Ratings.end(); refstruct++) 
        { // for each rating parent structure
            MStructure& st = m_PStructTable->getStructs()[refstruct->first];
            if(!st.isFrame()) {
                map<dword,float>::iterator wr= refstruct->second.begin();
                while(wr != refstruct->second.end())
                { // for each rating wr
                    map<dword,float>::iterator nextwr = wr; nextwr++;
                    const Winner* refwin=st.getSearcher().getWinner(wr->first);
                    if(refwin) {
                        okcount++;
                    } else {
                        refstruct->second.erase(wr);
                    }
                    wr = nextwr;
                }
            } else okcount += refstruct->second.size();
        }
        wi->second.updateBestRating();
    }
    cout << okcount << " good connections to struct " << m_Name << endl;
}

void MStructure::showStats(const SubStructure& subs, std::ostream& os) const
{
    os << " stats for ["<<subs.m_SupStructName<<"] --> ["
       <<subs.m_SubStructName<<"]"<<endl;
    set<dword> refmod;
    getRefModelIDs( insert_iterator<set<dword> >(refmod, refmod.begin()) );
    Model model; // not attached to anything
    MStructure& st = m_PStructTable->getStructs()[subs.m_SubStructName];
    set<dword> strefmod; st.getRefModelIDs(
        insert_iterator<set<dword> >(strefmod, strefmod.begin()) );
    list<dword> comod;
    set_intersection( refmod.begin(), refmod.end(), 
                      strefmod.begin(), strefmod.end(), 
                      back_inserter(comod) );
    dword ncomod = comod.size();
    DMatrixf tfmat(PropVec::size(), ncomod);
    DMatrixf ratings(1,ncomod);
    dword rowind = 0;
    for(list<dword>::const_iterator idi = comod.begin();
        idi != comod.end(); idi++, rowind++) 
    {
        Point2D pivot(0,0); bool usepivot = false;
        if(subs.m_Pivot>=0 && m_Model.getNNodes()>subs.m_Pivot) {
            if(getRefModel(*idi, model)) {  // find pivot
                pivot = model.getNode(subs.m_Pivot);
                pivot -= model.getCenter();
                model.convertPointToMM(pivot);
                usepivot = true;
            } else os << "error: could not find reference model "
                      << getInfoFilename(toString(*idi)) << endl;
        }
        //compute transformation
        PropVec prop, ssprop; 
        if(getRefProp(*idi, prop) && st.getRefProp(*idi, ssprop)) {
            setPropPos(prop, getPropPos(prop) + pivot);
            PropTF sstf = getPropTF(prop, ssprop);
            DMatrixf tfvec(PropVec::size(), 1, sstf.begin());
            tfmat.setRange(0,rowind,tfvec);
            //determine rating
            EMDistribution* ed = subs.generateEMDist(prop,1,false,
                                                     getSearchPara().m_PDist);
            if(ed) {
                ratings.at(0,rowind) = ed->ratePropVec(ssprop);
                os << *idi << ": " << ratings.at(0,rowind) << "  pv:"
                   << getPropTF(prop,ssprop) << endl;
                delete ed;
            } else os << "error generating exp.distr. for " << *idi << endl;
        } else os << "error getting propvec for " << m_Name 
                  << " or " << st.m_Name << " for species " 
                  << *idi << endl;
    }
    EMDistribution* ed = subs.generateEMDist(getIdentityPropTF(),1,false,
                                             getSearchPara().m_PDist);
    if(ed) {
        os << "central rating: " << ed->ratePropVec(subs.m_Mean) << endl;
        DUMP(subs.m_Mean);
        DUMP(subs.m_Stdev);
        delete ed;

	os << "pcomp  : " << PropVec(&subs.m_Sigma.at(0,0)) << endl;
	os << "average: " << dmutil::avg(ratings).at(0,0) << endl;
	os << "stdev  : " << dmutil::stdev(ratings).at(0,0) << endl;
	if(ncomod>0) {
	  os << "min    : " << ratings.min() << endl;
	  os << "max    : " << ratings.max() << endl;
	}
    } else os << "error generating exp.distr." << endl;
}

bool MStructure::stepSearch(float dt)
{
    return m_Searcher.step(dt) || isFrame();
}

//---------------------------------------------------------------------------
SubStructure::SubStructure(const std::string& subStructName, 
                           const std::string& supStructName,
                           const PropTF& transform,int pivot)
    : m_SubStructName(subStructName),m_SupStructName(subStructName),
      m_Transform(transform), m_Pivot(pivot), 
      m_Polar(false), m_RateWeight(SUBSTRUCT_RATEWEIGHT), 
      m_RateTH(SUBSTRUCT_RATETH), m_Mode(MODE_NONE)
{
}

void SubStructure::analyseTF(const DMatrixf& tfmat)
{
    using namespace dmutil;
    if(tfmat.empty() ||tfmat.sizeX()!=PropVec::size()) {
        m_PC = m_IPC = makeIdentity<float>(PropVec::size()+1);
        m_Sigma.resize(1,4); m_Sigma = 0.f;
        m_Mean = m_Stdev = 0.f;
        return;
    }
    bool show = false && (m_Mode&MODE_SHOWSTATS);
    if(show) DUMP(tfmat);
    DMatrixf ntfmat(tfmat);
    DMatrixf angles(1,ntfmat.sizeY());
    ntfmat.getRange(PVEC_DIR, 0, angles);
    StructTable::adjustByAvgDir(angles);
    ntfmat.setRange(PVEC_DIR, 0, angles);
    //todo: switch to polar position (translation)
//         ofstream f1("tfmat.dat");
//         f1 << tfmat;
//         ofstream f2("ids.dat");
//         copy(comod.begin(), comod.end(), ostream_iterator<dword>(f2, "\n"));
    //build a (0,1) standardized matrix of the distribution 
    DMatrixf avghg(avg(ntfmat));
    DMatrixf stdevhg(stdev(ntfmat));
    DMatrixf mavg(expand(avghg, 1,ntfmat.sizeY()));
    DMatrixf mstdev(expand(stdevhg, 1,ntfmat.sizeY()));
    ntfmat -= mavg; ntfmat /= mstdev;
    //build a normalized covariance matrix
    DMatrixf tfcov(ntfmat);
    tfcov.transpose();
    tfcov = tfcov.mulRight(ntfmat);
    tfcov *= (1.f/(ntfmat.sizeY())); //normalize by number of samples
    if(show) DUMP(tfcov);
    DMatrixf mU, mS, mV;
    dmutil::SVD(tfcov, mU, mS, mV); // TODO
    m_Sigma = mS.getDiag();
    float relvar = 1.f/4; //1.f/dmutil::sum(m_Sigma).at(0,0); // has to be 1/4
    for(dword i=0;i<m_Sigma.sizeY(); i++)
        if(m_Sigma.at(0,i)*relvar < getSearchPara().m_PCTH) 
            m_Sigma.at(0,i) = 0.f;
    dmutil::sqrt(m_Sigma);
    m_Mean = &avghg.at(0,0);
    m_Stdev = &stdevhg.at(0,0);
    if(show) DUMP(mU << mS << mV);
//make a homogeneous matrix to integrate de-standardization
    //m_Sigma.resize(1,m_Sigma.sizeY()+1,1.f);
    //mS = makeDiag(m_Sigma);
//     avghg.transpose();
//     avghg.resize(1,avghg.sizeY()+1,1.f);
//     stdevhg.transpose();
//     stdevhg.resize(1,stdevhg.sizeY()+1,1.f);
//     DMatrixf mVhg(makeIdentity<float>(mV.sizeX()+1));
//     mVhg.setRange(0,0,mV);
//     //mVhg.transpose(); //no transposed, or not?
//     //m_PC = makeDiag(stdevhg).mulRight(mVhg.mulRight(mS));
//     m_PC = makeDiag(stdevhg).mulRight(mVhg);
//     m_PC.setRange(m_PC.sizeX()-1,0,avghg);
    m_PC = mV;
    m_IPC = inverse(m_PC);
    //cout << m_Sigma << m_Mean << endl << m_Stdev << endl;
//     DUMP(m_SubStructName);
//     DUMP(m_Mean);
//     DUMP(m_Stdev);
}

EMDistribution* SubStructure::generateEMDist(const PropVec& wprop, float ppmm,
                                             bool backwards, 
                                             dword dtype) const
{
    //ATTENTION: backward transform isn't working the way I'm doing it here
    // so, only use forward transform
    EMDistribution* emdist = NULL;
    switch(DistrType(dtype)) {
        case EMD_PCA:
        {
        	if(m_Sigma.empty()) break;
            //generate a transformed gaussian distribution (using PCA xform)
            DMatrixf stddm(m_Sigma);
            stddm *= getSearchPara().m_ScaleStd;
            PropVec stdtf(&stddm.at(0,0));
            EMDGauss* eminput = new EMDGauss(PropVec(0.f), stdtf);
            //EMDRect* eminput = new EMDRect(stdtf*-1.f, stdtf);
            DMatrixf tfmat = backwards ? m_IPC : m_PC;
#ifndef _PROPTF_NORMALIZE_SCALE_
            DMatrixf mmmat = dmutil::makeIdentity<float>(tfmat.sizeX());
            mmmat.at(PVEC_POSX,PVEC_POSX) = ppmm;
            mmmat.at(PVEC_POSY,PVEC_POSY) = ppmm;
            tfmat = tfmat.mulLeft(mmmat);
#endif
            emdist = (EMDistribution*) new EMDXformer(eminput, wprop, tfmat);
            ((EMDXformer*)emdist)->setMean(m_Mean);
            ((EMDXformer*)emdist)->setStdev(m_Stdev);
        }
        break;
        case EMD_RECT:
        {
            //generate a rectangular uniform distribution
            PropVec propsd(m_Stdev);
            //propsd*=sqrt(3.f);
            PropVec lb(m_Mean-propsd);
            PropVec ub(m_Mean+propsd);
#ifndef _PROPTF_NORMALIZE_SCALE_
            setPropPos(lb, getPropPos(lb)*ppmm);
            setPropPos(ub, getPropPos(ub)*ppmm);
#endif
            PropVec olb(wprop), oub(wprop);
            if(backwards) { invPropTF(olb, ub); invPropTF(oub, lb); }
            else { fwdPropTF(olb, lb); fwdPropTF(oub, ub); }
            PropVec mean(m_Mean);
            setPropPos(mean, getPropPos(mean)*ppmm);
            PropVec a(wprop);
            ExpectationMap::correctLBUB(olb, oub);
            emdist = (EMDistribution*) new EMDRect(olb,oub);
        }
        break;
        case EMD_RECTTF:
        {
            //generate a rectangular uniform distribution
            PropVec propsd(m_Stdev);
            propsd*=sqrt(3.f);
            PropVec lb(m_Mean-propsd);
            PropVec ub(m_Mean+propsd);
#ifndef _PROPTF_NORMALIZE_SCALE_
            setPropPos(lb, getPropPos(lb)*ppmm);
            setPropPos(ub, getPropPos(ub)*ppmm);
#endif
            PropVec olb(getIdentityPropTF()), oub(getIdentityPropTF());
            if(backwards) { invPropTF(olb, lb); invPropTF(oub, ub); }
            else { fwdPropTF(olb, lb); fwdPropTF(oub, ub); }
            ExpectationMap::correctLBUB(olb, oub);
            EMDistribution* inpdist = (EMDistribution*) new EMDRect(olb,oub);
            emdist = (EMDistribution*) new EMDXformer(inpdist, wprop);
        }
        break;
        case EMD_GAUSS:
        {
            //generate a multidimensional axis-aligned gaussian distribution
            PropVec sdevpv(m_Stdev);
            sdevpv *= getSearchPara().m_ScaleStd;
            PropVec meanpv(m_Mean);
#ifndef _PROPTF_NORMALIZE_SCALE_
            setPropPos(sdevpv, getPropPos(sdevpv)*ppmm);
            setPropPos(meanpv, getPropPos(meanpv)*ppmm);
#endif
            PropVec cprop(getIdentityPropTF());
            if(backwards) invPropTF(cprop, meanpv);
            else fwdPropTF(cprop, meanpv); //cool form of: cprop = meanpv
            EMDistribution* inpdist = 
                (EMDistribution*) new EMDGauss(PropVec(0.f), sdevpv);
            emdist = (EMDistribution*) new EMDXformer(inpdist, wprop);
            ((EMDXformer*)emdist)->setMean(cprop);
        }
        break;
    }
    return emdist;
}

void SubStructure::getLBUB(PropVec& lb, PropVec& ub,
             const PropVec& wprop, float ppmm, float radius) const
{
    lb = ub = wprop;
    PropVec tflb(m_Mean);
    tflb -= (m_Stdev*radius);
    PropVec tfub(m_Mean);
    tfub += (m_Stdev*radius);
#ifndef _PROPTF_NORMALIZE_SCALE_
    setPropPos(tflb, getPropPos(tflb)*ppmm);
    setPropPos(tfub, getPropPos(tfub)*ppmm);
#endif
    fwdPropTF(lb, tflb);
    fwdPropTF(ub, tfub);
    if(getPropScale(lb)<0.001) setPropScale(lb, 0.001);
    setPropDir(lb, 0);
    setPropDir(ub, 2*M_PI);
}

void SubStructure::clear() 
{ 
    m_SubStructName.clear(); 
    m_SupStructName.clear(); 
    m_Transform = 0.f; 
    m_Pivot = -1;
    m_Polar = false;
    m_Mode = MODE_NONE;
    m_RateWeight = SUBSTRUCT_RATEWEIGHT;
    m_RateTH = SUBSTRUCT_RATETH;
}

bool SubStructure::read(ParseFile& is) 
{
    if(is.getKey() != "substruct") return false;
    clear();
    m_SubStructName = is.getValue();
    bool readon = true;
    while(readon && is.getNextLine()) {
        const string& key = is.getKey();
        const string& value = is.getValue();
        if(key == "transform") {
            fromString(value, m_Transform);
        } else if(key == "pivot") {
            fromString(value, m_Pivot);
            m_Pivot--;
        } else if(key == "rateweight") {
            fromString(value, m_RateWeight);
        } else if(key == "rateth") {
            fromString(value, m_RateTH);
        } else if(key == "mode") {
            if(value.find("nospawn") != value.npos)
                m_Mode |= MODE_NOSPAWN;
            if(value.find("showstats") != value.npos)
                m_Mode |= MODE_SHOWSTATS;
        } else if (key == "end") readon = false;
        else { is.setParseError("error parsing substructure"); return false; }
    }
    if(is.error()) return false;
    return true;
}

ostream& operator<<(ostream& os, const SubStructure& ss)
{
    os << "  substruct " << ss.m_SubStructName << endl;
    os << "    transform " << ss.m_Transform << endl;
    if(ss.m_Pivot>=0)
        os << "    pivot " << ss.m_Pivot+1 << endl;
    if(ss.m_Polar) os << "    polar" << endl;
    if(ss.m_RateWeight != SUBSTRUCT_RATEWEIGHT)
        os << "    rateweight " << ss.m_RateWeight << endl;
    if(ss.m_Mode != SubStructure::MODE_NONE) {
        os << "    mode" 
           << (ss.m_Mode&SubStructure::MODE_NOSPAWN ? " nospawn" : "")
           << (ss.m_Mode&SubStructure::MODE_SHOWSTATS ? " showstats" : "") 
           << endl;
    }
    os << "  end    # of substruct" << endl;
    return os;
}


