/* -*- C++ -*- */
#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include <vector>
#include <list>

#include "Node.h"
#include "Edge.h"
#include "VVector.h"
#include "ParseFile.h"
#include "DMatrix.h"
#include "PropVec.h"

//-----------------------------------------------------------------------------
#define __SHAPEWEIGHT   0.1

//-----------------------------------------------------------------------------
typedef std::vector<Node> NodeArray;
typedef std::vector<Edge> EdgeArray;

//--- for spring model
typedef struct {
    Point dpdt;
    Point dvdt;
} NodeDerivative;

//---

#ifndef SWIG
class CAMgraphicsProcess;
#endif
class SensorCollection;

class Model {
protected:
    enum PreCompF {PC_CENTER=0x01, PC_SCALE=0x02, PC_DIR=0x04, PC_QOF=0x08,
                   PC_LIVE=0x10, PC_NOTHING=0x00, PC_ALL=0x1f,
                   PC_PVEC=PC_CENTER|PC_SCALE|PC_DIR,
                   PC_STATS=PC_QOF|PC_LIVE};
public:
    enum DistType {DIST_XY, DIST_XYS, DIST_PVEC, DIST_POINTS,
                   DIST_CPOINTS, DIST_HPOINTS };
    //enum TTimeStamp { TS_CREATE=0, TS_WIN, TS_LOOSE, TS_LAST};
    enum ModelState { ST_WINNER=1, ST_LOOSER=2, ST_OLDSTATE=4, ST_DEL=8,
                      ST_MEMBER=16, ST_NODEL=32};
    //! Default constructor.
    Model(dataset_cptr _dataset=NULL, SensorCollection *sensors=NULL);
    //! Copy constructor
    Model(const Model& rhs);
    //! Destructor.
    ~Model();

    //! remove and destroy all geometry information (nodes and edges)
    void clear();
    //! is initialized?
    operator bool() const { return !nodes.empty(); }
    //! reset all model information (including geometry)
    void reset();
    //! add a Node to the model
    //! \returns index in the internal array
    int addNode(const Node &node);
    //! add an Edge to the model
    //! \returns index in the internal array
    int addEdge(const Edge &edge);
    //! create an Edge from two given nodes
    //! \returns index in the internal array
    int addEdge(const Node &from, const Node &to);
    //! create an Edge using node indices (from nodes - array)
    //! \returns index in the internal array
    int addEdge(int ifrom, int ito);

    const Edge& getEdge(int index) const { return edges[index]; }
    Edge& getEdge(int index) { return edges[index]; }

    const Node& getNode(int index) const { return nodes[index]; }
    Node& getNode(int index) { return nodes[index]; }

    int getNNodes() const { return nodes.size(); }
    int getNEdges() const { return edges.size(); }
    float getNodeAngle(const Node &n) const;
    void setName(const std::string &name);
    const std::string& getName() const { return m_Name; }
    dword getCRC() const { return m_CRC; }
    dword getID() const { return m_ID; }
    void setID(dword id) { m_ID = id; }

    //! Draw using OpenGL
    void draw(bool drawPoints=false) const;
    //! Draw to postscrip output
    //void drawPS(CAMgraphicsProcess& gp, dword mode=0) const;

    // stream operation
    //! read edge from stream
    bool readEdge(ParseFile &is);
    //! read node from stream
    bool readNode(ParseFile &is);
    //! read sensor from stream
    bool readSensor(ParseFile &is);
    //! read model parameters
    bool readParameter(ParseFile &is);
    /** Read complete geometry from input stream.
        # is comment character, v introduces a point (vertex), e an edge:
        #example
        v 1 1
        v 2 2
        e 1 2
    */
    friend ParseFile& operator>>(ParseFile &is, Model &g);
    /** Read geometry from stream. */
    friend std::ostream& operator<<(std::ostream &is, const Model &g);
    Model& operator=(const Model& rhs);
    /** Read geometry from file.  See operator>>() for further details. */
    bool readFile(const char* filename, bool fullread=true);
    /** Write geometry to file. */
    bool writeFile(const char* filename) const;
    void attachDataset(dataset_cptr dataset);

