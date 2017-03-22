#ifndef _DMATRIX_H_
#define _DMATRIX_H_

#include <iostream>
#include <vector>
#include <math.h>
#include "common.h"
#include <Eigen/Core>

/** Small matrix class.
    First index is colum number. Memory arrangement is row-wise. */
template<class T>
class DMatrix {
public:
    typedef typename std::vector<T>::const_iterator     const_iterator;
    typedef typename std::vector<T>::iterator           iterator;
    typedef T*                                          TPtr;
    typedef const T*                                    CTPtr;
    typedef DMatrix<T>                                  MT;
    typedef typename Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> EMT;

    DMatrix(dword _sx=0, dword _sy=0, const T& inival = T())
        : sx(_sx), sy(_sy), values(_sx*_sy, inival) {}
    DMatrix(dword _sx, dword _sy, const T* data)
        : sx(_sx), sy(_sy), values(_sx*_sy) {
        operator=(data);
    }
    ~DMatrix() {}

    MT& resize(dword _sx, dword _sy, const T& inival = T()) {
        sx = _sx; sy = _sy;
        values.resize(sx*sy, inival);
        return *this;
    }
    MT& reshape(dword _sx, dword _sy) {
        assert(sx*sy == _sx*_sy);
        sx = _sx; sy = _sy;
        return *this;
    }
    void clear() { values.clear(); sx = sy = 0; }

    dword sizeX() const { return sx; }
    dword sizeY() const { return sy; }
    dword size() const { return values.size(); }
    dword index(dword x, dword y) const { return y*sx+x; }
    T& at(dword x, dword y) { return values[index(x,y)]; }
    const T& at(dword x, dword y) const { return values[index(x,y)]; }
    std::vector<T>& getData() { return values; }
    const std::vector<T>& getData() const { return values; }
    bool empty() const { return values.empty(); }

    MT& operator=(const T& rhs) {
        for(iterator vi = values.begin(); vi != values.end(); vi++)
            *vi += rhs;
        return *this;
    }

    MT& operator=(const T* rhs) {
        for(iterator vi = values.begin(); vi != values.end(); vi++, rhs++)
            *vi += *rhs;
        return *this;
    }

    MT& operator+=(const MT& rhs) {
        const_iterator ri = rhs.values.begin();
        for(iterator vi = values.begin(); vi != values.end(); vi++, ri++)
            *vi += *ri;
        return *this;
    }
    MT& operator-=(const MT& rhs) {
        const_iterator ri = rhs.values.begin();
        for(iterator vi = values.begin(); vi != values.end(); vi++, ri++)
            *vi -= *ri;
        return *this;
    }
    MT& operator*=(const MT& rhs) {
        const_iterator ri = rhs.values.begin();
        for(iterator vi = values.begin(); vi != values.end(); vi++, ri++)
            *vi *= *ri;
        return *this;
    }
    MT& operator/=(const MT& rhs) {
        const_iterator ri = rhs.values.begin();
        for(iterator vi = values.begin(); vi != values.end(); vi++, ri++)
            *vi /= *ri;
        return *this;
    }

    MT& operator+=(const T& rhs) {
        for(iterator vi = values.begin(); vi != values.end(); vi++)
            *vi += rhs;
        return *this;
    }
    MT& operator-=(const T& rhs) {
        for(iterator vi = values.begin(); vi != values.end(); vi++)
            *vi -= rhs;
        return *this;
    }
    MT& operator*=(const T& rhs) {
        for(iterator vi = values.begin(); vi != values.end(); vi++)
            *vi *= rhs;
        return *this;
    }
    MT& operator/=(const T& rhs) {
        for(iterator vi = values.begin(); vi != values.end(); vi++)
            *vi /= rhs;
        return *this;
    }

    MT& max(const MT& rhs) {
        const_iterator ri = rhs.values.begin();
        for(iterator vi = values.begin(); vi != values.end(); vi++, ri++)
            if(*vi < *ri) *vi = *ri;
        return *this;
    }
    MT& max(const T& rhs) {
        for(iterator vi = values.begin(); vi != values.end(); vi++)
            if(*vi < rhs) *vi = rhs;
        return *this;
    }
    T max() const {
        assert(sx>0 && sy>0);
        //T val = std::numeric_limits<T>::min();
        T val(values[0]);
        for(const_iterator vi = values.begin(); vi != values.end(); vi++)
            if(*vi > val) val = *vi;
        return val;
    }
    MT& min(const MT& rhs) {
        const_iterator ri = rhs.values.begin();
        for(iterator vi = values.begin(); vi != values.end(); vi++, ri++)
            if(*vi > *ri) *vi = *ri;
        return *this;
    }
    MT& min(const T& rhs) {
        for(iterator vi = values.begin(); vi != values.end(); vi++)
            if(*vi > rhs) *vi = rhs;
        return *this;
    }
    T min() const {
        assert(sx>0 && sy>0);
        //T val = std::numeric_limits<T>::max();
        T val(values[0]);
        for(const_iterator vi = values.begin(); vi != values.end(); vi++)
            if(*vi < val) val = *vi;
        return val;
    }

    MT mulRight(const MT& rhs) const {
        assert(rhs.sy == sx);
        MT res(rhs.sx, sy, T(0));
        for(dword rx=0; rx<rhs.sx; rx++)
            for( dword ly=0; ly<sy; ly++)
                for(dword ry=0; ry<rhs.sy; ry++)
                    res.at(rx, ly) += at(ry, ly)*rhs.at(rx, ry);
        return res;
    }

