#ifdef WIN32
#include <windows.h>
#endif
#include <sstream>
#define _SP_DEFINE_
#include "SpeciesDB.h"
#include "Errors.h"
using namespace std;

//-----------------------------------------------------------------------------
//SpeciesDB

bool SpeciesDB::load(const char* filename) 
{
    clear();
    ifstream ifs(filename);
    ifs.close();
    ParseFile pf(filename);
    if(pf) {
        try {
            operator>>(pf, *this);
        } catch (const IOException *e) {
            cout << "Error loading " << filename << " message: " 
                 << e->getMessage() << endl;
            return false;
        }
        cout << "read " << size() << " entries from " << filename << endl;
        m_Directory = pf.getPath();
        m_Filename = pf.getFilename();
        return true;
    } else {
        cout << "Error loading " << filename << endl;
        return false;
    }
}

dword SpeciesDB::select(const Species& sp, enum Species::SpCompare how)
{
    m_Selection.clear();
    for(map<dword,Species>::const_iterator s=begin(); s!=end(); s++)
        if(s->second.select(sp, how))
            m_Selection.push_back(s->second.id);
    m_CSel = m_Selection.begin();
    return m_Selection.size();
}

dword SpeciesDB::select(const std::list<dword>& idlist)
{
    m_Selection = idlist;
    m_CSel = m_Selection.begin();
    return m_Selection.size();
}

dword SpeciesDB::getSelectionID(int dir, bool wrap)
{
    if(m_Selection.empty()) return 0;
    if(dir==1) { // dir>0
        m_CSel++;
        if(m_CSel==m_Selection.end()) {
            m_CSel = m_Selection.begin();
            if(!wrap) return 0;
        }
    } else if(dir>1) {
        m_CSel = m_Selection.end();
        m_CSel--;
    } else if(dir==-1) { // dir<0
        if(m_CSel==m_Selection.begin()) {
            m_CSel = m_Selection.end();
            m_CSel--;
            if(!wrap) return 0;
        } else m_CSel--;
    } else if(dir < -1) {
        m_CSel = m_Selection.begin();
    }
    return *m_CSel;
}

dword SpeciesDB::loadSelection(const std::string& fname, 
                                      std::list<dword>& sel)
{
    ifstream is(fname.c_str());
    if(is) {
        sel.clear();
        is >> ws;
        while(is) {
            dword selid=0;
            is >> selid;
            if(selid) sel.push_back(selid);
        }
    } else return 0;
    return sel.size();
}

dword SpeciesDB::writeSelection(const std::string& fname,
                                       const std::list<dword>& sel)
{
    ofstream os(fname.c_str());
    if(os) {
        for(list<dword>::const_iterator csel = sel.begin();
            csel != sel.end(); csel++)
        {
            os << *csel << endl;
        }
    } else return 0;
    return sel.size();
}

ParseFile& operator>>(ParseFile &is, SpeciesDB &sp)
{
    Species s;
    while(is) {
        is >> s;
        if(s.id) sp[s.id] = s;
    }
    return is;
}

ostream& operator<<(ostream &os, const SpeciesDB &sp)
{
    for(map<dword,Species>::const_iterator s=sp.begin(); s!=sp.end(); s++)
        os << s->second << endl;
    return os;
}

//-----------------------------------------------------------------------------
//Species

ParseFile& operator>>(ParseFile &is, Species &sp)
{
    sp.clear();
    sp.id = 0;
    sp.m_Flags = 0;
    while(is.getNextLine()) {
        const string& token = is.getKey();
        const string& value = is.getValue();
        //cout << "token: " << token << " value: " << value << endl;
        if(token == "species_id") {
            if(sp.id != 0) { is.pushLine(); return is; }
            istringstream(value) >> sp.id;
        } else if(sp.id == 0) {
            throw new IOException("record didn't start with 'species_id'");
        } else if(token == "flags")
            sp.readFlagString(value);
        sp[token] = value;
    }
    return is;
}

ostream& operator<<(ostream &os, const Species &sp)
{
    int entry=0;
    while(Species::SFIELDS[entry] != NULL) {
        const string fields(Species::SFIELDS[entry]);
        map<string,string>::const_iterator s=sp.find(fields);
        if(sp.m_Flags && fields == "flags")
            os << fields << " " << sp.getFlagString() << endl;
        else if(s != sp.end()) os << s->first << " " << s->second << endl;
        entry++;
    }
    return os;
}