    float getQualityOfFit() const;
    float getLiveliness() const;
    void translate(const Point &t);
    void addNoise(float r);
    void freeze();
    void push(const Point &t);
    void rotate(float angle, const Point &c);
    void rotate(float angle) {
        dword cf=m_PCFlags&PC_CENTER;rotate(angle,getCenter());m_PCFlags|=cf;}
    void pushRotate(const Point &c, float angle);
    void attract(const Point &c, float factor);
    //! Returns overall sum of edge lengths.
    float getFullLength() const;
    //! Returns mean squared distance from centroid.
    float getStdRadius(const Point& center) const;
    float getStdRadius() const { return getStdRadius(getCenter());}
    //! Returns the maximum radius from the centroid
    float getMaxRadius(const Point& center) const;
    float getMaxRadius() const { return getMaxRadius(getCenter());}

    //! multiply all rest lengths by factor
    void scale(float factor, bool movepoints=false);
    //! multiply all rest lengths of selected edges by factor
    void scaleSel(float factor, const DMatrixf& selm);
    //! adapt restlengths towards current lengths by ratio
    void adaptRestLength(float ratio);
    //! adapt restlengths of selected edges towards current lengths by ratio
    void adaptRestLengthSel(float ratio, const DMatrixf& selm);
    //! overall rest lengths sum is adapted to overall edge lengths by ratio
    void adaptProportion(float ratio);
    //! relation of overall edge length by overall rest length sum
    float getLengthRatio() const;
    //! mean squared difference from restlength normalized by restlength sum
    float getLengthVariation() const;
    //! normalized length variation to compensate for different sizes
    float getDeformation() const;
    //! get average sensor value
    float getSensorFit(dword* npts=NULL) const;
    //! get average fit of sensors along edges
    float getEdgeSensorFit(dword* npts=NULL) const;

// functions for spring model calculations
    void updateParticles(float dt,int method=0);
    void resetForces();
    void calculateForces(float dt);
    void calculateDerivatives(std::vector<NodeDerivative> &deriv);
    float addImageForces(float dt);
    void addTorque(float dt);
    void prepareTorque();

    void mergeSensorCollection(SensorCollection *sensors);
    SensorCollection* getSensorCollection() { return m_Sensors; }
    //! get sensors from sensor collection
    void reattachSensors();
    //! auto-create appropriate edge sensors (dist*stdRadius is distance)
    dword createEdgeSensors(float dist=0.1);

    const Point getCenter() const;
    float getDirection() const;
    const Node& getDirNode() const;
    void setDirNode(int id) { m_DirNodeInd = id; invalidatePC(PC_DIR); }

    void setWinner(bool iswin=true, float ts=0.) {
        if(iswin) m_Flags |= ST_WINNER;
        else m_Flags &= ~ST_WINNER;
        //if(ts!=0.) m_TimeStamp[TS_WIN]=ts;
    }
    bool isWinner() { return m_Flags&ST_WINNER; }
    void setLooser(bool isloose=true, float ts=0.) {
        if(isloose) m_Flags |= ST_LOOSER;
        else m_Flags &= ~ST_LOOSER;
        //if(ts!=0.) m_TimeStamp[TS_LOOSE]=ts;
    }
    bool isLooser() { return m_Flags&ST_LOOSER; }
    void setOldState(bool isoldstate=true, float ts=0.) {
        if(isoldstate) m_Flags |= ST_OLDSTATE;
        else m_Flags &= ~ST_OLDSTATE;
    }
    bool isOldState() { return m_Flags&ST_OLDSTATE; }
    dword& getFlags() { return m_Flags; }
    bool hasFlags(dword flags) const { return m_Flags & flags; }
    dword& setFlags(dword flags) { return m_Flags|=flags; }
    dword& unsetFlags(dword flags) { return m_Flags&=~flags; }
    dword& switchFlags(dword flags) { return m_Flags^=flags; }
    void setShapeWeight(float weight) {
        m_ShapeWeight = weight; invalidatePC(PC_QOF); }
    void adaptDataScale(dword ppmm=0);
    dword getDataScale() const { return m_ppmm; }

