#ifndef _PROPVEC_H_
#define _PROPVEC_H_
#include "VVector.h"
#include "Point.h"

#define _PROPTF_NORMALIZE_SCALE_

typedef VVector<float,4> PropVec;
typedef PropVec PropTF;

enum PropVecOffsets {PVEC_POS=0,PVEC_POSX=0, PVEC_POSY, PVEC_SCALE, PVEC_DIR};

inline Point2D getPropPos(const PropVec &prop)
{ return Point2D(&prop[PVEC_POS]); }

inline PropVec& setPropPos(PropVec &prop, const Point2D &p)
{   prop[PVEC_POSX]=p.x; prop[PVEC_POSY]=p.y; return prop; }

inline float getPropScale(const PropVec &prop)
{ return prop[PVEC_SCALE]; }

inline PropVec& setPropScale(PropVec &prop, float pscale) 
{ prop[PVEC_SCALE] = pscale; return prop; }

inline float getPropDir(const PropVec &prop)
{ return prop[PVEC_DIR]; }

inline PropVec& setPropDir(PropVec &prop, float dir)
{ prop[PVEC_DIR] = dir; return prop; }

inline Point2D getPropSDir(const PropVec& prop) 
{ return Point2D(getPropScale(prop),0).rotate(getPropDir(prop)); }

inline PropVec& setPropSDir(PropVec& prop, const Point2D& sdir) 
{ return setPropScale(setPropDir(prop, sdir.angle()), sdir.norm()); }

inline PropTF getPropTF(const PropVec& from, const PropVec& to)
{
    PropTF tf;
#ifdef _PROPTF_NORMALIZE_SCALE_
    setPropPos(tf,(getPropPos(to)-getPropPos(from)).rotate(-getPropDir(from))
               /getPropScale(from));
#else
    setPropPos(tf,(getPropPos(to)-getPropPos(from)).rotate(-getPropDir(from)));
#endif
    setPropScale(tf,getPropScale(to)/getPropScale(from));
    setPropDir(tf,getPropDir(to)-getPropDir(from));
    return tf;
}

inline PropVec& fwdPropTF(PropVec& from, const PropTF& tf)
{
#ifdef _PROPTF_NORMALIZE_SCALE_
    setPropPos(from, getPropPos(from)+(getPropPos(tf).rotate(getPropDir(from))
                                       *getPropScale(from)));
#else
    setPropPos(from, getPropPos(from)+getPropPos(tf).rotate(getPropDir(from)));
#endif
    setPropDir(from, getPropDir(from)+getPropDir(tf));
    setPropScale(from, getPropScale(from)*getPropScale(tf));
//     from[0] += tf[0]; from[1] += tf[1];
//     from[2] *= tf[2]; from[3] += tf[3];
    return from;
}

inline PropVec& invPropTF(PropVec& to, const PropTF& tf)
{
    setPropScale(to, getPropScale(to)/getPropScale(tf));
    setPropDir(to, getPropDir(to)-getPropDir(tf));
#ifdef _PROPTF_NORMALIZE_SCALE_
    setPropPos(to, getPropPos(to)-(getPropPos(tf).rotate(getPropDir(to))
                                   *getPropScale(to)));
#else
    setPropPos(to, getPropPos(to)-getPropPos(tf).rotate(getPropDir(to)));
#endif
//     to[0] -= tf[0]; to[1] -= tf[1];
//     to[2] /= tf[2]; to[3] -= tf[3];
    return to;
}

inline PropTF getIdentityPropTF() {
    PropVec ie(0.f); setPropScale(ie, 1); return ie;
}

inline PropVec& scalePropVec(PropVec& prop, float scale) {
    setPropPos(prop, getPropPos(prop)*scale);
    setPropScale(prop, getPropScale(prop)*scale);
    return prop;
}

#endif
