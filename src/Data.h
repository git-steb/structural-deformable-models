/* data set functions -*- C++ -*- */
#ifndef _DATASET_H_
#define _DATASET_H_

#include <vector>
#include "common.h"
#include "Image.h"
#include "glImage.h"
#include "Sensor.h"
#include "PropVec.h"
#include "vuThread.h"

typedef Image<byte> ByteImage;
typedef Image<float> FloatImage;

#define DEFAULT_PPMM    0

/** Implements data management.  Function for loading and querying the
    dataset. Also a set of data sheets is managed that contains
    preprocessed results for sensor evaluation. */
class Dataset : public Sensor {
//    friend class Sensor;
 public:
    /** Default constructor */
    Dataset();
    /** Constructor */
    Dataset(const char* filename);

    //Dataset(const Dataset& rhs) { operator=(rhs); }
    /** Destructor */
    ~Dataset() {};

    //Dataset& operator=(const Dataset& rhs);

    /** Load data set from file */
    bool load(const char* filename, dword ppmm=0);

    Dataset& copyData(const Dataset& rhs);

    /** Draw data set (using OpenGL) */
    void draw(float x=0, float y=0,
	      float sx = 1.0f, float sy = 1.0f) const;

    int getDim1Size() const { return m_Dim1; }
    int getDim2Size() const { return m_Dim2; }
    int getDim3Size() const { return m_Dim3; }
    int getNChannels() const { return m_NChannels; }
    std::ostream& print(std::ostream &os) const 
        { return os; } //don't print to model file

    void clear();
    bool initialized() const { return m_Dim1 && m_Dim2; }
    operator bool() const { return initialized(); }
    const ByteImage& getImage() const { return data.at(m_CurrImage); }
    ByteImage& getImage() { return data.at(m_CurrImage); }
    const ByteImage& getImage(int index) const { return data.at(index); }
    ByteImage& getImage(int index) { return data.at(index); }

    /** Load an Image<byte> */
    static bool loadImage(std::vector<Image<byte> > &image, 
                          const char *filename);
    /** Draw Image using glDrawPixels() */
    static void drawImage(const std::vector<Image<byte> > &image, 
                          float x=0, float y=0,
			  float sx = 1.0f, float sy = 1.0f);
    static void drawImage(const Image<byte> &image, float x=0, float y=0,
			  float sx = 1.0f, float sy = 1.0f);
    vuMutex writeMutex;

    dword getPPMM() const { return m_ppmm; }
    void setPPMM(dword ppmm) { m_ppmm = ppmm ? ppmm : DEFAULT_PPMM; }
    const Point2D& getOrigin() const { return m_Origin; }
    const Point2D& getOSize() const { return m_OSize; }

    const std::string& getFilename() const { return m_Filename; }
    PropVec getPropVec() const 
        { PropVec pv(0.f); return setPropScale(pv, (float)m_Dim1); }
    PropVec getPropVecMM() const 
        { PropVec pv(getPropVec()); return scalePropVec(pv, 1.f/m_ppmm); }
    void setHalveBeyondSize(dword hbs=0xffffffff) { m_HalveBeyondSize = hbs; }
    dword getHalveBeyondSize() const { return m_HalveBeyondSize; }
    void addNoise(float sigma);
 protected:
    /** implementing virtual from class Sensor */
    float calcValue(int x, int y) const {
	//vuLock l(*((vuMutex*)&writeMutex));
	if((dword)x < (dword)m_Dim1 && (dword)y < (dword)m_Dim2)
	    return data[m_CurrImage].getPixel(x,y);
	else return 0.0f;
    }
    /** implementing virtual from class Sensor */
    std::vector<float> calcMValue(int x, int y) const {
	//vuLock l(*((vuMutex*)&writeMutex));
	std::vector<float> mv(m_NChannels);
	int cc = 0;
	for(std::vector<float>::iterator cv = mv.begin();
	    cv != mv.end(); cv++, cc++) {
	    *cv = (float)data[cc].getPixel(x,y);
	}
        return mv;
    }
    void calcMinMax() {
        maxval = 255;
        minval = 0;
    }
    /** The data as one dimensional array.  If more than one channel
     is used data will be organized as vector<ByteImage>. */
    std::vector<ByteImage>   data;

    /** Dimensions in X, Y, and Z direction + number of channels */
    int                 m_Dim1, m_Dim2, m_Dim3, m_NChannels;
    int                 m_CurrImage;  //!< current image
    dword               m_ppmm;
    std::string         m_Filename;
    Point2D             m_Origin, m_OSize;
    mutable GLImage     m_GLImage;
    mutable bool        m_RefreshImage;
    dword               m_HalveBeyondSize; //<! if(img>hbs) halve resolution
};

#endif