    void adaptProperties(const PropVec &prop);
    const PropVec& getProperties() const;
    //! return property vector using millimeter scale
    PropVec getPropertiesMM() const;
    //! adapt to a property vector using millimeter scale
    void adaptPropertiesMM(PropVec prop);
    Point2D& convertPointFromMM(Point2D& pt) const;
    Point2D& convertPointToMM(Point2D& pt) const;
    PropVec& convertPropFromMM(PropVec& prop) const;
    PropVec& convertPropToMM(PropVec& prop) const;

    int nearestNode(const Point& pos, float& dist=*(float*)(NULL)) const;
    float distance(const Model& rhs,
                   enum Model::DistType kind=DIST_POINTS) const;
    void invalidatePC(dword pcflags=PC_ALL) { m_PCFlags &= ~pcflags; }

    dword getInstCount() const { return m_InstCount; }
    void setInstCount(dword ic) { m_InstCount=ic; }
    dword mergeModel( Model& mergemod);
    const std::string& getFilename() const { return m_Filename; }

    /** Set ID of highlighted node.
        Pass invalid node id (e.g. -1) to have none highlighted. */
    void setHLNode(int hlnode);
    //! Returns ID of highlighted node.  Value is -1 if none is selected.
    int getHLNode() const { return m_HLNode; }
    //! get number of selected nodes
    dword getSelectedNodesN(dword state=Node::ST_SELECT) const;

    //! Connect selected nodes.
    dword connectNodes();
    //! Remove edges between selected nodes.
    dword removeEdges();
    //! Removes a node and its adjacent edges.
    void removeNode(int nid);
    //! compute undirected adjacency matrix
    DMatrixf adjMat() const;
    //! compute mask matrix of selected nodes
    DMatrixf selMaskMat() const;

protected:
    //! remove edged not mentioned in (non-directional) adjacency matrix
    dword removeByAdjMat(DMatrixf& adjm);
    //! create edges mentioned in adjacency matrix
    dword createByAdjMat(const DMatrixf& adjm);
    //! rebuild cross indexing between nodes and edges
    void rebuildIndexTables();
    //! set reference to and from sensor collection for correct updates
    void attachSensorColl(SensorCollection* sc);

public:
    ParticleParam phys;                 //!< physical parameter set
    //std::vector<float> m_TimeStamp;          // time stamps (see. TTimeStamp)
    float         m_TimeStamp;          //!< time stamps (see. TTimeStamp)

protected:
    NodeArray nodes;                    //!< indexed list of nodes
    EdgeArray edges;                    //!< indexed list of edges

    dataset_cptr                m_Dataset;
    SensorCollection            *m_Sensors;
    std::string                 m_SensorFile;
    float                       m_ShapeWeight;
    dword                       m_Flags;
    std::string         m_Name;
    dword           m_CRC;
    dword                       m_ppmm;
    mutable int                 m_DirNodeInd;
    mutable dword               m_PCFlags;
    mutable PropVec             m_PropVec;
    mutable float               m_QOF;
    mutable float               m_Liveliness;
    mutable float               m_Scale;
    mutable float               m_Direction;
    dword                       m_InstCount; //!< number of merged instances
    std::string                      m_Filename;
    //const static PropVec        s_PropWeights;
    mutable int                 m_HLNode;
    mutable dword               m_ID;
    //std::vector<NodeDerivative> m_Deriv;
    //float                       m_LastDT;
};

#endif
