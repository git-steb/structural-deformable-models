/* -*- C++ -*- */
// Image.h: Schnittstelle für die Klasse Image.
//
//////////////////////////////////////////////////////////////////////
#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "common.h"

template <class T>
class Image : public std::vector<T>
{
public:
    typedef typename std::vector<T>::iterator iterator;
    typedef typename std::vector<T>::const_iterator const_iterator;

    Image();
    Image(int sx, int sy) 
        : std::vector<T>(sx*sy), sizeX(sx), sizeY(sy) {};
    Image(int sx, int sy, const T &value) 
        : std::vector<T>(sx*sy, value), sizeX(sx), sizeY(sy) {};
    Image(const Image<T> & rhs);
    virtual ~Image() {};
    void freeImage();

    void setSize(int nx, int ny);
    void setSize(int nx, int ny, const T &value);
    
    template<class S>
    bool sameSize(const Image<S>& rhs) 
        { return rhs.getSizeX()==sizeX && rhs.getSizeY()==sizeY; }


    const T *getData() const {return &(*begin());}
    T *getData() {return &(*begin());}

    int getSizeX() const { return sizeX;}
    int getSizeY() const { return sizeY;}
    int getSize() const { return size();}

    Image<T>& operator=(const Image<T>& rhs);
    Image<T>& operator=(const T& rhs);
    Image<T>& copy(const Image<T>& rhs);
    Image<T>& copy(const T* rhs);

    int getIndex(int x, int y) const { return sizeX*y+x; }
    int getBoundedIndex(int x, int y) const { 
        if((dword)x < (dword)sizeX && (dword)y < (dword)sizeY)
            return sizeX*y+x;
        else
            return -1;
    }
    
    const T getPixel(int x, int y) const {
        return at(getIndex(x,y));
    }
    void setPixel(int x, int y, const T &value) {
        at(getIndex(x,y)) = value;
    }
    //! calculate maximum
    const T findMax() const;
    //! calculate minimum
    const T findMin() const;
    /** calculate 2-norm of 2D array
        internally using double, so only works on scalar data types
    */
    const T norm() const;
    double sum() const;
    Image<T>& operator*=(const T& v);
    Image<T>& operator/=(const T& v);
    Image<T>& operator+=(const T& v);
    Image<T>& operator-=(const T& v);
    Image<T>& operator*=(const Image<T>& rhs);
    Image<T>& operator/=(const Image<T>& rhs);
    Image<T>& operator+=(const Image<T>& rhs);
    Image<T>& operator-=(const Image<T>& rhs);

    void addLine(int x0, int y0, int x1, int y1, const T& value);

    Image<T>& unsetNAN(double val=0);
    Image<T>& threshold(const T& th, double binarize=0);
    Image<T>& findMaxima(int env=3);

//template template member functions (still) have to be in the class definition
    template <class S>
    Image<T>& convertFrom(const Image<S>& rhs)
	{
            setSize(rhs.getSizeX(),rhs.getSizeY());
            const S* sd = rhs.getData();
            T* dd = getData();
            for(int i=getSize();i>0;i--,dd++,sd++)
                *dd = T(*sd);
            return *this;
	}

    const Image<T> mirror(bool horiz=true, bool vert=true) const
	{
	    Image<T> dimg(getSizeX(),getSizeY());
	    int i,j;
	    for(j=0;j<getSizeY();j++)
		for(i=0;i<getSizeX();i++)
		{
		    int si = horiz ? getSizeX()-i-1 : i;
		    int sj = vert ? getSizeY()-j-1 : j;
		    dimg.setPixel(i,j,getPixel(si,sj));
		}
	    return dimg;
	}

    template <class S>
    const Image<T> convolve(const Image<S>& kernel) const
	{
            Image<T> dst(sizeX,sizeY);
            int ksx = kernel.getSizeX()/2;
            int ksy = kernel.getSizeY()/2;
            int endX = sizeX-ksx;
            int endY = sizeY-ksy;
            //int offset = ksy*sizeX+ksx;
            int winskip = sizeX-kernel.getSizeX();
            T* dstdat = dst.getData() + ksx + (ksy*sizeX);
            const T* srcdat = getData();
            for(int i=ksy;i<endY;i++)
            {
                for(int j=ksx;j<endX;j++,dstdat++,srcdat++)
                {
                    S sum = 0;
                    const S *kdat = kernel.getData();
                    const T *sdat = srcdat;
                    for(int l=0;l<kernel.getSizeY();l++)
                    {
                        for(int k=0;k<kernel.getSizeX();k++, kdat++, sdat++)
                        {
                            sum += *kdat**sdat;
                        }
                        sdat += winskip;
                    }
                    *dstdat = T(sum);
                }
                dstdat+=ksx*2;
                srcdat+=ksx*2;
            }
            return dst;
	}

