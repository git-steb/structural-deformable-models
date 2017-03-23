#ifndef _CLUSTERER_H_
#define _CLUSTERER_H_
#include <iostream>
#include <math.h>
#include <string>
#include "common.h"

template<int D>
class CElement {
public:
    CElement() : id(0), multiv(0) {}
    CElement(float v) : id(0), multiv(0) {
        operator=(v);
    }
    CElement(const vector<float> &rhs) : id(0), multiv(0) {
        operator=(rhs);
    }
    CElement<D>& operator=(const vector<float> &rhs) {
        for(int i=0; i<D; i++) val[i]=rhs[i];
        return *this;
    }
    CElement<D>& operator=(const CElement &rhs) {
        for(int i=0; i<D; i++) val[i]=rhs.val[i];
        id = rhs.id;
        multiv = rhs.multiv;
        return *this;
    }
    CElement<D>& operator=(float v) {
        for(int i=0; i<D; i++) val[i]=v;
        return *this;
    }
    CElement<D>& operator+=(const CElement<D> &rhs) {
        for(int i=0; i<D; i++) val[i]+=rhs.val[i];
        return *this;
    }
    CElement<D>& operator*=(const CElement<D> &rhs) {
        for(int i=0; i<D; i++) val[i]*=rhs.val[i];
        return *this;
    }
    CElement<D>& operator*=(float v) {
        for(int i=0; i<D; i++) val[i]*=v;
        return *this;
    }
    CElement<D>& operator/=(float v) {
        for(int i=0; i<D; i++) val[i]/=v;
        return *this;
    }
    CElement<D>& randomize(float min=0, float max=1) {
        for(int i=0; i<D; i++) val[i]=frand(max-min)+min;
        return *this;
    }
    float dist2(const CElement<D> &rhs) const {
        float d=0;
        for(int i=0; i<D; i++) {
            register float cd = val[i]-rhs.val[i];
            d += cd*cd;
        }
        return d;
    }
    float dist(const CElement<D> &rhs) const {
        return sqrt(dist2(rhs));
    }

    dword getNMembers() const { return multiv; }
    dword& getNMembers() { return multiv; }
    dword getClassID() const { return multiv; }
    void setClassID(dword cid) { multiv = cid; }
    friend ostream& operator<<(ostream& os, const CElement<D> &rhs) {
        for(int i=0; i<D; i++) {
            if(i) os << " ";
            os << rhs.val[i];
        }
        return os;
    }

public:
    float val[D];
    dword id;
    dword multiv; // class ID or number of members
};

template<int D>
class Clusterer {
public:
    typedef typename vector< CElement<D> >::iterator eiterator;
    typedef typename vector< CElement<D> >::const_iterator const_eiterator;

    Clusterer(dword nclasses=1, float min=0, float max=1 ) {
        initialize(nclasses, min, max);
    };

    void clearAccumulation() {
        dword cid=0;
        for(eiterator cv = m_ElemAcc.begin();
            cv != m_ElemAcc.end(); cv++, cid++)
        {
            *cv = 0;
            cv->id = cid;
            cv->getNMembers() = 0;
        }
    }

    void initialize( dword nclasses, float min=0, float max=1 ) {
        m_Classes.resize(nclasses);
        m_ElemAcc.resize(nclasses);
        dword cid=0;
        for(eiterator cv = m_Classes.begin();
            cv != m_Classes.end(); cv++, cid++)
        {
            cv->id = cid;
            cv->getNMembers() = 0;
            cv->randomize(min,max);
            //cout << *cv << endl;
        }
        clearAccumulation();
    }

    dword classify(CElement<D> &elem) {
        float mind=numeric_limits<float>::max();
        dword minid=0;
        dword cid=0;
        for(eiterator cv = m_Classes.begin();
            cv != m_Classes.end(); cv++, cid++)
        {
            float d = elem.dist2(*cv);
            if(d<mind) {
                mind = d;
                minid = cid;
            }
        }
        CElement<D> &ae = m_ElemAcc[minid];
        ae += elem;
        ae.getNMembers()++;
        elem.setClassID(minid);
        return minid;
    }

    dword classifyAll() {
        dword reclass = 0;
        clearAccumulation();
        for(eiterator e=m_Elements.begin();
            e != m_Elements.end(); e++)
        {
            dword oldclass = e->getClassID();
            if(oldclass != classify(*e))
                reclass++;
        }
        for(eiterator ci=m_Classes.begin(), ca=m_ElemAcc.begin();
            ci!=m_Classes.end(); ci++, ca++)
        {
            if(ca->getNMembers()) {
                *ca *= 1.0f/ca->getNMembers();
                *ci = *ca;
            } else ci->getNMembers() = 0;
            //cout << *ci << " members: " << ci->getNMembers() << endl;
        }
        cout << reclass << " re-classifications" << endl; //DEBUG
        return reclass;
    }

    void iterateClassification( dword maxit=10) {
        int counter=0; //DEBUG
        while(maxit>0) {
            cout << "iteration " << (++counter) << endl; //DEBUG
            if(classifyAll() == 0) break;
            maxit--;
        }
        int cu=0;
        for(eiterator ci=m_Classes.begin(); ci!=m_Classes.end(); ci++)
            if(ci->getNMembers()) cu++;
        cout << "number of classes used: " << cu << endl;
    }

public:
    vector< CElement<D> >       m_Elements;
    vector< CElement<D> >       m_Classes;
    vector< CElement<D> >       m_ElemAcc;
};

#endif
