#include <iostream>
#include <stdio.h>
#include <assert.h>
#include "Fourier.h"
#include "common.h"

#define WISDOM "fftw_stats.wis"
bool Fourier2D::s_WisdomLoaded = false;
bool Fourier2D::s_WisdomModified = false;

#define F2D_REAL_TRANSFORM

//#define F2D_PLANQUAL FFTW_MEASURE
#define F2D_PLANQUAL FFTW_ESTIMATE

Fourier2D::Fourier2D() : m_PlansCreated(false) {
    loadWisdom();
}
Fourier2D::~Fourier2D() {
    destroyTransform();
    saveWisdom();
}
void Fourier2D::transform() {
    assert(m_PlansCreated);
    fftw_execute(m_Plan2D);
}

void Fourier2D::transformInv() {
    assert(m_PlansCreated);
    fftw_execute(m_Plan2Dinv);
}

void Fourier2D::loadWisdom() {
    if(!s_WisdomLoaded) {
        FILE* wisdom_file = fopen(WISDOM, "a+");
        rewind(wisdom_file);
        fftw_import_wisdom_from_file(wisdom_file);
        fclose(wisdom_file);
        s_WisdomLoaded = true;
    }
}

void Fourier2D::saveWisdom() {
    if(s_WisdomLoaded && s_WisdomModified) {
        FILE* wisdom_file = fopen(WISDOM, "w");
        fftw_export_wisdom_to_file(wisdom_file);
        fclose(wisdom_file);
        std::cout << "fftw plan written to file " << WISDOM << "."<<std::endl;
        s_WisdomModified = false;
    }
}

void Fourier2D::initTransform(int xsize, int ysize, double* f, double *F) {
    if(m_PlansCreated) destroyTransform();
#ifdef F2D_REAL_TRANSFORM
    m_Plan2D    = fftw_plan_dft_r2c_2d(ysize, xsize, f, (fftw_complex*)F, 
                                       F2D_PLANQUAL);
    m_Plan2Dinv = fftw_plan_dft_c2r_2d(ysize, xsize, (fftw_complex*)F, f, 
                                       FFTW_ESTIMATE);
#else
    m_Plan2D    = fftw_plan_dft_2d(ysize, xsize, 
                                   (fftw_complex*)f, (fftw_complex*)F, 
                                   FFTW_FORWARD, FFTW_ESTIMATE);
    m_Plan2Dinv = fftw_plan_dft_2d(ysize, xsize, 
                                   (fftw_complex*)f, (fftw_complex*)F, 
                                   FFTW_BACKWARD, FFTW_ESTIMATE);
#endif
    m_SizeX = xsize; m_SizeY = ysize;
    m_PlansCreated = true;
    s_WisdomModified = true;
}

void Fourier2D::destroyTransform() {
    if(m_PlansCreated) {
        fftw_destroy_plan(m_Plan2D);
        fftw_destroy_plan(m_Plan2Dinv);
        m_SizeX = m_SizeY = 0;
        m_PlansCreated = false;
    }
}
