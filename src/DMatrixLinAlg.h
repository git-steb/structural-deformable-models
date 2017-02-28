#ifndef _DMATRIXLINALG_H_
#define _DMATRIXLINALG_H_
#include "DMatrix.h"
#include "DMatrixUtil.h"
#include <Eigen/SVD>
#include <functional>
#include <algorithm>
#include <vector>

#define DMLA_TAU	0.0000001f

namespace linalg {
  typedef Eigen::MatrixXf Matrix;
}

namespace dmutil {
    
    template<class T>
    linalg::Matrix convert(const DMatrix<T>& dmat)
    {
        Eigen::MatrixXf mat((int)dmat.sizeY(), (int)dmat.sizeX());
        linalg::Matrix eth(mat);
        //typename DMatrix<T>::const_iterator dat = dmat.getData().begin();
	// TODO: translate this code from LinAlg to Eigen
        //for(register int i=eth.q_row_lwb(); i<=eth.q_row_upb(); i++)
        //    for(register int j=eth.q_col_lwb(); j<=eth.q_col_upb(); j++)
        //        eth(i,j) = dmat.at(j-1,i-1);
        return mat;
    }
    
    template<class T>
    DMatrix<T> convert(const linalg::Matrix& mat)
    {
        linalg::Matrix cmat(mat);
        linalg::Matrix eth(cmat);
        DMatrix<T> dmat; // (eth.q_col_upb()-eth.q_col_lwb()+1, // TODO
                         // eth.q_row_upb()-eth.q_row_lwb()+1);
        //typename DMatrix<T>::iterator dat = dmat.getData().begin();
        //for(register int i=eth.q_row_lwb(); i<=eth.q_row_upb(); i++)
        //    for(register int j=eth.q_col_lwb(); j<=eth.q_col_upb(); j++)
        //        dmat.at(j-1,i-1) = eth(i,j);
        return dmat;
    }

    template<class T>
    DMatrix<T>& invert(DMatrix<T>& dmat) 
    {
        bool m_Verbose = false;
        try {
            //if(!m_Verbose) freopen("/dev/null","a+",stderr);
            linalg::Matrix mat = convert<T>(dmat);
            //dmat = convert<T>(linalg::inverse(mat)); // TODO
        } catch(void*)
        {
            //if(!m_Verbose) freopen("/dev/stdout","a+",stderr);
        }
        //if(!m_Verbose) freopen("/dev/stdout","a+",stderr);
        return dmat;
    }
    
    template<class T>
    DMatrix<T> inverse(const DMatrix<T>& dmat) 
    {
        DMatrix<T> imat(dmat);
        return invert(imat);
    }
    
    template<class T>
    bool SVD(const DMatrix<T>& dmat, 
             DMatrix<T>& mU, DMatrix<T>& mS, DMatrix<T>& mV) 
    {
        using namespace Eigen;
        bool m_Verbose = false;
        try {
            //if(!m_Verbose) freopen("/dev/null","a+",stderr);
            // zeropad matrix to make enough rows 
            // (rank doesn't matter for the algorithm)
            int d = dmat.sizeX() - dmat.sizeY();
            if(d<0) d=0;
            linalg::Matrix mat = 
                convert<T>(DMatrix<T>(dmat.sizeX(), dmat.sizeY()+d,0.0f)
                           .setRange(0,0,dmat));
	    JacobiSVD<MatrixXd> svd( mat, ComputeThinU | ComputeThinV);	    
            //linalg::SVD svd(mat); // TODO
            //if(m_Verbose) cout << "condition number of matrix A " << 
            //                  svd.q_cond_number() << endl;
            //if(m_Verbose) cout << svd.q_U().q_nrows() << " rows" << endl;
            
            DMatrix<T> U = dmutil::convert<T>(svd.matrixU());
            DMatrix<T> V = dmutil::convert<T>(svd.matrixV());
            VectorXf sig(svd.singularValues());
            DMatrix<T> S;
            S.resize(V.sizeY(),U.sizeX(),T(0));
            dword sxy = min(S.sizeX(),S.sizeY());
	    std::vector< std::pair<T,dword> > sdiag(sxy);
            for(dword i=0; i<sxy; i++) {
                S.at(i,i) = sig(i+1);
                sdiag[i].first = sig(i+1);
                sdiag[i].second = i;
            }
            mS = S; mU = U; mV = V;
            //sort by diagonal of S
            sort(sdiag.begin(), sdiag.end(), std::greater< std::pair<float,T> >());
            DMatrix<T> colu(1,U.sizeY());
            DMatrix<T> colv(1,V.sizeY());
            for(dword i=0; i<sxy; i++) {
                dword permi = sdiag[i].second;
                if(permi != i) {
                    U.getRange(permi,0,colu);
                    mU.setCol(i, colu);
                    V.getRange(permi,0,colv);
                    mV.setCol(i, colv);
                    mS.at(i,i) = S.at(permi,permi);
                }
            }
        } catch(void*)
        {
            //if(!m_Verbose) freopen("/dev/stdout","a+",stderr);
            return false;
        }
        //if(!m_Verbose) freopen("/dev/stdout","a+",stderr);
        return true;
    }
    
    template<class T> 
    DMatrix<T> pseudoInv(const DMatrix<T>& dmat)
    {
        DMatrix<T> U,S,V;
        if(dmutil::SVD(dmat, U,S,V)) {
            //create an inverted S and skip the nearly-zero elements 
            //on the diagonal
            dword sxy = std::min(S.sizeX(),S.sizeY());
            for(dword i=0; i<sxy; i++)
                if(S.at(i,i)>T(DMLA_TAU)) S.at(i,i) = T(1)/S.at(i,i);
                else S.at(i,i) = 0;
             S.transpose();
             U.transpose();
            DMatrix<T> P = V.mulRight(S.mulRight(U));  // actually V * S(+)*Ut
            return P;
        }
        return dmat;
    }
};


#undef DMLA_TAU

#endif