    MT mulLeft(const MT& rhs) const {
        return rhs.mulRight(*this);
    }

    MT& setCol(dword col, MT& vec) {
        assert(sy == vec.getData().size());
        iterator vi = values.begin()+col;
        for(const_iterator ri = vec.getData().begin();
            ri != vec.getData().end(); ri++, vi+=sx)
            *vi = *ri;
        return *this;
    }
    MT& setCol(dword col, const T& val=1) {
        iterator vi = values.begin()+col;
        for(dword i=0; i<sy; i++, vi+=sx)
            *vi = val;
        return *this;
    }
    MT& setRow(dword row, MT& vec) {
        assert(sx == vec.getData().size());
        iterator vi = values.begin()+(row*sx);
        for(const_iterator ri = vec.getData().begin();
            ri != vec.getData().end(); ri++, vi++)
            *vi = *ri;
        return *this;
    }
    MT& setRange(dword ox, dword oy, const MT& mat) {
        assert(ox<sx && oy<sy);
        dword width = mat.sx <= sx-ox ? mat.sx : sx-ox;
        dword height = mat.sy <= sy-oy ? mat.sy : sy-oy;
        for(dword y = 0; y<height; y++) {
            iterator dst = values.begin()+index(ox,oy+y);
            const_iterator src = mat.values.begin() + mat.index(0,y);
            for(dword x=0; x<width; x++, src++, dst++)
                *dst = *src;
        }
        return *this;
    }
    MT& getRange(dword ox, dword oy, MT& mat) const {
        assert(ox<sx && oy<sy);
        dword width = mat.sx <= sx-ox ? mat.sx : sx-ox;
        dword height = mat.sy <= sy-oy ? mat.sy : sy-oy;
        for(dword y = 0; y<height; y++) {
            const_iterator src = values.begin()+index(ox,oy+y);
            iterator dst = mat.values.begin() + mat.index(0,y);
            for(dword x=0; x<width; x++, src++, dst++)
                *dst = *src;
        }
        return mat;
    }
    MT& setRow(dword row, const T& val=1) {
        iterator vi = values.begin()+(row*sx);
        for(dword i=0; i<sx; i++, vi++)
            *vi = val;
        return *this;
    }
    MT& setDiag(MT& vec, int offset=0) {
        int count = offset>=0 ? sx-offset : sy+offset;
        assert( count <= vec.getData().size() );
        iterator vi = values.begin() + ( offset>=0 ? offset : -offset*sx);
        for(const_iterator ri = vec.getData().begin(); count>0;
            count--, ri++, vi+=sx+1)
            *vi = *ri;
        return *this;
    }
    MT& setDiag(const T& val=1, int offset=0) {
        int count = offset>=0 ? sx-offset : sy+offset;
        iterator vi = values.begin() + ( offset>=0 ? offset : -offset*sx);
        for(; count>0; count--, vi+=(sx+1))
            *vi = val;
        return *this;
    }
    MT  getDiag(int offset=0) {
        int count = offset>=0 ? sx-offset : sy+offset;
        MT res(1,count);
        const_iterator vi = values.begin()+( offset>=0 ? offset : -offset*sx);
        for(iterator ri = res.getData().begin(); count>0;
            count--, ri++, vi+=sx+1)
            *ri = *vi;
        return res;
    }

    MT& setUpper(const T& val = 1, int offset=0) {
        int i=0,j=0;
        for(iterator mi = values.begin();
            mi != values.end(); mi++, i++) {
            if(i==sx) { j++; i=0; }
            if(i-j>=offset) *mi = val;
        }
        return *this;
    }
    MT& setLower(const T& val = 1, int offset=0) {
        int i=0,j=0;
        for(iterator mi = values.begin();
            mi != values.end(); mi++, i++) {
            if(i==(int)sx) { j++; i=0; }
            if(i-j<=offset) *mi = val;
        }
        return *this;
    }

    MT& transpose() {
        if(sx*sy>1) {
            if(sx==sy)
                for(dword j=0; j<sy-1; j++)
                    for(dword i = j+1; i<sx; i++)
                    {
                        T& a = at(i,j);
                        T& b = at(j,i);
                        T dummy = a; a = b; b = dummy;
                    }
            else if(sx == 1 || sy == 1) reshape(sy, sx);
            else {
                MT tmp(*this);
                reshape(sy,sx);
                for(dword j=0; j<sy; j++)
                    for(dword i = 0; i<sx; i++)
                    {
                        at(i,j) = tmp.at(j,i);
                    }
            }
        }
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const DMatrix<T>& rhs) {
        os << rhs.sx << " " << rhs.sy;
        dword cc=0;
        for(const_iterator vi = rhs.values.begin();
            vi != rhs.values.end(); vi++, cc--) {
            if(!cc) { cc = rhs.sx; os << std::endl; }
            else os << " ";
            os << *vi;
        }
        os << std::endl;
        return os;
    }
    friend std::istream& operator>>(std::istream& is, DMatrix<T>& rhs) {
        is >> rhs.sx; is >> rhs.sy;
        rhs.resize(rhs.sx, rhs.sy);
        for(iterator vi = rhs.values.begin();
            vi != rhs.values.end(); vi++)
            is >> *vi;
        return is;
    }

protected:
    dword       sx,sy;
    std::vector<T>   values;
};

typedef DMatrix<int> DMatrixi;
typedef DMatrix<float> DMatrixf;
typedef DMatrix<double> DMatrixd;

#endif
