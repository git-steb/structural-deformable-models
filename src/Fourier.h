#ifndef _FOURIER_H_
#define _FOURIER_H_

#include <fftw3.h>

class Fourier2D {
public:
    Fourier2D();
    ~Fourier2D();
    void initTransform(int xsize, int ysize, double* f, double* F);
    void destroyTransform();
    void transform();
    void transformInv();
    static int complexX(int sizex) { return (sizex/2+1)*2; }
    int complexX() const { return complexX(m_SizeX); }
    int complexY() const { return m_SizeY; }
    int realX() const { return m_SizeX; }
    int realY() const { return m_SizeY; }

protected:
    void loadWisdom();
    void saveWisdom();

//--- member variables ---
    fftw_plan           m_Plan2D, m_Plan2Dinv;
    bool                m_PlansCreated;
    int                 m_SizeX, m_SizeY;
    static bool         s_WisdomLoaded, s_WisdomModified;
};

#endif
