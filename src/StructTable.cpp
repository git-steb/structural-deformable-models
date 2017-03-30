#include <fstream>
#include <iomanip>
#include <set>
#include <algorithm>
#include <iterator>
#include <fxkeys.h>
#include "Brain.h"
#include "DMatrix.h"
#include "DMatrixLinAlg.h"
#include "utils.h"
#include "StructTable.h"
#include "Searcher.h"

using namespace std;

//---------------------------------------------------------------------------

StructTable::StructTable(Brain& brain, const string& filename)
{
    m_TMarks.clear();
    m_TMarks.resize(TM_LAST, 0.0f);
    attachBrain(brain);
    load(filename);
}

StructTable::~StructTable()
{
    clear();
}

void StructTable::clear()
{
    m_Structs.clear();
    m_CurStruct.clear();
    m_Time = 0;
    m_State = 0;
    m_TMarks.clear();
    m_TMarks.resize(TM_LAST, 0.0f);
    m_BestPath.clear();
    m_Interpretations.clear();
}

dataset_ptr StructTable::getDataset()
{
    return m_Brain->getSensorData().shared_from_base<Dataset>();
}

SensorCollection* StructTable::getSensors()
{ return (SensorCollection*) &m_Brain->getSensors(); }

void StructTable::reattachSensors()
{
    for(map<string,MStructure>::iterator si = m_Structs.begin();
        si != m_Structs.end(); si++)
    {
        si->second.getModel().reattachSensors();
    }
}

void StructTable::adaptDataScale()
{
    for(map<string,MStructure>::iterator si = m_Structs.begin();
        si != m_Structs.end(); si++)
    {
        si->second.getModel().adaptDataScale();
    }
}

bool StructTable::load(const string& filename)
{
    if(filename.empty()) return false;
    cout << "loading " << filename << endl;
    ParseFile is(filename);
    if(is) m_FileName = filename;
    return read(is);
}

bool StructTable::read(ParseFile &is)
{
    if(is) {
        clear();
        m_Path = is.getPath();
        MStructure cstructure("", this);
        while(is) {
            if(cstructure.read(is)) {
                if(cstructure) m_Structs[cstructure.getName()] = cstructure;
            } else if(!readInterpretations(is))
                is.setParseError(
                    string("error reading structure table. ") +
                    "(A structure has to begin with struct <name>)");
        }
        connectSubSuper();
        evalRelations();
    }
    if(is.error()) { cerr << is.getErrorMsg() << endl; return false; }
    return true;
}

bool StructTable::readInterpretations(ParseFile& is)
{
    m_Interpretations.clear();
    bool readon = true, allright = false;
    while(is.getNextLine() && readon) {
        if(is.getKey() == "interpretations") allright = true;
        else if(!allright) { is.pushLine(); return false; }
        else if(is.getKey() == "end") readon = false;
        else {
            set<string>& elements = m_Interpretations[is.getKey()];
            istringstream slist(is.getValue());
            slist >> ws;
            while(slist) {
                string structure; slist >> structure;
                if(structure.empty()) break;
                elements.insert(structure);
                slist >> ws;
            }
            if(elements.empty())
                is.setParseError("error reading structure interpretation");
        }
    }
    return true;
}

void StructTable::write(ostream &os) const
{
    for(map<string,MStructure>::const_iterator si = m_Structs.begin();
        si != m_Structs.end(); si++)
    {
        si->second.write(os);
    }
    if(!m_Interpretations.empty()) {
        os << "interpretations" << endl;
        for(map<string,set<string> >::const_iterator ip =
                m_Interpretations.begin(); ip != m_Interpretations.end(); ip++)
        {
            os << "  " << ip->first;
            for(set<string>::const_iterator sp = ip->second.begin();
                sp != ip->second.end(); sp++)
                os << " " << *sp;
            os << endl;
        }
        os << "end" << endl << endl;
    }
}

bool StructTable::hasStructure(const std::string& sname) const
{
    return m_Structs.find(sname) != m_Structs.end();
}

const MStructure& StructTable::getStructure(const std::string& sname) const
{
    return m_Structs.find(sname)->second;
}

