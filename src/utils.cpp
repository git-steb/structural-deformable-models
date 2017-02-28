#include <stdlib.h>
#include <fstream>
#include <string>
#ifdef LINUX
#include <fnmatch.h>
#endif
#include "utils.h"

using namespace std;

static string matlabprog = "matlab -nojvw -nosplash -nodesktop -r ";
static string matpath = "./";

static string __fnmatchpattern;
const string& __set_fnmatchpattern(const string& pattern)
{ return __fnmatchpattern = pattern; }
#ifdef LINUX
int __fixed_fnmatch(const struct dirent* dent) 
{ return !fnmatch(__fnmatchpattern.c_str(), dent->d_name,0); }
#else
int __fixed_fnmatch(const struct dirent* dent)
{ return 0; }
#endif

const char* getTemp() 
{
    const char* tc = getenv("TEMP");
    if(!tc) tc = getenv("TMP");
    return tc;
}

string& replaceAll(string& txt, char src, char tar)
{
    int pos = 0;
    while((pos = txt.find(src,pos)) != (int)txt.npos) {
        txt[pos++] = tar;
    }
    return txt;
}

int matlabCall(const std::string& cmd)
{
    return system((matlabprog+"\""+cmd+", exit\"").c_str());
}

/*
int matlabCall(const std::string& cmd)
{
    string temp = getTemp();
    string tmpfile = temp+"/matlabscript.m";
    string wintmpfile = tmpfile;
    replaceAll(wintmpfile,'/','\\');
    ofstream mf(tmpfile.c_str());
    if(mf) {
        mf << "% This is an automatically generated matlab script file"<<endl;
        mf << cmd << endl;
        mf << "exit" << endl;
        mf.close();
        int ret = system((matlabprog+wintmpfile).c_str());
        remove(tmpfile.c_str());
        return ret;
    } else cerr << "could not create tempfile " << tmpfile << endl;
    return -1;
}
*/

#ifdef NOGLUT
extern "C" {
void glutBitmapCharacter(void *font, int character)
{}
int glutBitmapWidth(void *font, int character)
{ return 0; }
}
#endif