    Image<T>& interleave(const std::vector< Image<T> >& img, 
                         dword ncomp=0) 
        {
            if(!ncomp) ncomp = img.size();
            const int comp = img.size()>ncomp ? ncomp : img.size();
            const int size = img[0].size();
            setSize(img[0].getSizeX()*ncomp, img[0].getSizeY());
            Image<byte>::const_iterator bi[comp];
            for(int a=0; a<comp; a++) bi[a] = img[a].begin();
            T *cv = getData();
            const int cvinc = ncomp-comp;
            for(int i=0; i<size; i++) {
                for(int a=0; a<comp; a++) {
                    *(cv++) = *bi[a]; 
                    bi[a]++;
                }
                cv += cvinc;
            }
            return *this;
        }

    Image<T> scaleBy(float s) { return scaleBy(s,s); }
    Image<T> scaleBy(float sx, float sy) 
        {
            //can only scale down by integral numbers
            int stepx = (int)round(1/sx); if(stepx<=0) stepx=1;
            int stepy = (int)round(1/sy); if(stepy<=0) stepy=1;
            Image<T> nimg(getSizeX()/stepx, getSizeY()/stepy);
            Image<T>::const_iterator src = begin();
            Image<T>::iterator dst = nimg.begin();
            for(int j=0; j<nimg.sizeY; j++) {
                Image<T>::const_iterator linebegin = src;
                for(int i=0; i<nimg.sizeX; i++, dst++, src+=stepx) {
                    *dst = *src;
                }
                src = linebegin+(stepy*sizeX);
            }
            return nimg;
        }
    
    /*! \returns true if image has been initialized (data is not NULL)*/
    bool initialized() const { return !empty(); }
    
    friend std::ostream& operator<<(std::ostream& os, const Image<T>& rhs) {
        os << rhs.sizeX << " " << rhs.sizeY << endl;
        int xc = rhs.sizeX;
        for(const_iterator pnt = rhs.begin(); pnt != rhs.end(); pnt++) {
            os<<*pnt;
            if(--xc == 0) {
                cout << endl;
                xc = rhs.sizeX;
            } else cout << " ";
        }
        return os;
    }

    bool writePPM(const std::string& fname) const {
        std::fstream ofile(fname.c_str(), std::ios::binary|std::ios::out);
        return writePPMstream(ofile);
    }

    bool writePPMstream(std::ostream& os) const{
        if(!os) return false;
        os << sizeX << " " << sizeY << endl;
        os.write((const char*)&front(), sizeX*sizeY);
        return true;
    }

    bool readPPM(const std::string& fname) {
        std::fstream ifile(fname.c_str(), std::ios::binary|std::ios::in);
        return readPPMstream(ifile);
    }

    bool readPPMstream(std::istream& is) {
        if(!is) return false;
        is >> sizeX;
        is >> sizeY;
        resize(sizeX*sizeY);
        is.read((char*)&front(), sizeX*sizeY);
        return true;
    }

    friend std::istream& operator>>(std::istream& is, Image<T>& rhs) {
        is >> rhs.sizeX;
        is >> rhs.sizeY;
        rhs.resize(rhs.sizeX*rhs.sizeY);
        for(iterator pnt = rhs.begin(); is && pnt != rhs.end(); pnt++)
            is>>*pnt;
        return is;
    }

    void insert(const Image<T> &ii, const int x = 0, const int y = 0);
    Image<T>& zeroPad(int brd, const T& col=0, bool smoothbrd=false);

protected:
    int sizeX, sizeY;
};

template<class T>
Image<T>::Image()
{
    sizeX = sizeY = 0;
}

template<class T>
Image<T>::Image(const Image & rhs)
{
    operator=(rhs);
}

template<class T>
void Image<T>::freeImage()
{
    setSize(0);
    sizeX = sizeY = 0;
}

