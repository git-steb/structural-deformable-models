/* species database -*- C++ -*- */
#ifndef _SPECIESDB_H_
#define _SPECIESDB_H_

#include <iostream>
#include <exception>
#include <map>
#include <list>
#include "common.h"
#include "ParseFile.h"

class Species : public std::map<std::string, std::string> {
public:
#ifndef SWIG
    static const char* SFIELDS[];
    const static char s_SPFLAGS[];
#endif
    enum SpFlags {
        FLIPPED=0x01, QUALITY=0x02, SCALE=0x04, SEL1=0x08, DEL=0x10,
        SEL2=0x20, SEL3=0x04, SEL4=0x80, SEL5=0x100
    };
    enum  SpCompare { CMP_EQUALS, CMP_CONTAINS, CMP_LESSER, CMP_GREATER };
    Species(dword _id=0, dword _flags=0) : id(_id), m_Flags(_flags) {}
    Species &operator=(const Species &rhs) {
        if(&rhs == this) return *this;
        std::map<std::string, std::string>::operator=(rhs);
        id = rhs.id;
        m_Flags = rhs.m_Flags;
        return *this;
    }
    void reset();
    bool hasFlag(dword flag) const;
    dword setFlag(dword flag);
    dword unsetFlag(dword flag);
    dword readFlagString(const std::string &flags);
    std::string getFlagString() const;
    bool select(const Species& sp,
                enum SpCompare how=Species::CMP_CONTAINS) const;
    dword getScale() const;
    friend ParseFile& operator>>(ParseFile &is, Species &sp);
    friend std::ostream& operator<<(std::ostream &os, const Species &sp);

    dword id;
    dword m_Flags;
};

class SpeciesDB : public std::map<dword, Species> {
public:
    SpeciesDB() {}
    SpeciesDB(const char* filename) {
        load(filename);
    }
    bool load(const char* filename);

    friend ParseFile& operator>>(ParseFile &is, SpeciesDB &sp);
    friend std::ostream& operator<<(std::ostream &os, const SpeciesDB &sp);
    const std::string& getDirectory() const { return m_Directory; }
    const std::string& getFilename() const { return m_Filename; }
    dword select(const Species& rhs,
                 enum Species::SpCompare how=Species::CMP_CONTAINS);
    dword select(const std::list<dword>& idlist);
    dword getSelectionID(int dir=0, bool wrap=true);
    dword loadSelection(const std::string& fname)
        { return loadSelection(fname, m_Selection); }
    dword writeSelection(const std::string& fname) const
        { return writeSelection(fname, m_Selection); }
    static dword loadSelection(const std::string& fname,std::list<dword>& sel);
    static dword writeSelection(const std::string& fname,
                                const std::list<dword>& sel);
protected:
    std::string                      m_Directory;
    std::string                      m_Filename;
    std::list<dword>                 m_Selection;
    std::list<dword>::iterator       m_CSel;
};

/** This class helps to select subsets of a data base.  As well it
    assists in navigation through the selectors and the resulting
    selections.*/
class DBSelector {
public:
    DBSelector(SpeciesDB &db) : m_DB(db), m_CSel(m_Selectors.begin()) {};
    DBSelector(SpeciesDB &db, const char* filename)
        : m_DB(db), m_CSel(m_Selectors.begin())
        { load(filename); }
    /** Load a set of species used as selectors for filtering the data base.
        Calls update() afterwards. */
    bool load(const char* filename);
    //!should be called when data base has changed
    void update();
    //!Returns a handle to the currently selected species in the data base.
    Species& getCurSpecies()
        {
            std::map<dword,Species>::iterator n = m_DB.find(
                m_DB.getSelectionID());
            if(n != m_DB.end()) return n->second;
            else return m_CSpecies;
        }
    /**Move on to next selection.
       \param dir -1 move backwards, 1 move forwards, 0 just refresh
       \param wrap if false then return false if end of selection list has been reached. Otherwise just wrap around.
    */
    bool nextSelection(int dir=1, bool wrap=true);
    /**Move on to next selector.
       \param walkdir -1=move backwards, 1=move forwards (default), 0=just refresh
       \param wrap if false then return false if end of selector list has been reached. Otherwise just wrap around.
    */
    bool nextSelector(int walkdir=1, bool wrap=true, dword mincount=1);

public:
    SpeciesDB                   &m_DB;
    std::list<Species>               m_Selectors;
    std::list<Species>::iterator     m_CSel;
    Species                     m_CSpecies;
};

#ifdef _SP_DEFINE_
const char* Species::SFIELDS[] = {
    "species_id",
    "order",
    "family",
    "name",
    "cname",
    "type",
    "image",
    "flags",
    "scale",
    NULL
};
const char Species::s_SPFLAGS[] = "fqs1d2345\0x00";
#endif

#endif
