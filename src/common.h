/* -*- C++ -*- */
#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef WIN32
#include <windows.h>
#endif
#include <assert.h>
#include "simpletypes.h"
#include <limits>

#ifndef NOTRACE
#define TRACE(msg) cerr << __FILE__ << ":" << __LINE__ << ": " << msg << endl
#define MSG(msg)    std::cout << msg << std::endl;
#define DUMP(expr)  std::cout << #expr << " = " << expr << std::endl
#define MARK(msg)   std::cout << ">>>> " << msg << std::endl;
#define LINE()      std::cout << "-----------------------" << std::endl
#else
#define TRACE(msg) 
#define MSG(msg)
#define DUMP(expr)
#define MARK(msg)
#define LINE()
#endif
#define TRACE0 TRACE("")

#endif
