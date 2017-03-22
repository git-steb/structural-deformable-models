#ifdef WIN32
#include <windows.h>
#endif
#include <fstream>
#include <sstream>
#include <math.h>
#include <GL/gl.h>
#include "Data.h"

//-- image loader setup
#include "fxconfig.h"
#include <fx.h>
#ifdef HAVE_PNG_H
#include <FXPNGImage.h>
#endif
#ifdef HAVE_JPEG_H
#include <FXJPGImage.h>
#endif
#ifdef HAVE_TIFF_H
#include <FXTIFImage.h>
#endif
#include <FXICOImage.h>
#include <FXTGAImage.h>
#include <FXRGBImage.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <signal.h>
#ifndef WIN32
#include <unistd.h>
#endif
using namespace std;

//-----------------------------------------------------------------------------
//static declarations
static void copyFXImage2Image(vector<Image<byte> > &dimg, const FXImage &img);
static void copyFXImage2Image(Image<byte> &dimg, const FXImage &img);
static void copyImage2FXImage(FXImage &img, const vector<Image<byte> >& dimg);

template<class T>
Image<T>& makeNoiseImage(Image<T>& nimg, T mean, T sigma);

//-----------------------------------------------------------------------------
//Data methods

Dataset::Dataset() : data(1),
                     m_Dim1(0), m_Dim2(0), m_Dim3(0), m_NChannels(0),
                     m_CurrImage(0),m_ppmm(DEFAULT_PPMM),m_RefreshImage(false),
                     m_HalveBeyondSize(0xffffffff)
{
    setID("d0");
}

Dataset::Dataset(const char* filename) : m_Dim1(0), m_Dim2(0), m_Dim3(0),
                                         m_NChannels(0), m_CurrImage(0),
                                         m_ppmm(DEFAULT_PPMM),
                                         m_RefreshImage(false)
{
    load(filename);
}

bool Dataset::load(const char* filename, dword ppmm)
{
    if(data.empty()) data.push_back(*(new ByteImage()));
    vuLock l(writeMutex);
    bool loaded = loadImage(data, filename);
    if(loaded) {
        m_Filename = filename;
        m_Origin = 0.0f;
        if(data[0].getSizeY()>3 && data[0].getSizeX()>3) {
            ByteImage zimg(data[0].getSizeX()-2, data[0].getSizeY()-2,0);
            int brd = (1<<6);
            m_Origin = (float)brd;
            for(vector< ByteImage >::iterator ii=data.begin();
                ii!=data.end();ii++)
            {
                if((dword)ii->getSize() > m_HalveBeyondSize) {
                    *ii = ii->scaleBy(0.5);
                    ppmm /= 2;
                    cout << "reducing image" << endl;
                }
                ByteImage copimg(*ii);
                copimg.insert(zimg, 1, 1);
                double avgcol = copimg.sum()/
                    ((copimg.getSizeX()+copimg.getSizeY())*2-4);
                ii->zeroPad(brd, byte(avgcol));
                // make edge length even
                dword nx = (ii->getSizeX()/2+1)*2;
                dword ny = (ii->getSizeY()/2+1)*2;
                ByteImage ni(nx, ny, byte(avgcol));
                ni.insert(*ii);
                *ii = ni;
//                 stringstream fname;
//                 fname << "bimage" << ii-data.begin() << ".ppm";
//                 if(ii->writePPM(fname.str())) cout << "ok." << endl;
            }
        }
        m_OSize.x = (float)data[0].getSizeX();
        m_OSize.y = (float)data[0].getSizeY();
        m_CurrImage = 0;
        m_Dim1 = data[0].getSizeX();
        m_Dim2 = data[0].getSizeY();
        m_Dim3 = 1;
        m_RefreshImage = true;
        m_NChannels = data.size();
        setPPMM(ppmm);
        setModified(UPD_DATA);
        performUpdate();
        return true;
    } else return false;
}

void Dataset::draw(float x, float y, float sx, float sy) const
{
    if(!data.empty())
    {
        if(m_RefreshImage) {
            m_GLImage.unsetImage();
            m_GLImage.setImage(data);
            m_RefreshImage = false;
        }
        m_GLImage.draw(x, y, sx, sy);
        //drawImage(data,x,y,sx,sy);
    }
}

