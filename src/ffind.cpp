#include <iostream>
#include <string>
#include <list>

#include <libgen.h>
#include <dirent.h>
#include <fnmatch.h>
using namespace std;


char __fnmatchpattern[2048];
int __fixed_fnmatch(const struct dirent* dent) {
    return !fnmatch(__fnmatchpattern, dent->d_name,0);
}

class FFind
{
public:
    FFind(const string& pattern) { setPattern(pattern); }
    void setPattern(const string& pattern) {
        char *dname=NULL,*bname=NULL;
        char *p1 = strdup(pattern.c_str());
        char *p2 = strdup(pattern.c_str());
        dname = dirname(p1);
        bname = basename(p2);
        m_Pattern = bname;
        m_Dir = dname;
        if(p1) free(p1);
        if(p2) free(p2);
    }

    template<class T>
    int findFiles(const std::string& fmask, T iter) {
        setPattern(fmask);
        struct dirent **namelist;
        int n;
        // THIS IS NOT THREADSAFE :-(
        strncpy(__fnmatchpattern, m_Pattern.c_str(), 2047);
        n = scandir(m_Dir.c_str(), &namelist, __fixed_fnmatch, alphasort);
        if (n < 0)
            perror("scandir");
        else {
            while(n--) {
                *iter++ = namelist[n]->d_name;
                free(namelist[n]);
            }
            free(namelist);
        }

    }

protected:
    string  m_Pattern;
    string  m_Dir;
};

void perror(const char* errmsg) {
    cerr << errmsg << endl;
}

int main(){
    struct dirent **namelist;
    int n;
    FFind ff("./*.cpp");
    ff.findAll();

    return 0;
}
