#ifndef _DMATRIXUTIL_H_
#define _DMATRIXUTIL_H_

namespace dmutil {
    template<class T>
        DMatrix<T> makeDiag(const DMatrix<T>& diag) {
        dword sxy = diag.size();
        DMatrix<T> mat(sxy, sxy, T(0));
        typename DMatrix<T>::const_iterator di = diag.getData().begin();
        for(dword i = 0; i < sxy; i++, di++) mat.at(i,i) = *di;
        return mat;
    }

    template<class T>
        DMatrix<T> makeIdentity(dword sxy, const T& diag = 1) {
        return DMatrix<T>(sxy, sxy, T(0)).setDiag(diag);
    }

    template<class T>
        DMatrix<T> makeUpper(dword sxy, bool filldiag=true, const T& val = 1) {
        return DMatrix<T>(sxy, sxy).setUpper(1, val);
    }

    template<class T>
        DMatrix<T> makeLower(dword sxy, bool filldiag=true, const T& val = 1) {
        return DMatrix<T>(sxy, sxy).setLower(val);
    }

    template<class T>
        DMatrix<T> makeRotMat2D(const float& angle) {
        DMatrix<T> mat(2,2);
        mat.at(0,0) = (T)cos(angle); mat.at(1,0) = (T)-sin(angle);
        mat.at(0,1) = (T)sin(angle); mat.at(1,1) = (T)cos(angle);
        return mat;
    }

    template<class T>
        DMatrix<T> horizCat(const DMatrix<T>& lm, const DMatrix<T>& rm)
        {
            assert(lm.sizeY() == rm.sizeY());
            DMatrix<T> res(lm.sizeX()+rm.sizeX(), lm.sizeY());
            res.setRange(0,0, lm);
            res.setRange(lm.sizeX(),0, rm);
            return res;
        }

    template<class T>
        DMatrix<T> vertCat(const DMatrix<T>& um, const DMatrix<T>& lm)
        {
            assert(um.sizeX() == lm.sizeX());
            DMatrix<T> res(um.sizeX(), lm.sizeY() + um.sizeY());
            res.setRange(0,0, um);
            res.setRange(0,um.sizeY(), lm);
            return res;
        }

    template<class T>
        DMatrix<T> sum(const DMatrix<T>& mat)
        {
            DMatrix<T> res(mat.sizeX(), 1);
            typename DMatrix<T>::const_iterator mi = mat.getData().begin();
            for(dword y=0; y<mat.sizeY(); y++) {
                typename DMatrix<T>::iterator ri = res.getData().begin();
                for(dword x=0; x<mat.sizeX(); x++, ri++, mi++)
                    *ri += *mi;
            }
            return res;
        }

    template<class T>
        DMatrix<T>& sqr(DMatrix<T>& mat)
        {
            for(typename DMatrix<T>::iterator mi = mat.getData().begin();
                mi != mat.getData().end(); mi++)
                *mi = *mi**mi;
            return mat;
        }

    template<class T>
        DMatrix<T>& sqrt(DMatrix<T>& mat)
        {
            for(typename DMatrix<T>::iterator mi = mat.getData().begin();
                mi != mat.getData().end(); mi++)
                *mi = (T)sqrtf((double)*mi);
            return mat;
        }

    template<class T>
        DMatrix<T> avg(const DMatrix<T>& mat)
        {
            DMatrix<T> res(sum(mat));
            return res/=T(mat.sizeY());
        }

    template<class T>
        DMatrix<T> var(const DMatrix<T>& mat)
        {
            DMatrix<T> e(avg(mat));
            DMatrix<T> x2(mat); sqr(x2);
            DMatrix<T> e2(avg(x2));
            return e2-=sqr(e);
        }

    template<class T>
        DMatrix<T> stdev(const DMatrix<T>& mat)
        {
            DMatrix<T> sd(var(mat));
            return dmutil::sqrt(sd);
        }

    template<class T>
        DMatrix<T> expand(const DMatrix<T>& mat, dword mx, dword my)
        {
            DMatrix<T> res(mat.sizeX()*mx, mat.sizeY()*my);
            for(dword y=0; y<my; y++)
                for(dword x=0; x<mx; x++)
                    res.setRange(mat.sizeX()*x, mat.sizeY()*y, mat);
            return res;
        }

    template<class T>
        DMatrix<T>& threshold(DMatrix<T>& mat, const T& th, const T& repl=T(0))
        {
            for(typename DMatrix<T>::iterator mi = mat.getData().begin();
                mi != mat.getData().end(); mi++)
                if(*mi<th) *mi = repl;
            return mat;
        }

    template<class T>
        DMatrix<T>& abs(DMatrix<T>& mat)
        {
            for(typename DMatrix<T>::iterator mi = mat.getData().begin();
                mi != mat.getData().end(); mi++)
                if(*mi<T(0)) *mi = -*mi;
            return mat;
        }

}; // namespace dmutil

#endif