template<class T>
void Image<T>::setSize(int nx, int ny)
{
    if(!empty() && nx==sizeX && ny==sizeY) return;
    sizeX = nx;
    sizeY = ny;
    clear();
    if(nx*ny) resize(nx*ny);
}

template<class T>
void Image<T>::setSize(int nx, int ny, const T &value)
{
    sizeX = nx;
    sizeY = ny;
    clear();
    if(nx*ny) resize(nx*ny, value);
}

template<class T>
Image<T>& Image<T>::copy(const Image<T>& rhs)
{
    if(&rhs == this) return *this;
    std::vector<T>::operator=(rhs);
    sizeX = rhs.getSizeX();
    sizeY = rhs.getSizeY();
    return *this;
}

template<class T>
Image<T>& Image<T>::copy(const T* rhs)
{
    for(iterator pnt = begin(); pnt != end(); pnt++, rhs++)
        *pnt = *rhs;
    return *this;
}

template<class T>
Image<T>& Image<T>::operator=(const Image<T>& rhs)
{
    return copy(rhs);
}

template<class T>
Image<T>& Image<T>::operator=(const T& rhs)
{
    if(!empty() && sizeX>0 && sizeY>0)
    {
        for(iterator pnt = begin(); pnt != end(); pnt++)
            *pnt = rhs;
    }
    return *this;
}

template<class T>
const T Image<T>::findMax() const
{
    if(empty()) return 0;
    const T* pnt = getData();
    T max = *pnt;
    for(int i=getSize(); i>0; i--, pnt++)
        if(*pnt>max) max = *pnt;
    return max;
}

template<class T>
const T Image<T>::findMin() const
{
    if(empty()) return 0;
    const T* pnt = getData();
    T min = *pnt;
    for(int i=getSize(); i>0; i--, pnt++)
        if(*pnt<min) min = *pnt;
    return min;
}

template<class T>
const T Image<T>::norm() const
{
    double sum=0;
    const T* pnt = getData();
    for(int i=getSize(); i>0; i--, pnt++)
        sum += *pnt**pnt;
    sum = T(sqrt(sum));
    return T(sum);
}

template<class T>
double Image<T>::sum() const
{
    double sum=0;
    const T* pnt = getData();
    for(int i=getSize(); i>0; i--, pnt++)
        sum += (double)*pnt;
    return sum;
}

template<class T>
Image<T>& Image<T>::operator*=(const T& v)
{
    T* pnt = getData();
    for(int i=getSize(); i>0; i--, pnt++)
        *pnt *= v;
    return *this;
}

template<class T>
Image<T>& Image<T>::operator/=(const T& v)
{
    T* pnt = getData();
    for(int i=getSize(); i>0; i--, pnt++)
        *pnt /= v;
    return *this;
}

template<class T>
Image<T>& Image<T>::operator+=(const T& v)
{
    T* pnt = getData();
    for(int i=getSize(); i>0; i--, pnt++)
        *pnt += v;
    return *this;
}

template<class T>
Image<T>& Image<T>::operator-=(const T& v)
{
    T* pnt = getData();
    for(int i=getSize(); i>0; i--, pnt++)
        *pnt -= v;
    return *this;
}

template<class T>
Image<T>& Image<T>::operator*=(const Image<T>& rhs)
{
    T* pnt = getData();
    const T* rval = rhs.getData();
    for(int i=getSize(); i>0; i--, pnt++, rval++)
        *pnt *= *rval;
    return *this;
}

template<class T>
Image<T>& Image<T>::operator/=(const Image<T>& rhs)
{
    T* pnt = getData();
    const T* rval = rhs.getData();
    for(int i=getSize(); i>0; i--, pnt++)
        *pnt /= *rval;
    return *this;
}

template<class T>
Image<T>& Image<T>::operator+=(const Image<T>& rhs)
{
    T* pnt = getData();
    const T* rval = rhs.getData();
    for(int i=getSize(); i>0; i--, pnt++)
        *pnt += *rval;
    return *this;
}

template<class T>
Image<T>& Image<T>::operator-=(const Image<T>& rhs)
{
    T* pnt = getData();
    const T* rval = rhs.getData();
    for(int i=getSize(); i>0; i--, pnt++)
        *pnt -= *rval;
    return *this;
}