void Species::reset() {
    clear();
    m_Flags = 0;
    id = 0;
}

bool Species::hasFlag(dword flag) const {
    return m_Flags & flag;
}

dword Species::setFlag(dword flag) {
    return m_Flags |= flag;
}

dword Species::unsetFlag(dword flag) {
    return m_Flags &= ~flag;
}

string Species::getFlagString() const {
    dword bv=1;
    string fs;
    for(int b=0; b<32 && s_SPFLAGS[b]; b++,bv=bv<<1)
        if(m_Flags&bv) fs += s_SPFLAGS[b];
    return fs;
}

dword Species::readFlagString(const string &flags) {
    m_Flags = 0;
    dword bv=1;
    for(int b=0; b<32 && s_SPFLAGS[b]; b++,bv=(bv<<1))
        if(flags.find(s_SPFLAGS[b]) != flags.npos) m_Flags |= bv;
    return m_Flags;
}

bool Species::select(const Species& sp, enum SpCompare how) const
{
    for(map<string,string>::const_iterator rs=sp.begin();
        rs != sp.end(); rs++)
    {
        map<string,string>::const_iterator s=find(rs->first);
        if(s == end()) return false;
        const string &lv = s->second;
        const string &rv = rs->second;
        if(s->first == "flags") {
            for(string::const_iterator f=rv.begin(); f!=rv.end(); f++)
                if(lv.find(*f)==lv.npos) return false;
        } else {
            switch(how) {
                case CMP_EQUALS:
                    if(lv != rv) return false;
                    break;
                case CMP_CONTAINS:
                    if(lv.find(rv) == lv.npos) return false;
                    break;
                case CMP_LESSER:
                    if(lv > rv) return false;
                    break;
                case CMP_GREATER:
                    if(lv < rv) return false;
                    break;
            }
        }
    }
    return true;
}

dword Species::getScale() const
{
    dword scale=0;
    const_iterator s=find("scale");
    if(s!=end()) {
        istringstream(s->second)>>scale;
    }
    return scale;
}
//-----------------------------------------------------------------------------
//DBSelector

bool DBSelector::load(const char* filename)
{
    ParseFile rf(filename);
    while(rf) {
        Species sp;
        rf >> sp;
        sp.erase("species_id");
        m_Selectors.push_back(sp);
    }
    cout << m_Selectors.size() << " records read from " << filename << endl;
    update();
    return true;
}

void DBSelector::update()
{
    m_CSel = m_Selectors.begin();
    nextSelector(0);
}

bool DBSelector::nextSelection(int dir, bool wrap)
{
    if(m_DB.empty()) return false;
    if(!m_DB.getSelectionID()) {
        map<dword,Species>::iterator n = m_DB.find(m_CSpecies.id);
        if(n != m_DB.end()) {
            if(dir>0) {
                n++;
                if(n == m_DB.end()) {
                    n = m_DB.begin();
                    if(!wrap) return false;
                }
            } else if(dir<0) {
                if(n == m_DB.begin()) {
                    n = m_DB.end();
                    n--;
                    if(!wrap) return false;
                } else n--;
            }
            m_CSpecies = n->second;
        } else return false;
    } else {
        map<dword,Species>::iterator n = m_DB.find(
            m_DB.getSelectionID(dir,wrap));
        if(n != m_DB.end()) m_CSpecies = n->second;
        else return false;
    }
    return true;
}

bool DBSelector::nextSelector(int walkdir, bool wrap, dword mincount) {
    bool ret = true;
    list<Species>::iterator osel = m_Selectors.end();
    while(m_CSel != osel) 
    {
        if(osel == m_Selectors.end()) osel = m_CSel;
        if(walkdir<0) {
            if(m_CSel == m_Selectors.begin()) {
                m_CSel = m_Selectors.end();
                ret = wrap;
            }
            m_CSel--;
        } else if(walkdir>0) {
            m_CSel++;
            if(m_CSel == m_Selectors.end()) {
                m_CSel = m_Selectors.begin();
                ret = wrap;
            }
        }
        dword nsel = m_DB.select(*m_CSel);
        if(nsel >= mincount) {
            cout << nsel << " records found using selector:" << endl 
                 << *m_CSel;
            nextSelection(0);
            osel = m_CSel;
        } else {
            cout << " no records found using selector:" <<endl<<*m_CSel;
        }
    }
    if(m_CSel == m_Selectors.end()) {
        m_CSel = m_Selectors.begin();
        return false;
    }
    return ret;
}