MStructure& StructTable::getNextStruct(int dir, bool wrap)
{
    static MStructure nostruc;
    if(m_Structs.empty()) return nostruc;
    if(m_CurStruct.empty()) {
        m_CurStruct = m_Structs.begin()->first;
        return m_Structs.begin()->second;
    }
    map<string,MStructure>::iterator st = m_Structs.find(m_CurStruct);
    if(st == m_Structs.end()) {
        m_CurStruct = m_Structs.begin()->first;
        return m_Structs.begin()->second;
    }
    if(dir > 0) {
        st++;
        if(st == m_Structs.end()) {
            if(wrap) st = m_Structs.begin();
            else { m_CurStruct.clear(); return nostruc; }
        }
        m_CurStruct = st->first;
    } else if(dir < 0) {
        if(st == m_Structs.begin()) {
            if(wrap) st = m_Structs.end();
            else { m_CurStruct.clear(); return nostruc; }
        }
        st--;
        m_CurStruct = st->first;
    }
    return st->second;
}

void StructTable::evalRelations()
{
//     int ret = matlabCall(string("addpath ")+m_Path + ", antpca");
//     DUMP(ret);
    for(map<string,MStructure>::iterator si = m_Structs.begin();
        si != m_Structs.end(); si++)
    {
        si->second.buildAllStats();
    }
    //rebuildExpMaps();
}

void StructTable::rebuildExpMaps()
{
    for(map<string,MStructure>::iterator si = m_Structs.begin();
        si != m_Structs.end(); si++)
        si->second.rebuildExpMap();
}

void StructTable::connectSubSuper()
{
    for(map<string,MStructure>::iterator si = m_Structs.begin();
        si != m_Structs.end(); si++)
        si->second.refSubSuper(true);   // just clear super-list
    for(map<string,MStructure>::iterator si = m_Structs.begin();
        si != m_Structs.end(); si++)
        si->second.refSubSuper();       // update super-list of substructs
}

DMatrixf& StructTable::adjustByAvgDir(DMatrixf& mat)
{
    Point2D dir(0,0);
    DMatrixf::iterator el = mat.getData().begin();
    for(dword ind=0; ind<mat.size(); ind++, el++)
        dir += Point2D(1,0).rotate(*el);
    float avgdir = dir.angle();
    el = mat.getData().begin();
    for(dword ind=0; ind<mat.size(); ind++, el++)
        *el = mapAnglePI(*el, avgdir);
    return mat;
}

bool StructTable::triggerTest(int mx, int my, int what)
{
    switch(what)
    {
    case KEY_t:
        rebuildExpMaps();
        break;
    case KEY_g:
        evalRelations();
        break;
    case KEY_n:
    {
        for(map<string,MStructure>::iterator si = m_Structs.begin();
            si != m_Structs.end(); si++)
            si->second.buildMasterModel();
    }
    break;
    case KEY_v:
    {
        findBestConnection();
//             for(map<string,MStructure>::iterator si = m_Structs.begin();
//                 si != m_Structs.end(); si++)
//                 si->second.verifyWinnerRating();
    }
    break;
    case KEY_c:
        //m_TMarks[TM_TIMEOUT] = m_Time + 10;
        m_State = m_State ^ ST_RESULT;
        break;
    case KEY_u:
        cout << "starting structure search" << endl;
        startSearch();
        break;
    case KEY_i:
        cout << "stopping structure search" << endl;
        startSearch(false);
        break;
    default:
    {
        if(what == KEY_h) {
            m_State&=~ST_RUN;
        } if(what == KEY_s) {
            m_State^=ST_RUN;
        } else if(what == KEY_r) {
            m_Time=0;
            m_TMarks[TM_UPDEM] = m_TMarks[TM_TIMEOUT] = 0;
            m_BestPath.clear();
        }
        bool ret=false;
        for(map<string,MStructure>::iterator si = m_Structs.begin();
            si != m_Structs.end(); si++)
            ret = si->second.m_Searcher.triggerTest(mx,my,what);
        return ret;
    }
    }
    return true;
}

void StructTable::startSearch(bool dostart)
{
    if(dostart) {
        m_State|=ST_RUN;
        m_State&=~(ST_DONE|ST_TIMEOUT);
        m_Time=0;
        m_TMarks[TM_UPDEM] = m_TMarks[TM_TIMEOUT] = 0;
        m_BestPath.clear();
        m_IPaths.clear();
    } else m_State &= ~ST_RUN;
    for(map<string,MStructure>::iterator si = m_Structs.begin();
        si != m_Structs.end(); si++)
    {
        if(dostart) si->second.m_Searcher.clear();
        si->second.m_Searcher.startSearch(dostart);
    }
}