template<class T>
void Image<T>::addLine(int x0, int y0, int x1, int y1, const T& value) 
{
    if(empty()) return;
    //left point first?
    if(x0>x1) {
        x0 += x1; x1 = x0-x1; x0 -= x1;		//who needs a dummy?
        y0 += y1; y1 = y0-y1; y0 -= y1;
    }
    T* dpos = getData() + x0 + y0*sizeX;
    int pincY = sizeX;
    int xinc = 1;
    int yinc = 1;
    int dy = y1 - y0; 
    int dx = x1 - x0; 

    int nx = dy;	//normal (negative reciprocal direction)
    int ny = -dx; 

    int dir = dx>absint(dy) ? 1 : -1;
    if(dy<0) 
    {
        yinc=-1;
        pincY = -pincY;
    }

    if(dir==1)
    {
        int diagInc = 2*(nx+ny*yinc);
        int axisInc = 2*nx;

        int x = x0;
        int y = y0;

        int d = 2*nx + ny*yinc;
        while (x <= x1) 
        {
            *dpos += value;
            if (d*yinc >= 0) {						// move diagonally
                y += yinc;
                dpos += pincY;
                d += diagInc;
            }
            else d += axisInc;
            x += xinc;
            dpos += xinc;
        }
    } else {
        int diagInc = 2*(nx+ny*yinc);
        int axisInc = 2*ny*yinc;

        int x = x0;
        int y = y0;
        int d = 2*ny*yinc + nx;
        int cnt = absint(dy);
        while (cnt--) 
        {
            *dpos += value;
            if (d*yinc <= 0) {						// move diagonally
                x += xinc;
                dpos += xinc;
                d += diagInc;
            }
            else d += axisInc;
            y += yinc;
            dpos += pincY;
        }
        *dpos += value;
    }
}

template <class T>
Image<T>& Image<T>::threshold(const T& th, double binarize)
{
    bool bin = (binarize>0);
    T* pnt = getData();
    for(int i=getSize(); i>0; i--, pnt++)
        if(*pnt<th) *pnt = 0;
        else if (bin) *pnt = T(binarize);
    return *this;
}

template <class T>
Image<T>& Image<T>::unsetNAN(double val)
{
    T* pnt = getData();
    for(int i=getSize(); i>0; i--, pnt++)
	if(isnan(*pnt)) *pnt = val;
    return *this;
}

template <class T>
Image<T>& Image<T>::findMaxima(int env)
{
    env = (env/2)*2 + 1;
    int ksx = env/2;
    int ksy = env/2;
    int endX = sizeX-ksx;
    int endY = sizeY-ksy;
    int offset = ksy*sizeX+ksx;
    int winskip = sizeX-env;
    T* srcdat = getData();
    T* maxpnt = getData()+offset;
    for(int i=ksy;i<endY;i++)
    {
        for(int j=ksx;j<endX;j++,srcdat++, maxpnt++)
        {
            bool ismax = true;
            const T *sdat = srcdat;
            maxpnt = srcdat+offset;
            for(int l=0;l<env && ismax; l++)
            {
                for(int k=0;k<env && ismax; k++, sdat++)
                {
                    if(*maxpnt<=*sdat &&
                       maxpnt != sdat) ismax=false;
                }
                sdat += winskip;	//jump to next line
            }
            if(!ismax) *maxpnt = 0;
        }
        maxpnt+=ksx*2;
        srcdat+=ksx*2;
    }
    return *this;
}

template <class T>
void Image<T>::insert(const Image<T> &ii, const int x, const int y) 
{
    const int iwidth = std::min(getSizeX() - x, ii.getSizeX());
    const int iheight = std::min(getSizeY() - y, ii.getSizeY());
    if(iwidth>0 && iheight>0)
    {
        for(int j=0; j<iheight; j++) {
            const_iterator src = ii.begin()+ii.getIndex(0,j);
            iterator dst = begin()+getIndex(x,j+y);
            for(int i=0; i<iwidth; i++, src++, dst++)
            {
                *dst = *src;
            }
        }
    }
}

template<class T>
Image<T>& Image<T>::zeroPad(int brd, const T& col, bool smoothbrd)
{
    if(brd>0)
    {
        Image<T> ni(getSizeX()+2*brd, getSizeY()+2*brd, col);
        ni.insert(*this, brd, brd);
        if(smoothbrd) {
            //smooth the inner image and blit again
            Image<float> flt(2*brd,2*brd, 1.0f/(4*brd*brd));
            ni.convolve(flt);
            ni.insert(*this, brd, brd);
        }
        *this = ni;
    }
    return *this;
}


#endif
