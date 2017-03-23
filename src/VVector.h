#ifndef _VVECTOR_H_
#define _VVECTOR_H_

template<typename T>
struct Traits{
    T static zero() { return 0; };
    T static one() { return 1; };
};
template<> struct Traits<float>;
template<> struct Traits<double>;

/** a small numerical vector class */
template<class T, unsigned int D>
    class VVector
{
public:
    typedef VVector<T, D> VT;
    typedef T*            TPtr;
    typedef const T*      CTPtr;
    typedef Traits<T>     TTrait;

    VVector() {};
    VVector(const T &sval) {
        operator=(sval);
    }
    VVector(CTPtr pval) {
        operator=(pval);
    }
    VVector(const VT &rhs) {
        operator=(rhs);
    }
    ~VVector() {};

    static VT zero() { return VT(TTrait::zero()); }
    static VT one() { return VT(TTrait::one()); }

    TPtr begin() { return val; }
    CTPtr begin() const { return val; }
    TPtr end() { return &val[D]; }
    CTPtr end() const { return &val[D]; }
    static unsigned int size() { return D; }
    const T& operator[](unsigned int i) const { return val[i]; }
    T& operator[](unsigned int i) { return val[i]; }

    bool operator==(const VT& rhs) const {
        CTPtr sv=rhs.begin();
        for(CTPtr pv=begin(); pv!=end(); pv++, sv++)
            if(*pv != *sv) return false;
        return true;
    }
    bool operator<(const VT& rhs) const {
        CTPtr sv=rhs.begin();
        for(CTPtr pv=begin(); pv!=end(); pv++, sv++)
            if(*pv >= *sv) return false;
        return true;
    }
    bool operator>(const VT& rhs) const {
        CTPtr sv=rhs.begin();
        for(CTPtr pv=begin(); pv!=end(); pv++, sv++)
            if(*pv <= *sv) return false;
        return true;
    }
    bool operator<=(const VT& rhs) const {
        CTPtr sv=rhs.begin();
        for(CTPtr pv=begin(); pv!=end(); pv++, sv++)
            if(*pv > *sv) return false;
        return true;
    }
    bool operator>=(const VT& rhs) const {
        CTPtr sv=rhs.begin();
        for(CTPtr pv=begin(); pv!=end(); pv++, sv++)
            if(*pv < *sv) return false;
        return true;
    }

    VT& operator=(const T &rhs) {
        for(TPtr pv=begin(); pv!=end(); pv++)
            *pv = rhs;
        return *this;
    }
    VT& operator=(CTPtr rhs) {
        for(TPtr pv=begin(); pv!=end(); pv++, rhs++)
            *pv = *rhs;
        return *this;
    }
    VT& operator=(const VT &rhs) {
        CTPtr sv=rhs.begin();
        for(TPtr pv=begin(); pv!=end(); pv++, sv++)
            *pv = *sv;
        return *this;
    }

    VT& operator+=(const VT &rhs) {
        CTPtr sv=rhs.begin();
        for(TPtr pv=begin(); pv!=end(); pv++, sv++)
            *pv += *sv;
        return *this;
    }
    VT& operator-=(const VT &rhs) {
        CTPtr sv=rhs.begin();
        for(TPtr pv=begin(); pv!=end(); pv++, sv++)
            *pv -= *sv;
        return *this;
    }
    VT& operator*=(const VT &rhs) {
        CTPtr sv=rhs.begin();
        for(TPtr pv=begin(); pv!=end(); pv++, sv++)
            *pv *= *sv;
        return *this;
    }
    VT& operator/=(const VT &rhs) {
        CTPtr sv=rhs.begin();
        for(TPtr pv=begin(); pv!=end(); pv++, sv++)
            *pv /= *sv;
        return *this;
    }
    VT& operator+=(const T &rhs) {
        for(TPtr pv=begin(); pv!=end(); pv++)
            *pv += rhs;
        return *this;
    }
    VT& operator-=(const T &rhs) {
        for(TPtr pv=begin(); pv!=end(); pv++)
            *pv -= rhs;
        return *this;
    }
    VT& operator*=(const T &rhs) {
        for(TPtr pv=begin(); pv!=end(); pv++)
            *pv *= rhs;
        return *this;
    }
    VT& operator/=(const T &rhs) {
        for(TPtr pv=begin(); pv!=end(); pv++)
            *pv /= rhs;
        return *this;
    }

    VT operator+(const VT &rhs) const {
        VT r(*this);
        return r+=rhs;
    }
    VT operator-(const VT &rhs) const {
        VT r(*this);
        return r-=rhs;
    }
    VT operator*(const VT &rhs) const {
        VT r(*this);
        return r*=rhs;
    }
    VT operator/(const VT &rhs) const {
        VT r(*this);
        return r/=rhs;
    }
    VT operator+(const T &rhs) const {
        return (VT(*this) += rhs);
    }
    VT operator-(const T &rhs) const {
        return (VT(*this) -= rhs);
    }
    VT operator*(const T &rhs) const {
        return (VT(*this) *= rhs);
    }
    VT operator/(const T &rhs) const {
        return (VT(*this) /= rhs);
    }