bool StructTable::stepSearch(float dt)
{
    bool dotimeout=false;
    bool ret=true;
    if((m_State&ST_DONE)) return true;
    if((m_State&ST_RUN) == 0) return false;
    if(m_Time == 0) {
//initialize
        for(map<string,MStructure>::iterator si = m_Structs.begin();
            si != m_Structs.end(); si++)
            si->second.buildMasterModel();
        m_TMarks[TM_TIMEOUT] = m_Time + getSearchPara().m_EvolveCycle*3000;
    }
    if(dotimeout && m_TMarks[TM_TIMEOUT] - m_Time <= 0) {
//finalize
        m_State &= ~ST_RUN;
        m_State |= ST_RESULT;
        ret = false;
    } else {
        bool doupdate = m_TMarks[TM_UPDEM] - m_Time <= 0;
        if(doupdate) {   //pre update
            rebuildExpMaps();
            m_TMarks[TM_UPDEM] = m_Time + getSearchPara().m_EvolveCycle;
        }
        for(map<string,MStructure>::iterator si = m_Structs.begin();
            si != m_Structs.end(); si++)
        {
            if(doupdate) si->second.m_Searcher.evolve();
            ret &= si->second.stepSearch(dt);
        }
        if(doupdate) findBestConnection();
    }
    m_Time += dt;
    if(ret) {
        m_State |= ST_DONE;
        cout << "search terminated after " << m_Time << " seconds." << endl;
    }
    return ret;
}

bool StructTable::showNextIP(int dir) const
{
    bool ret = true;
    Interpretations::const_iterator cip = m_Interpretations.find(m_CurIP);
    if(cip == m_Interpretations.end()) {
        if(m_Interpretations.empty()) {
            m_CurIP.clear();
            return false;
        }
        cip = m_Interpretations.begin();
    }
    if(dir == 1) {
        cip++;
        if(cip==m_Interpretations.end()) {
            cip = m_Interpretations.begin();
            ret = false;
        }
    } else if(dir > 1) {
        cip = m_Interpretations.end(); cip--;
    } else if(dir == -1) {
        if(cip==m_Interpretations.begin()) {
            cip = m_Interpretations.end();
            ret = false;
        }
        cip--;
    } else if(dir < -1) {
        cip = m_Interpretations.begin();
    }
    m_CurIP = cip->first;
    return ret;
}

void StructTable::draw() const
{
    const string& ipname = getShownIP();
    map<string,StructPath>::const_iterator spath = m_IPaths.find(ipname);
    for(map<string,MStructure>::const_iterator si = m_Structs.begin();
        si != m_Structs.end(); si++)
    {
        if((m_State & ST_RESULT) == 0) si->second.m_Searcher.draw();
        if(spath != m_IPaths.end()) {
            StructPath::const_iterator wi = spath->second.find(si->first);
            if(wi != spath->second.end()) {
                const Winner *nw = si->second.getWinner(wi->second);
                if(nw && nw->m_Model) {
                    glColor3f(1,.1,0);
                    glLineWidth(4.0f);
                    nw->m_Model->draw();
                }
            }
        }
    }
}


float StructTable::findBestConnection()
{
    if(m_Structs.empty()) return m_BestPath.m_Goodness;
    m_BestPath.unprotectWinners(*this);
    m_BestPath.clear();
    for(map<string,MStructure>::iterator si = m_Structs.begin();
        si != m_Structs.end(); si++)
    {
        si->second.verifyWinnerRating();
    }

    m_IPaths.clear();
    for(Interpretations::const_iterator ip=m_Interpretations.begin();
        ip != m_Interpretations.end(); ip++)
    { // for each interpretation
        cout << "scanning interpretation " << ip->first << endl;
        m_NodePaths.clear();
        for(map<string,MStructure>::iterator si = m_Structs.begin();
            si != m_Structs.end(); si++)
        { // for each structure
            MStructure& st = si->second;
            StructPath bestpath;
            const map<dword,Winner>& winners = st.getSearcher().getWinList();
            for(map<dword,Winner>::const_iterator wi = winners.begin();
                wi != winners.end(); wi++)
            { // for each winner in current structure
                StructPath path;
                path.blockOtherPaths(ip->second, *this);
                findBestConnection(path, wi->second);
                if(path.m_Goodness > bestpath.m_Goodness)
                {
                    bestpath = path;
                }
            }
            if(bestpath.m_Goodness > m_IPaths[ip->first].m_Goodness)
            {
                m_IPaths[ip->first] = bestpath;
            }
            cout << "  best cost for " << si->first << ": "
                 << bestpath.m_Goodness
                 << " (" << bestpath.getRelGoodness() << ") " << endl;
            //<< " length: "<<bestpath.size()<<endl;
        }
        if(m_IPaths[ip->first].getRelGoodness() > m_BestPath.getRelGoodness())
        {
            m_BestPath = m_IPaths[ip->first];
            m_BestIP = ip->first;
        }
    }
    cout << "best interpretation is " << m_BestIP << endl;
    m_BestPath.protectWinners(*this);
    m_BestPath.print(cout, *this);
//     DUMP(m_BestPath.m_Goodness);
//     DUMP(m_BestPath.size());
//     cout << "showing path" << endl;
//     for(StructPath::const_iterator ps=m_BestPath.begin();
//         ps != m_BestPath.end(); ps++)
//     {
//         cout << ps->first << ": " << ps->second.m_BestRating << endl;
//     }
    return m_BestPath.getRelGoodness();
}

