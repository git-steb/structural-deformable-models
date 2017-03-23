#ifndef _AIRNAN_H_
#define _AIRNAN_H_

/* ---- begin of: stuff taken from Gordon Kindlman's teem/air lib */
#ifdef IRIX
#define TEEM_QNANHIBIT 0
#else
#define TEEM_QNANHIBIT 1
#endif

/*
make/cygwin.mk:TEEM_ENDIAN = 1234
make/darwin.mk:TEEM_ENDIAN = 4321
make/irix6.mk:TEEM_ENDIAN = 4321
make/linux.mk:TEEM_ENDIAN = 1234
make/solaris.mk:TEEM_ENDIAN = 4321
*/
#define TEEM_ENDIAN 1234

#ifdef WIN32
typedef signed __int64 airLLong;
typedef unsigned __int64 airULLong;
#else
typedef signed long long airLLong;
typedef unsigned long long airULLong;
#endif

/* ---- begin of: stuff taken from Gordon Kindlman's teem/air lib */
/* 754.c: IEEE-754 related stuff values */
typedef union {
  unsigned int i;
  float f;
} airFloat;
typedef union {
  airULLong i;
  double d;
} airDouble;

/*
** _airFloat, _airDouble
**
** these unions facilitate converting amonst
** i: unsigned integral type
** c: (sign,exp,frac) triples of unsigned integral components
** v: the floating point numbers these bit-patterns represent
*/
typedef union {
  unsigned int i;
  struct {
#if TEEM_ENDIAN == 1234
    unsigned int frac : 23;
    unsigned int exp : 8;
    unsigned int sign : 1;
#else
    unsigned int sign : 1;
    unsigned int exp : 8;
    unsigned int frac : 23;
#endif
  } c;
  float v;
} _airFloat;

/*
** The hex numbers in braces are examples of C's "initial member of a union"
** aggregate initialization.  We'd be totally out of luck without this.
*/

#if TEEM_QNANHIBIT == 1
static const int airMyQNaNHiBit = 1;
static const airFloat airFloatNaN  = {0x7fffffff};
static const airFloat airFloatQNaN = {0x7fffffff};
static const airFloat airFloatSNaN = {0x7fbfffff};
#else
static const int airMyQNaNHiBit = 0;
static const airFloat airFloatNaN  = {0x7fbfffff};
static const airFloat airFloatQNaN = {0x7fbfffff};
static const airFloat airFloatSNaN = {0x7fffffff};
#endif

#define AIR_NAN (airFloatNaN.f)   /* same as airFloatQNaN.f */
#define AIR_QNAN (airFloatQNaN.f)

/*
******** airIsNaN()
**
** returns 1 if input is either kind of NaN, 0 otherwise.  It is okay
** to only have a a float version of this function, as opposed to
** having one for float and one for double, because Section 6.2 of the
** 754 spec tells us that that NaN is to be preserved across precision
** changes.
*/
inline int
airIsNaN(float g) {
  _airFloat f;

  f.v = g;
  return (255 == f.c.exp && f.c.frac);
}

/* ---- end of: stuff taken from Gordon Kindlman's teem/air lib */

#endif
