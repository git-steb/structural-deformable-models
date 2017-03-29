#ifndef _FFIND_H_
#define _FFIND_H_

#include <iostream>
#include <string>
#include <list>

#if defined(LINUX) || defined(DARWIN)
#include <libgen.h>
#include <dirent.h>
#endif

#include "utils.h"

class FFind
{
public:
    FFind() {}
    FFind(const std::string& pattern) { setPattern(pattern); }
    void setPattern(const std::string& pattern) {
        char *dname=NULL,*bname=NULL;
#if defined(LINUX) || defined(DARWIN)
        char *p1 = strdup(pattern.c_str());
        char *p2 = strdup(pattern.c_str());
        dname = dirname(p1);
        bname = basename(p2);
        m_Pattern = bname;
        m_Dir = dname;
        if(p1) free(p1);
        if(p2) free(p2);
#endif
    }

    template<class T>
        int find(const std::string& fmask, T iter) {
        setPattern(fmask);
        int n,nn;
#if defined(LINUX) || defined(DARWIN)
        struct dirent **namelist;
        // THIS IS NOT THREADSAFE :-(
        __set_fnmatchpattern(m_Pattern);
        nn = n = scandir(m_Dir.c_str(), &namelist, __fixed_fnmatch, alphasort);
        if (n < 0)
            return 0;
        else {
            while(n--) {
                *iter++ = namelist[n]->d_name;
                free(namelist[n]);
            }
            free(namelist);
        }
#else
        nn = n = ::findFiles(fmask, iter);
#endif
        return nn;
    }

    template<class T>
        static int findFiles(const std::string& fmask, T iter) {
        FFind ff;
        return ff.find(fmask, iter);
    }

protected:
    std::string m_Pattern;
    std::string m_Dir;
};

#endif