bool StructTable::findBestConnection(StructPath& path,
                                     const Winner& winner) const
{
    if(winner.m_WinnerID == Winner::WID_EMPTY) { TRACE0; return false; }
    const string& csname = winner.m_StructName;
    if(csname.empty()) { TRACE0; return false; }
    if(path[csname] != Winner::WID_EMPTY) {
        return false;           // termination criterion
    }
    if(winner.m_WinnerID == Winner::WID_FRAME) {
        path[csname] = winner.m_WinnerID;
        return true;     // confirm, but don't add goodness
    }
    NodePaths &np = m_NodePaths[csname];
    NodePaths::const_iterator wpath = np.find(winner.m_WinnerID);
    if(wpath != np.end()) { // we already have this path!
        path.merge(wpath->second);
        return true;
    }
    StructPath &bestpath = np[winner.m_WinnerID];
    bestpath = path.branch();

    const MStructure& cstruct = getStructure(csname);
    bestpath[csname] = winner.m_WinnerID;
    bestpath.m_Goodness += cstruct.getWeight()
        *winner.m_Model->getQualityOfFit();
    //* cstruct.m_Searcher.relativeQOF(winner.m_Model->getQualityOfFit());
    bestpath.m_Maxness  += cstruct.getWeight();

    for(Winner::Ratings::const_iterator wr = winner.m_Ratings.begin();
        wr != winner.m_Ratings.end(); wr++)
    {  // for each rating structure
        map<string,MStructure>::const_iterator sti =
            m_Structs.find(wr->first);
        if(sti != m_Structs.end()) {
            const MStructure& refstruct = sti->second;
            const SubStructure& subs =
                refstruct.m_SubStructures.find(csname)->second;
            StructPath spath = bestpath;
            for(map<dword,float>::const_iterator wi = wr->second.begin();
                wi != wr->second.end(); wi++)
            {
                const Winner* nextWinner = refstruct.getWinner(wi->first);
                StructPath cpath = spath;
                if(nextWinner && wi->second > subs.m_RateTH) {
                    if(findBestConnection(cpath, *nextWinner)) {
                        cpath.m_Goodness += subs.m_RateWeight*wi->second;
                        cpath.m_Maxness  += subs.m_RateWeight;
                    }
                }
                if(cpath.m_Goodness > bestpath.m_Goodness)
                    bestpath = cpath;
            }
        }
    }
    path.merge(bestpath);
    return true;
}

//----------------------------------------------------------------------------
StructPath::StructPath() : m_Goodness(0.f), m_Maxness(0.f),
                           m_WinnersProtected(false)
{}

StructPath::StructPath(const StructPath& rhs)
{
    operator=(rhs);
}

StructPath::~StructPath()
{
    //unprotectWinners();
}

StructPath& StructPath::operator=(const StructPath& rhs)
{
    if(this != &rhs) {
        map<string,dword>::operator=(rhs);
        m_Goodness = rhs.m_Goodness;
        m_WinnersProtected = rhs.m_WinnersProtected;
        m_Maxness = rhs.m_Maxness;
    }
    return *this;
}

void StructPath::clear()
{
    map<string,dword>::clear();
    m_Goodness = 0.0f;
    m_Maxness = 0.f;
    m_WinnersProtected = false;
}