Dataset& Dataset::copyData(const Dataset& rhs)
{
    data = rhs.data;
    m_CurrImage = 0;
    m_Dim1 = data[0].getSizeX();
    m_Dim2 = data[0].getSizeY();
    m_Dim3 = 1;
    m_NChannels = data.size();
    m_ppmm = rhs.m_ppmm;
    m_Filename = rhs.m_Filename;
    m_HalveBeyondSize = rhs.m_HalveBeyondSize;
    setModified(UPD_DATA);
    performUpdate();
    return *this;
}

void Dataset::clear()
{
    m_Dim1 = m_Dim2 = m_Dim3 = 0;
    m_NChannels = 0; m_CurrImage = 0;
    m_ppmm = DEFAULT_PPMM;
    data.clear();
    data.resize(1);
    m_Filename.clear();
    m_Origin = 0.0f;
    setModified(UPD_DATA);
    performUpdate();
}

//---------------------------------------------------------------------------
// Data static methods

bool Dataset::loadImage(vector< Image<byte> > &image, const char* file){
    FXString ext=FXPath::extension(file);
    FXImage *img=NULL;
    FXApp *app = FXApp::instance(); //getApp()

    if(comparecase(ext,"bmp")==0){
        img=new FXBMPImage(app,NULL,IMAGE_KEEP|IMAGE_SHMI|IMAGE_SHMP);
    }
    else if(comparecase(ext,"gif")==0){
        img=new FXGIFImage(app,NULL,IMAGE_KEEP|IMAGE_SHMI|IMAGE_SHMP);
    }
    else if(comparecase(ext,"tga")==0){
        img=new FXTGAImage(app,NULL,IMAGE_KEEP|IMAGE_SHMI|IMAGE_SHMP);
    }
#ifdef HAVE_PNG_H
    else if(comparecase(ext,"png")==0){
        img=new FXPNGImage(app,NULL,IMAGE_KEEP|IMAGE_SHMI|IMAGE_SHMP);
    }
#endif
#ifdef HAVE_JPEG_H
    else if(comparecase(ext,"jpg")==0){
        img=new FXJPGImage(app,NULL,IMAGE_KEEP|IMAGE_SHMI|IMAGE_SHMP);
    }
#endif
#ifdef HAVE_TIFF_H
    else if(comparecase(ext,"tif")==0 || comparecase(ext,"tiff")==0){
        img=new FXTIFImage(app,NULL,IMAGE_KEEP|IMAGE_SHMI|IMAGE_SHMP);
    }
#endif
    bool load_ok = false;
    if(img!=NULL) {
        // Load it
        FXFileStream stream;
        if(stream.open(file,FXStreamLoad)){
            //app->beginWaitCursor();
            load_ok = img->loadPixels(stream);
            if(load_ok) {
                img->create();
                //app->endWaitCursor();
                //img->mirror(false, true);
                copyFXImage2Image(image, *img);
            }
            stream.close();
        }
        delete img;
    }
    //note: modal error message causes deadlock on Brain::m_DataMutex
    //between Brain::loadData and Brain::drawAllModels
    //if(!load_ok)
    //    FXMessageBox::error(/*this app->getRootWindow()*/ app,MBOX_OK,"Error Loading Image","Unsupported type: %s",ext.text());
    return load_ok;
}

void Dataset::addNoise(float sigma)
{
    for(vector< ByteImage >::iterator ii=data.begin();
        ii!=data.end();ii++)
    {
        byte* data = ii->getData();
        for(int i=ii->getSize(); i>0; i--, data++)
        {
            int val = int(int(*data) + fgauss01()*sigma);
            if(val<0) *data = 0;
            else if(val>255) *data = 255;
            else *data = byte(val);
        }
    }
    m_RefreshImage = true;
}

