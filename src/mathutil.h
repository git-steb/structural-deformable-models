#ifndef _MATHUTIL_H_
#define _MATHUTIL_H_

#include <math.h>
#include "simpletypes.h"
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265359
#endif
#ifndef M_1_PI
#define M_1_PI (1/M_PI)
#endif
#ifndef M_E
#define M_E 2.718281828
#endif
#ifndef M_SQRT2PI
#define M_SQRT2PI 2.5066283
#endif

inline int absint(int v) { return v<0 ? -v : v;}

template <class T>
bool clamp(T& x, T& y, const T x0, const T y0, const T x1, const T y1)
{
    bool ret=false;
    if(x<x0) { ret = true; x = x0;}
    if(x>x1) { ret = true; x = x1;}
    if(y<y0) { ret = true; y = y0;}
    if(y>y1) { ret = true; y = y1;}
    return ret;
}

/*
  template <class T>
  T min(const T a, const T b)
  {
  return a<b ? a : b;
  }

  template <class T>
  T max(const T a, const T b)
  {
  return a>b ? a : b;
  }
*/

#define frand(max) (((float)rand())*(max)/RAND_MAX)
#define FRAND1 (((float)rand())/RAND_MAX)

/* Polar (Box-Mueller) method; See Knuth v2, 3rd ed, p122 */
/* from GSL/gauss.c */
inline float fgauss01()
{
    float x, y, r2;
    do
    {
        /* choose x,y in uniform square (-1,-1) to (+1,+1) */
        x = -1 + frand(2);
        y = -1 + frand(2);
        /* see if it is in the unit circle */
        r2 = x * x + y * y;
    } while (r2 > 1.0 || r2 == 0);
    /* Box-Mueller transform */
    return y * sqrt (-2.0 * log (r2) / r2);
}

inline float gauss(float x, float stdev) {
    return exp(-x*x/(2*stdev*stdev)) / (stdev*M_SQRT2PI);
}
inline float gauss2(float x2, float stdev) {
    return exp(-x2/(2*stdev*stdev)) / (stdev*M_SQRT2PI);
}
inline double gaussd(float x, double stdev) {
    return exp(-x*x/(2*stdev*stdev)) / (stdev*M_SQRT2PI);
}
inline float gaussd2(float x2, double stdev) {
    return exp(-x2/(2*stdev*stdev)) / (stdev*M_SQRT2PI);
}

inline float mapAngle360(float a) {
    int d=(int)a/360 - (int)(a<0);
    return a-360*(float)d;
}

inline float mapAngle360(float a, float mina) {
    float ashift = a-mina;
    int d=(int)ashift/360 - (int)(ashift<0);
    return a-360*(float)d;
}

inline float mapAngle180(float a) {
    a = mapAngle360(a);
    return a<=180 ? a : a-360;
}

inline float mapAngle2PI(float a) {
    float turns=floor(a*(0.5*M_1_PI));
    return a-(2*M_PI)*turns;
}

inline float mapAngle2PI(float a, float mina) {
    float turns=floor((a-mina)*(0.5*M_1_PI));
    return a-(2*M_PI)*turns;
}

inline float mapAnglePI(float a) {
    a = mapAngle2PI(a);
    return (a>M_PI) ? a-(2*M_PI) : a;
}

inline float mapAnglePI(float a, float cnta) {
    a = mapAngle2PI(a, cnta);
    return (a-cnta>M_PI) ? a-(2*M_PI) : a;
}

inline dword power2(dword x)
{
    return 1<< int(ceil(log(float(x))/log(2.f)));
}

#endif