Winner* StructPath::getWinner(MStructure& struc)
{
    iterator wi = find(struc.getName());
    if(wi != end() && wi->second) {
        Winner* winner = (Winner*)struc.getWinner(wi->second);
        if(!winner) wi->second = 0;
        return winner;
    } else return NULL;
}

const Winner* StructPath::getWinner(const MStructure& struc) const
{
    const_iterator wi = find(struc.getName());
    if(wi != end() && wi->second)
    {
        const Winner* winner = struc.getWinner(wi->second);
        if(!winner) (dword&)(wi->second) = 0;
        return winner;
    } else return NULL;
}

void StructPath::protectWinners(StructTable& structs)
{
    if(m_WinnersProtected) return;
    for(iterator wi = begin(); wi != end(); wi++)
    {
        if(wi->second>=Winner::WID_FIRST &&
           wi->second<=Winner::WID_LAST) {
            Winner* winner = (Winner*)structs.getStructs()[wi->first]
                .getWinner(wi->second);
            if(winner && winner->m_Model) {
                winner->m_Model->setFlags(Model::ST_NODEL);
            } else wi->second = 0;
        }
    }
    m_WinnersProtected = true;
}

void StructPath::unprotectWinners(StructTable& structs)
{
    if(!m_WinnersProtected) return;
    for(iterator wi = begin(); wi != end(); wi++)
    {
        if(wi->second>=Winner::WID_FIRST &&
           wi->second<=Winner::WID_LAST) {
            Winner* winner = (Winner*)structs.getStructs()[wi->first]
                .getWinner(wi->second);
            if(winner && winner->m_Model) {
                winner->m_Model->unsetFlags(Model::ST_NODEL);
            } else wi->second = 0;
        }
    }
    m_WinnersProtected = false;
}

void StructPath::blockOtherPaths(const set<string>& freepath,
                                 const StructTable& structs)
{
    for(map<string,MStructure>::const_iterator
            st = structs.getStructs().begin();
        st != structs.getStructs().end(); st++)
    {
        if(freepath.find(st->second.getName()) == freepath.end())
            operator[](st->second.getName()) = Winner::WID_BLOCKED;
    }
    m_WinnersProtected = false;
}

ostream& StructPath::print(ostream& os, const StructTable& structs) const
{
    os << setw(10);
    os << "path goodness: " << m_Goodness << " ("
       << getRelGoodness() << ")" << endl;
    for(const_iterator wi = begin(); wi != end(); wi++)
    {
        if(wi->second != Winner::WID_EMPTY &&
           wi->second != Winner::WID_BLOCKED)
        {
            os << "  " << wi->first;
            const MStructure& st = structs.getStructure(wi->first);
            const Winner* winner = st.getWinner(wi->second);
            if(winner) {
                if(winner->m_Model)
                    os << "\t ("<< st.getWeight() <<") * "
                       << winner->m_Model->getQualityOfFit() << endl;
                else os << "\t (no model)"<<endl;
                for(const_iterator ri = begin(); ri != end(); ri++)
                {
                    float rating = winner->getConnection(ri->first,
                                                         ri->second);
                    if(rating>=0.f) {
                        if(st.hasSupStruct(ri->first)) {
                            const SubStructure& subs =
                                st.getSupStruct(ri->first);
                            os << "    <--[" << ri->first
                               << "\t] = ("<<subs.m_RateWeight<<") * "
                               << rating << endl;
                        }
                    }
                }
                os << "       (best: " << winner->m_BestRating << ")" << endl;
            } else os << " (deleted)"<< endl;
        }
    }
    os << setw(0) << flush;
    return os;
}

StructPath StructPath::branch() const
{
    StructPath bp(*this);
    for(iterator wi = bp.begin(); wi != bp.end(); wi++)
        if(wi->second != Winner::WID_EMPTY) wi->second = Winner::WID_BLOCKED;
    bp.m_Goodness = 0.f; bp.m_Maxness = 0.f;
    return bp;
}

void StructPath::merge(const StructPath& mp)
{
    for(const_iterator wi = mp.begin(); wi != mp.end(); wi++)
        if(wi->second != Winner::WID_EMPTY &&
           wi->second != Winner::WID_BLOCKED)
            operator[](wi->first) = wi->second;
    m_Goodness += mp.m_Goodness; m_Maxness += mp.m_Maxness;
}