void Dataset::drawImage(const Image<byte> &image, float x, float y,
                        float sx, float sy)
{
    glPixelZoom(sx, sy);
    glRasterPos3f(x,y,0.0f);
    glDrawPixels(image.getSizeX(),
                 image.getSizeY(),
                 GL_LUMINANCE,
                 GL_UNSIGNED_BYTE,
                 image.getData());
}

byte* interleave(const vector< Image<byte> > &img) {
    const int comp = img.size()>3 ? 3 : img.size();
    int size = img[0].size();
    byte *b = new byte[size*3];
    if(comp<3) {
        Image<byte>::const_iterator bi = img[0].begin();
        byte *cv = b;
        for(int i=0; i!=size; i++, cv++, bi++)
            *cv = *bi;
    } else { // comp == 3
        Image<byte>::const_iterator bi[comp];
        for(int a=0; a<comp; a++) bi[a] = img[a].begin();
        byte *cv = b;
        for(int i=0; i<size; i++) {
            *(cv++) = *bi[0]; bi[0]++;
            *(cv++) = *bi[1]; bi[1]++;
            *(cv++) = *bi[2]; bi[2]++;
        }
    }
    return b;
}

void Dataset::drawImage(const vector< Image<byte> > &image, float x, float y,
                        float sx, float sy)
{
    byte *buf = interleave(image);
    glPixelZoom(sx, sy);
    glRasterPos3f(x,y,0.0f);
    glDrawPixels(image[0].getSizeX(),
                 image[0].getSizeY(),
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 buf);
    delete buf;
}

//--------------------------------------------------------------------------
//--- internal static

static void copyFXImage2Image(vector< Image<byte> > &dimg, const FXImage &img)
{
    //FXImage *img=imgv->getImage();
    // copy to image buffer
    int height = img.getHeight();
    int width = img.getWidth();
    int size = width*height;
    byte *imgdat = (byte*)img.getData();
    const int nchan = 4;
    dimg.clear();
    dimg.resize(nchan);//, Image<byte>(width,height));
    for(vector< Image<byte> >::iterator ii = dimg.begin();
        ii!=dimg.end(); ii++)
        ii->setSize(width, height);
    //dimg.setSize(width,height);
    int sind = 0;
    for(int i=size;i>0;i--,imgdat+=nchan,sind++)
    {
        for(int c=0; c<nchan; c++) {
            dimg[c][sind] = imgdat[c];
        }
    }
}

static void copyFXImage2Image(Image<byte> &dimg, const FXImage &img)
{
    //FXImage *img=imgv->getImage();
    // copy to image buffer
    int height = img.getHeight();
    int width = img.getWidth();
    int size = width*height;
    byte *imgdat = (byte*)img.getData();
    dimg.setSize(width,height);
    byte *htimg = dimg.getData();
    int nchan = 4;
    for(int i=size;i>0;i--,imgdat+=nchan,htimg++)
    {
        //int val = int(0.299f*imgdat[0]+0.587f*imgdat[1]+0.114f*imgdat[2]);
        //int val = (int(imgdat[0])+imgdat[1]+imgdat[2])/3;
        int val = imgdat[0];
        //*htimg = val > 255 ? 255 : val;
        *htimg = byte(val);
    }
}

// copy to image buffer
static void copyImage2FXImage(FXImage &img, const Image<byte>& dimg)
{
    int height = img.getHeight();
    int width = img.getWidth();
    img.resize(dimg.getSizeX(), dimg.getSizeY());
    int size = width*height;
    byte *imgdat = (byte*)img.getData();
    const byte *htimg = dimg.getData();
    int nchan = 4;
    for(int i=size;i>0;i--,imgdat+=nchan,htimg++)
    {
        imgdat[0] = *htimg;
        imgdat[1] = *htimg;
        imgdat[2] = *htimg;
    }

    img.create();
}

template<class T>
Image<T>& makeNoiseImage(Image<T>& nimg, T mean, T sigma)
{
    T* data = nimg.getData();
    for(int j=0; j<nimg.getSizeY(); j++)
        for(int i=0; i<nimg.getSizeX(); i++, data++)
        {
            *data = (T)(fgauss01()*sigma+mean);
        }
    return nimg;
}
