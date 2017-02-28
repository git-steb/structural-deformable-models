#ifndef _UTILS_H_
#define _UTILS_H_

#include <iostream>
#include <sstream>
#include <string>
#ifdef WIN32
#include <io.h>
#endif
#ifdef LINUX
#include <dirent.h>
#endif
#include <ctype.h>
#include <assert.h>
#include <sys/time.h>
#include "common.h"

#define MAKE_STRING( msg ) \
 ( ((std::ostringstream&)(std::ostringstream() << msg)).str() )
#define MAKE_CSTRING( msg ) \
 ( ((std::ostringstream&)(std::ostringstream() << msg)).str().c_str() )

#define COMMENT_CHAR '#'

inline std::istream& comment(std::istream &s)
{
  char c;
  while (s.get(c)) {
    if (!isspace(c)) {
      if (c == COMMENT_CHAR) {
	while (s.get(c) && c != '\n');
      } else {
	s.putback(c);
	break;
      }
    }
  }
  return s;
}

inline unsigned long getMilliSeconds() {
#ifdef WIN32
//     typedef unsigned long LONGLONG;
//     typedef long LARGE_INTEGER;
    
    static LONGLONG frequency=0;
    if(!frequency)
        QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
    LONGLONG counter;
    QueryPerformanceCounter((LARGE_INTEGER*)&counter);
    return (unsigned long) (counter*1000 / frequency);
#else
    static struct timeval time;
    gettimeofday(&time, 0);
    return (unsigned long)time.tv_sec*1000 + (unsigned long)time.tv_usec/1000;
#endif
}

template<class T>
std::string toString(T v) {
    std::stringstream ss;
    ss << v;
    return ss.str();
}

template<class T>
T& fromString(const std::string& str, T& v) {
    std::istringstream ss(str);
    ss >> v;
    return v;
}

std::string& replaceAll(std::string& txt, char src, char tar);

int matlabCall(const std::string& cmd);

const char* getTemp();

// only usable if LINUX defined
const std::string& __set_fnmatchpattern(const std::string& pattern);
int __fixed_fnmatch(const struct dirent* dent);

#ifdef LINUX
template<class T>
int findFiles(const std::string& fmask, T iter) { return 0; }
#else
template<class T>
int findFiles(const std::string& fmask, T iter) {
    struct _finddata_t ff;
    int ffhandle;
    int ffcount=0;
    if((ffhandle = _findfirst(fmask.c_str(), &ff)) != -1) {
        do {
            *iter++ = ff.name;
            ffcount++;
        } while(_findnext(ffhandle, &ff) == 0);
    }
    return ffcount;
}
#endif 

#endif