    VT& times(const VT& rhs) {
        CTPtr sv=rhs.begin();
        for(TPtr pv=begin(); pv!=end(); pv++, sv++)
            *pv *= *sv;
        return *this;
    }

    T dot(const VT &rhs) const {
        T r= TTrait::zero();
        for(CTPtr pv=begin(), *sv=rhs.begin(); pv!=end(); pv++, sv++)
            r += *pv * *sv;
        return r;
    }

    VVector<T, 3> cross(const VVector<T, 3>& rhs) const {
        VVector<T, 3> r;
        r.val[0]= val[1]*rhs.val[2] - val[2]*rhs.val[1];
        r.val[1]= val[2]*rhs.val[0] - val[0]*rhs.val[2];
        r.val[2]= r.val[2] = val[0]*rhs.val[1] - val[1]*rhs.val[0];
        return r;
    }
    T sum() const { //norm1
        T n= TTrait::zero();
        for(CTPtr pv=begin(); pv!=end(); pv++)
            n += *pv;
        return n;
    }
    T prod() const {
        T n= TTrait::one();
        for(CTPtr pv=begin(); pv!=end(); pv++)
            n *= *pv;
        return n;
    }
    T norm2() const {
        T n= TTrait::zero();
        for(CTPtr pv=begin(); pv!=end(); pv++)
            n += *pv * *pv;
        return n;
    }
    T norm() const { return sqrt(norm2()); }
    T normalize() {
        T n = norm();
        operator*=(TTrait::one()/n);
        return n;
    }
    T avg() const { return sum()/D; }
    T var() const { return (*this-avg()).norm2(); }
    T stdev() const { return sqrt(var()); }
    VT& sqrtEach() {
        for(TPtr pv=begin(); pv!=end(); pv++)
            *pv = sqrt(*pv);
        return *this;
    }
    T maxVal() const {
        T mv=val[0];
        for(CTPtr pv=begin(); pv!=end(); pv++)
            if(mv < *pv) mv = *pv;
        return mv;
    }
    T minVal() const {
        T mv=val[0];
        for(CTPtr pv=begin(); pv!=end(); pv++)
            if(mv > *pv) mv = *pv;
        return mv;
    }
    unsigned int majorComponent() const {
        T mv=TTrait::zero();
        unsigned int mc=0,cc=0;
        for(CTPtr pv=begin(); pv!=end(); pv++, cc++) {
            T apv = abs(*pv);
            if(mv < apv) { mv = apv; mc = cc; }
        }
        return mc;
    }
    VT& clampLB(const VT &lb) {
        CTPtr sv=lb.begin();
        for(TPtr pv=begin(); pv!=end(); pv++, sv++)
            if(*pv < *sv) *pv = *sv;
        return *this;
    }
    VT& clampUB(const VT &ub) {
        CTPtr sv=ub.begin();
        for(TPtr pv=begin(); pv!=end(); pv++, sv++)
            if(*pv > *sv) *pv = *sv;
        return *this;
    }
    VT& clamp(const VT &lb, const VT &ub) {
        CTPtr sv=lb.begin();
        CTPtr uv=ub.begin();
        for(TPtr pv=begin(); pv!=end(); pv++, sv++, uv++) {
            if(*pv > *uv) *pv = *uv;
            else if(*pv < *sv) *pv = *sv;
        }
        return *this;
    }
    float angle(const VT& rhs) const {
        float dv = cross(rhs).norm(); //sin(a)
        float dh = dot(rhs); //cos(a)
#ifndef _THERE_IS_NO_ATAN2_
        return atan2(dv,dh);
#else
        float dvp,dvn;
        if(dv > 0.0) {
            dvp = 1.0;
            dvn = 0.0;
        } else {
            dvp = 0.0;
            dvn = 1.0;
        }
        float dhn = (dh < 0) ? 1.0 : 0.0;
        float psi;
        if (dh == 0.0)
            psi = M_PI_2 * (dvp + 3.0 * dvn);
        else if (dv == 0.0)
            psi = M_PI * dhn;
        else
            psi = atan(dv/dh) + M_PI * dhn * dvp
                + (M_PI + M_PI * (dh > 0.0)) * dvn;
        return psi * (180.0*M_1_PI);
#endif
    }

    friend std::ostream& operator<<(std::ostream& os, const VT& rhs) {
        for(CTPtr pv= rhs.begin(); pv != rhs.end(); pv++) {
            //if(pv != rhs.begin()) os << " ";
            os << *pv << " ";
        }
        return os;
    }
    friend std::istream& operator>>(std::istream &is, VT &rhs) {
        for(TPtr pv=rhs.begin(); pv!=rhs.end(); pv++) {
            is >> *pv;
        }
        return is;
    }

public:
    T val[D];
};

template<> struct Traits<float> {
    float static zero() { return 0.f; };
    float static one() { return 1.f; };
};

template<> struct Traits<double> {
    double static zero() { return 0.; };
    double static one() { return 1.; };
};

#endif // _VVECTOR_H_
