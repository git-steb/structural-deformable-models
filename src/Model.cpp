#ifdef WIN32
#include <windows.h>
#endif

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctype.h>

#include "Model.h"
#include "SensorSet.h"
#include "SensorColl.h"
#include "Data.h"
#include "PartParam.h"
#include "Searcher.h"
#include "crc.h"
#include "mathutil.h"
#include "camgraph/gprocess.h"

using namespace std;

ParticleParam ParticleParam::global = ParticleParam();

Model::Model(const Dataset *_dataset, SensorCollection *sensors)
    : m_TimeStamp(0), m_Dataset(_dataset), m_Sensors(sensors), 
      m_Flags(0), m_PCFlags(PC_NOTHING), m_InstCount(1), m_HLNode(-1), m_ID(0)
{
    phys.gravitational = phys.gravitational;
    phys.viscousdrag = phys.viscousdrag;
    m_ShapeWeight = __SHAPEWEIGHT;
    setName("00");
    m_ppmm = DEFAULT_PPMM;
    m_DirNodeInd=-1;
    if(m_Sensors) m_Sensors->refModel(this);
}

Model::Model(const Model& rhs)
{
    m_Sensors = NULL;
    operator=(rhs);
}

Model::~Model() 
{
    attachSensorColl(NULL);
}

Model& Model::operator=(const Model& rhs)
{
    if(&rhs == this) return *this;
    m_Dataset = rhs.m_Dataset;
    attachSensorColl(rhs.m_Sensors);
    m_SensorFile = rhs.m_SensorFile;
    nodes = rhs.nodes;
    edges = rhs.edges;
    for(EdgeArray::iterator e=edges.begin(); e!=edges.end(); e++)
    {
	e->setNodesList(nodes);
    }
    m_Flags = rhs.m_Flags;
//     phys.gravitational = rhs.phys.gravitational;
//     phys.viscousdrag = rhs.phys.viscousdrag;
    phys = rhs.phys;
    m_Name = rhs.m_Name;
    m_CRC = rhs.m_CRC;
    m_ppmm = rhs.m_ppmm;
    m_TimeStamp = rhs.m_TimeStamp;
    m_DirNodeInd = rhs.m_DirNodeInd;
    m_PCFlags = rhs.m_PCFlags;
    m_PropVec = rhs.m_PropVec;
    m_Liveliness = rhs.m_Liveliness;
    m_ShapeWeight = rhs.m_ShapeWeight;
    m_QOF = rhs.m_QOF;
    m_InstCount = rhs.m_InstCount;
    m_Filename = rhs.m_Filename;
    m_HLNode = rhs.m_HLNode;
    //m_Deriv = rhs.m_Deriv;
    //m_LastDT = rhs.m_LastDT;
    m_ID = rhs.m_ID;
    return *this;
}

void Model::attachDataset(const Dataset *dataset) {
    if(m_Dataset == dataset) return;
    m_Dataset = dataset;
    if(m_Sensors && m_Sensors->getSensor("d0") != dataset) 
        m_Sensors->addSensor("d0", (Sensor*)dataset);
    adaptDataScale();
}

void Model::clear()
{
    nodes.clear();
    edges.clear();
    //m_Deriv.clear();
    m_DirNodeInd=-1;
}

void Model::reset()
{
    clear();
    m_TimeStamp = 0; //vector<float>(TS_LAST, 0);
    m_Flags=0;
    m_PCFlags=PC_NOTHING;
    m_InstCount=1;
    phys.gravitational = phys.gravitational;
    phys.viscousdrag = phys.viscousdrag;
    m_ShapeWeight = __SHAPEWEIGHT;
    setName("00");
    //m_ppmm = DEFAULT_PPMM;
    m_Filename.clear();
}

//! add a Node to the model
//! \returns index in the internal array
int Model::addNode(const Node &node)
{
    if(node.index!=-1 && nodes.size()>(dword)node.index) {
 	TRACE(node.index);
 	nodes[node.index] = node;
 	return node.index;
    }
    int index = nodes.size();
    //nodes[index] = node;
    nodes.push_back(node);
    //nodes.end()->index = index;
    nodes[index].index = index;
    assert(nodes[index].index == index);
    return index;
}

//! add an Edge to the model
//! \returns index in the internal array
int Model::addEdge(const Edge &edge)
{
    int index = edges.size();
    edges.push_back(edge);
    edges[index].index = index;
    edges[index].setNodesList(nodes);
    edges[index].fromNode().edges.push_back(index+1);
    edges[index].toNode().edges.push_back(-index-1);
    return index;
}

//! create an Edge from two given nodes
//! \returns index in the internal array
int Model::addEdge(const Node &from, const Node &to)
{
    int fi = addNode(from);
    int ti = addNode(to);
    return addEdge(Edge(fi,ti,nodes));
}

//! create an Edge using node indices (from nodes - array)
//! \returns index in the internal array
int Model::addEdge(int ifrom, int ito)
{
    assert((dword)ifrom<nodes.size() && 
           (dword)ito<nodes.size() && ifrom>=0 && ito>=0);
    return addEdge( Edge(ifrom, ito, nodes, &phys) );
}

bool Model::readEdge(ParseFile &is)
{
    if(!is.getNextLine()) return false;
    if(is.getKey() != "e") {
	is.pushLine();
	return false;
    }
    istringstream vs(is.getValue());
    vs >> ws;
    int from, to;
    vs >> from;
    from--;
    int size = nodes.size();
    if(from<0 || from>size-1) throw new IOException("Edge is made of node indices that don't exist.");
    vs >> to;
    to--;
    if(to<0 || to>size-1) throw new IOException("Edge is made of node indices that don't exist.");
    Edge &ed = edges[addEdge(from, to)];
    string param;
    while(vs >> param) {
        if(param == "sc") 
            vs >> ed.springconstant;
        else if(param == "dc")
            vs >> ed.dampingconstant;
        else if(param == "rl")
            vs >> ed.restlength;
        else if(param == "es") {
            char ft = 0;
            vs >> ft;
            if(ft == 'f') vs >> ed.edgesensor;
            else if(ft == 't') { vs >> ed.edgesensor; ed.edgesensor *= -1; }
            else {
                is.setParseError("error reading edge parameter [es]");
                throw new IOException("error reading edge parameter [es]");
            }
        } else {
            is.setParseError("error reading edge parameters");
            throw new IOException("error reading edge parameters");
        }
    }
    return true;
}

bool Model::readNode(ParseFile &is)
{
    if(!is.getNextLine()) return false;
    if(is.getKey() != "v") {
	is.pushLine();
	return false;
    }
    istringstream vs(is.getValue());
    vs >> ws;
    Node n(&phys);
    vs >> n.x;
    vs >> n.y;
    n.attachSensor(getZeroSensor());
    string param;
    while(vs >> param) {
        if(param == "s") {
            vs >> param;
            if(m_Sensors) n.attachSensor(m_Sensors->getSensor(param));
            else {
                is.setParseError("tried to load undefined sensor");
                throw new IOException("tried to load undefined sensor");
            }
        } else if(param == "m") 
            vs >> n.mass;
        else {
            is.setParseError("error reading node parameters");
            throw new IOException("error reading node parameters");
        }
    }
    addNode(n);
    //debugline: cout << addNode(n) << " - " << n << endl;
    return true;
}

bool Model::readSensor(ParseFile &is)
{
    if(!is.getNextLine()) return false;
    if(is.getKey() == "sensors") {
        ParseFile sf(is.getPath()+is.getValue());
        if(sf) {
            m_SensorFile = is.getValue();
            while(sf) readSensor(sf);
        } else cerr << "couldn't load sensor file " 
                    << is.getPath()+is.getValue() << endl;
    } else is.pushLine();
    if(!m_Sensors) m_Sensors = new SensorCollection();
    Sensor* s = m_Sensors->readSensor(is);
    if(!s) return false;
    m_Sensors->addSensor(s);
    //s->enableUpdate(Sensor::UPD_MINMAX); done in Node::attachSensor
    return true;
}

bool Model::readParameter(ParseFile &is) 
{
    if(!is.getNextLine()) return false;
    if(is.getKey() != "p") {
	is.pushLine();
	return false;
    }
    istringstream vs(is.getValue());
    string pname;
    vs >> pname;
    if(pname == "mass") {
        vs >> phys.mass;
    } else if(pname == "springconst") {
        vs >> phys.springconst;
    } else if(pname == "grav") {
        vs >> phys.gravitational;
    } else if(pname == "viscousdrag") {
        vs >> phys.viscousdrag;
    } else if(pname == "damping") {
        vs >> phys.damping;
    } else if(pname == "imageforce") {
        vs >> phys.imgforce;
    } else if(pname == "torque") {
        vs >> phys.torque;
    } else if(pname == "name") {
	string name;
        vs >> name;
	setName(name);
    } else if(pname == "ppmm") {
        vs >> m_ppmm;
    } else if(pname == "dirnode") {
        vs >> m_DirNodeInd;
    } 
    return true;
}

ParseFile& operator>>(ParseFile &is, Model &g)
{
    g.reset();
    g.m_Filename = is.getFilename();
    g.phys = ParticleParam::global;
    g.setName("<noname>");
    g.m_ppmm = DEFAULT_PPMM;
    try{
        int phase=0;
        while(is) {
            if(phase == 0) {
                if(!g.readSensor(is) && !g.readParameter(is)) 
                    phase++;
            }
            if(phase == 1) {
                if(!g.readNode(is)) 
                    phase++;
            }
            if(phase == 2) {
                if(!g.readEdge(is)) 
                    phase++;
            }
            if(phase == 3 && !is.eof()) {
                g.reset();
                throw new IOException("Error reading model file.");
            }
        }
    } catch(const IOException *e) {
        const char* omsg = e->getMessage();
        int l=strlen(omsg);
        char msg[l+256];
        sprintf(msg,"%s (interpreted line %i)", omsg, is.getLineNumber());
        throw new IOException(msg);
    }
    g.adaptDataScale();
    g.prepareTorque();
    return is;
}

ostream& operator<<(ostream &os, const Model &g)
{
    os << "p name " << g.m_Name << endl;
    if(g.m_ppmm != DEFAULT_PPMM) 
        os << "p ppmm " << g.m_ppmm << endl;
    os << "p dirnode " << g.m_DirNodeInd << endl;
    os << g.phys;
    os << endl;
    if(!g.m_SensorFile.empty()) {
        ofstream sf((ParseFile::getPath(g.m_Filename)+g.m_SensorFile).c_str());
        sf << *g.m_Sensors;
        os << "sensors " << g.m_SensorFile << endl;
    } else {
        g.m_Sensors->clearPrintList();
        for(NodeArray::const_iterator n = g.nodes.begin(); n != g.nodes.end(); 
            n++)
            n->sensor->hprint(os, (SensorCollection*)g.m_Sensors);
    }
    os << endl;
    for(NodeArray::const_iterator n = g.nodes.begin(); n != g.nodes.end(); n++)
	os << *n;
    os << endl;
    for(EdgeArray::const_iterator e = g.edges.begin(); e != g.edges.end(); e++)
	os << *e;
    return os;
}

bool Model::readFile(const char *filename, bool fullread)
{
    ParseFile is(filename);
    if(is) {
	try {
	    is >> *this;
	} catch (const IOException *e) {
 	    cerr << "IOException: " << e->getMessage() << endl
 		 << "reading file " << filename << endl;
            reset();
	    return false;
	}
	return true;
    } else return false;
}

bool Model::writeFile(const char *filename) const
{
    ofstream os(filename);
    if(os) {
	try {
	    os << "# file '" << filename << "' "<<endl;
	    ParticleParam::global = phys;
	    os << *this;
	} catch (const IOException *e) {
	    cerr << "IOException: " << e->getMessage() << endl
		 << "writing file " << filename << endl;
	    return false;
	}
	os.close();
	return true;
    } else return false;
}

void Model::setName(const string &name) 
{
    m_Name = name;
    m_CRC = (dword)CRC((unsigned char*)m_Name.c_str(), m_Name.size());
}

void Model::setHLNode(int hlnode) {
    if(m_HLNode>=0 && m_HLNode<getNNodes()) 
        getNode(m_HLNode).disableState(Node::ST_HIGHLIGHT);
    m_HLNode = hlnode;
    if(m_HLNode>=0 && m_HLNode<getNNodes())
        getNode(m_HLNode).enableState(Node::ST_HIGHLIGHT);
    else m_HLNode = -1;
}

//! compute directed adjacency matrix
DMatrixf Model::adjMat() const
{
    DMatrixf adjm(getNNodes(), getNNodes(), 0.0f);
    for(EdgeArray::const_iterator e=edges.begin(); e!=edges.end(); e++)
    {
        assert(e->from < getNNodes() && e->to < getNNodes());
        adjm.at(e->from, e->to)++;
    }
    DMatrixf tri(getNNodes(), getNNodes());
    tri.setLower();
    tri*=adjm; tri.transpose();
    adjm += tri;
    adjm.setLower(0);
    return adjm;
}

DMatrixf Model::selMaskMat() const
{
    DMatrixf selmc(getNNodes(), getNNodes());
    DMatrixf selmr(getNNodes(), getNNodes());
    for(NodeArray::const_iterator n = nodes.begin(); n != nodes.end(); n++)
        if(n->hasState(Node::ST_SELECT)) {
            selmr.setRow(n->getIndex());
            selmc.setCol(n->getIndex());
        }
    selmr+=selmc;
    selmc=selmr;
    selmr.min(1);
    return selmc-=selmr;
}

//! remove edged mentioned in (non-directional) adjacency matrix
dword Model::removeByAdjMat(DMatrixf& adjm)
{
    assert(adjm.sizeX() == adjm.sizeY() && adjm.sizeX() == (dword)getNNodes());
    dword count=0;
    EdgeArray::iterator e=edges.begin();
    while(e!=edges.end()) {
        int from,to;
        if(e->from > e->to) { from = e->from; to = e->to; }
        else { from = e->to; to = e->from; }
        assert(e->to < getNNodes());
        if(adjm.at(from, to)>0.0001) {
            edges.erase(e);
            adjm.at(from, to)--;
            count++;
        } else e++;
    }
    if(count) rebuildIndexTables();
    return count;
}

//! create edges mentioned in adjacency matrix
dword Model::createByAdjMat(const DMatrixf& adjm)
{
    assert(adjm.sizeX() == adjm.sizeY() && adjm.sizeX() == (dword)getNNodes());
    dword count=0;
    for(dword j=0; j<adjm.sizeY(); j++)
        for(dword i=0; i<adjm.sizeX(); i++) {
            if(adjm.at(i,j)>0.0001) {
                addEdge(i,j);
                count++;
            }
        }
    return count;
}

//! Connect selected nodes.
dword Model::connectNodes()
{
    DMatrixf selm = selMaskMat();
    selm.setLower(0);
    DMatrixf adjm = adjMat();
    adjm.min(1);
    DMatrixf crem(selm);
    adjm *= selm;
    crem -= adjm;
    dword nc = createByAdjMat(crem);
    if(nc) prepareTorque();
    return nc;
}

//! Remove edges between selected nodes.
dword Model::removeEdges()
{
    DMatrixf selm = selMaskMat();
    selm *= adjMat();
    dword nc = removeByAdjMat(selm);
    if(nc) prepareTorque();
    return nc;
}

//! Removes a node and its adjacent edges.
void Model::removeNode(int nid)
{
    if(!(nid>=0 && nid<getNNodes())) return;
    EdgeArray::iterator e=edges.begin();
    while(e!=edges.end()) {
        if(e->to == nid || e->from == nid) edges.erase(e);
        else {
            if(e->to > nid) e->to--;
            if(e->from > nid) e->from--;
            e++;
        }
    }
    m_DirNodeInd = -1;
    m_HLNode = -1;
    nodes.erase(nodes.begin()+nid);
    rebuildIndexTables();
    prepareTorque();
}

void Model::rebuildIndexTables()
{
    dword idx=0;
    for(NodeArray::iterator n=nodes.begin(); n!=nodes.end(); n++,idx++) {
        n->index = idx;
        n->edges.clear();
    }
    idx=0;
    for(EdgeArray::iterator e=edges.begin(); e!=edges.end(); e++, idx++) {
        e->index = idx;
        e->fromNode().edges.push_back(e->index+1);
        e->toNode().edges.push_back(-e->index-1);
    }
}

void Model::draw(bool drawPoints) const {
    if(!edges.empty()) {
        glBegin(GL_LINES);
        for(EdgeArray::const_iterator e=edges.begin(); e!=edges.end(); e++)
        {
            e->fromNode().glVertex();
            e->toNode().glVertex();
        }
        glEnd();
    }
    if(drawPoints &&  !nodes.empty()) {
	glPointSize(3.0f);
	glEnable(GL_POINT_SMOOTH);
	glColor3f(0,0,1);
	glBegin(GL_POINTS);
	for(NodeArray::const_iterator n=nodes.begin(); n!=nodes.end(); n++)
	{
	    n->glVertex();
	}
	glEnd();
	glDisable(GL_POINT_SMOOTH);
	glPointSize(1.0f);
	for(NodeArray::const_iterator n=nodes.begin(); n!=nodes.end(); n++)
	{
	    if(n->hasState()) n->draw();
	}
    }
}

void Model::drawPS(CAMgraphicsProcess& gp, dword mode) const {
    double x[2], y[2];
    double height  = !m_Dataset || !m_Dataset->initialized() ? 0.0 
        : m_Dataset->getDim2Size();
    for(EdgeArray::const_iterator e=edges.begin(); e!=edges.end(); e++)
    {
        const Node& from = e->fromNode();
        const Node& to = e->toNode();
        x[0] = from.x; x[1] = to.x;
        if(height > 0.0) { y[0] = from.y; y[1] = to.y; }
        else { y[0] = height-from.y; y[1] = height-to.y; }
        gp.plot(x,y,2);
    }
}

void Model::attachSensorColl(SensorCollection* sc)
{
    if(m_Sensors == sc) return;
    if(m_Sensors) m_Sensors->unrefModel(this);
    if(sc) sc->refModel(this);
    m_Sensors = sc;
}

void Model::mergeSensorCollection(SensorCollection *sensors) 
{
    if(!sensors) return;
    if(m_Sensors != NULL) {
        sensors->merge(*m_Sensors);
    }
    m_Sensors = sensors;
}

float Model::getNodeAngle(const Node &n) const {
    if(n.edges.size()>=2) {
	int eind1 = n.edges[0];
	Point e1 = eind1<0 ? edges[-eind1-1].idir() : edges[eind1-1].dir();
	int eind2 = n.edges[1];
	Point e2 = eind2<0 ? edges[-eind2-1].idir() : edges[eind2-1].dir();
	//TRACE(e1<<" "<<e2<<" "<<e1.angle(e2));
	return e1.angle(e2);
    } return 0.0;
}

void Model::reattachSensors() {
    for (NodeArray::iterator p=nodes.begin(); p!=nodes.end();p++) {
	p->attachSensor(m_Sensors->getSensor(p->sensorID));
    }
}

dword Model::createEdgeSensors(float dist)
{
    float smpdist = getStdRadius()*dist;
    if(smpdist < 0.000001) return 0;
    dword nsens=0;
    for(EdgeArray::iterator e=edges.begin(); e!=edges.end(); e++)
    {
        if(e->fromNode().sensor == e->toNode().sensor && e->fromNode().sensor)
        {                       // positive means from-node
            nsens += (e->edgesensor = (int)(e->length()/smpdist));
        }
    }
    return nsens;
}
    
//----------------------------------------------------------------------------
//---- Spring model stuff -----

void Model::resetForces()
{
    Point zero;
    for (NodeArray::iterator p=nodes.begin(); p!=nodes.end();p++) {
	p->f = zero;
    }
}
// Update the forces on each particle
void Model::calculateForces(float dt)
{
   Point down(0.0f, -1.0f);
   Point f;
   float imgf, ifscale;
   if(phys.imgforce == 0) {
       imgf = 0;
       ifscale = 0;
   } else {
       imgf = addImageForces(dt);
       ifscale=1;
   }
   if(imgf>0.0) ifscale = ((float)phys.imgforce/imgf)*(float)nodes.size();
   //TRACE(imgf<< " scale="<<ifscale);
   for (NodeArray::iterator p=nodes.begin(); p!=nodes.end();p++) {
      //if (p->fixed) continue;
       p->f *= ifscale;
       p->f += p->ef/dt;
       p->ef = 0.;
      /* Gravitation */
       if(phys.gravitational > 0.0f)
	   p->f = (down * (phys.gravitational * p->mass));

      /* Viscous drag */
       if(phys.viscousdrag > 0.0f)
	   p->f -= (p->v * phys.viscousdrag);
   }

   /* Handle the spring interaction */
   for (EdgeArray::iterator s=edges.begin(); s!=edges.end(); s++)
   {
       Point d = s->fromNode();
       d -= s->toNode();
       float len = d.norm();
       float len1 = 1/len;
       //spring force
       f = (s->springconstant * (len - s->restlength));
                                        //calc damping force
       Point vd(s->fromNode().v);
       vd -= s->toNode().v;
       d *= len1;                       //spring direction normalized
       //vd.times(d);                     //project velocity difference to spring
       vd = vd.dot(d);                     //project velocity difference to spring
       vd *= s->dampingconstant;
       f += vd;
       f *= (d*-1);

       //if not fixed - maybe
       s->fromNode().f += f;
       s->toNode().f -= f;
   }
}

float Model::addImageForces(float dt) {
    if(!m_Dataset || !m_Dataset->initialized()) return 0.0f;
    if(m_Dataset) const_cast<vuMutex&>(m_Dataset->writeMutex).lock();
    float allf=0;
    for (NodeArray::iterator p=nodes.begin(); p!=nodes.end();p++) {
	p->addSensorForce();
	allf += p->f.norm();
    }
    if(m_Dataset) const_cast<vuMutex&>(m_Dataset->writeMutex).unlock();
    return allf;
}

void Model::prepareTorque() {
    for(NodeArray::iterator p=nodes.begin(); p!=nodes.end();p++) {
	p->oangle = getNodeAngle(*p);
    }
}

void Model::addTorque(float dt) {
  //doesn't really work with a node array other than nodes 
  //(because of edges.fromNode()...
    if(m_Dataset) const_cast<vuMutex&>(m_Dataset->writeMutex).lock();
    //NodeArray &narr = tp ? *tp : nodes;
    for(NodeArray::iterator p=nodes.begin(); p!=nodes.end();p++) {
        if(!p->edges.empty()) {
            float da = mapAnglePI(getNodeAngle(*p) - p->oangle);
            //TRACE(da << " " << p->oangle << " " << getNodeAngle(*p));
            if(da!=0.0) {
                int eind = p->edges[0];
                Edge &e1 = eind<0 ? edges[-eind-1]:edges[eind-1];
                Node &n1 = eind<0 ? e1.fromNode() : e1.toNode();
                Point d1(n1-*p);
                d1.normalize();
                eind = p->edges[1];
                Edge &e2 = eind<0 ? edges[-eind-1]:edges[eind-1];
                Node &n2 = eind<0 ? e2.fromNode() : e2.toNode();
                Point d2(*p-n2);
                d2.normalize();
                Point t1 = d1.flipOrtho()*(da*phys.torque*0.5);
                n1.f -= t1;
                p->f += t1;
                Point t2 = d2.flipOrtho()*(da*phys.torque*0.5);
                n2.f -= t2;
                p->f += t2;
            }
        }
    }
    if(m_Dataset) const_cast<vuMutex&>(m_Dataset->writeMutex).unlock();
}

// Perform one step of the solver
void Model::updateParticles(float dt,int method)
{
    method = getSearchPara().m_ParaSolver;
    if(nodes.empty()) return;
    switch (method) {
        case 0:                                   // Euler
        {
            std::vector<NodeDerivative> m_Deriv(getNNodes());
            resetForces();
            calculateForces(dt);
            addTorque(dt);
            calculateDerivatives(m_Deriv);
            vector<NodeDerivative>::iterator d=m_Deriv.begin();
            for (NodeArray::iterator n=nodes.begin();
                 n != nodes.end(), d != m_Deriv.end(); n++, d++) {
                *n += (d->dpdt*dt);
                n->v += (d->dvdt*dt);
            }
        }
        break;
        case 1:                                   // Midpoint
        {
            int i;
            std::vector<NodeDerivative> m_Deriv(getNNodes());
            resetForces();
            calculateForces(dt);
            addTorque(dt);
            calculateDerivatives(m_Deriv);
            NodeArray ptmp(nodes);
            for (i=0;i<(int)nodes.size();i++) {
                nodes[i] += (m_Deriv[i].dpdt * (dt * 0.5));
                nodes[i].v += (m_Deriv[i].dvdt * (dt * 0.5));
            }
            vector<NodeDerivative> deriv2(getNNodes());
            resetForces();
            calculateForces(dt);
            addTorque(dt);
            calculateDerivatives(deriv2);
            for (i=0;i<(int)nodes.size();i++) {
                nodes[i].setPos(ptmp[i]+
                                ((m_Deriv[i].dpdt+deriv2[i].dpdt)* (0.5*dt)));
                nodes[i].v = ptmp[i].v+
                    ((m_Deriv[i].dvdt+deriv2[i].dvdt)*(0.5*dt));
            }
        }
        break;
        case 2:                                   // latent midpoint
        {
            // the goal of this approach is to use information about
            // the last derivative to get more stable results if step
            // size is increasing. actually, there is no point in using
            // a derivative of a long past position to correct the
            // current derivative for further steps. incorporating the
            // old one makes the method even more instable...
            
            // in the original implementation this is preserved from last time
            std::vector<NodeDerivative> m_Deriv(getNNodes());
            float m_LastDT = 0;
            vector<NodeDerivative> deriv(getNNodes());
            resetForces();
            calculateForces(dt);
            addTorque(dt);
            calculateDerivatives(deriv);
            if(m_LastDT == 0 || deriv.size() != m_Deriv.size()) {
                m_Deriv = deriv;
                m_LastDT = 0;
            }
            //float wnow = dt*m_LastDT / (dt+m_LastDT);
            //float wlast =  dt*dt / (dt+m_LastDT);
            float wnow = 0.9*dt;
            float wlast = dt - wnow;
            int i;
            for (i=0;i<(int)nodes.size();i++) {
                nodes[i] += (m_Deriv[i].dpdt*wlast+deriv[i].dpdt*wnow);
                nodes[i].v += (m_Deriv[i].dvdt*wlast+deriv[i].dvdt*wnow);
            }
            m_Deriv = deriv;
            m_LastDT = dt;
        }
        break;
    }
    invalidatePC();
}

//  Calculate the derivatives
//  dp/dt = v
//  dv/dt = f / m
void Model::calculateDerivatives(vector<NodeDerivative> &deriv)
{
    deriv.resize(nodes.size());
    vector<NodeDerivative>::iterator d = deriv.begin();
    for (NodeArray::iterator n = nodes.begin();
	 d != deriv.end(), n != nodes.end(); 
	 d++, n++) {
	d->dpdt = n->v;
	d->dvdt = n->f;
	d->dvdt *= (1/n->mass);
    }
}

const Point Model::getCenter() const {
    if(!(m_PCFlags&PC_CENTER)) {
        Point c(0,0);
        if(!nodes.empty()) {
            for (NodeArray::const_iterator n = nodes.begin();
                 n != nodes.end(); 
                 n++) 
            {
                c+=*n;
            }
            c *= (1.0f/nodes.size());
        }
        setPropPos(m_PropVec, c);
        m_PCFlags|=PC_CENTER;
    }
    return getPropPos(m_PropVec);
}

const Node& Model::getDirNode() const {
    static const Node dummy;
    if(!getNNodes()) return dummy;
    if(m_DirNodeInd < 0) {
        Point c = getCenter();
        float cdist = -1.;
        dword cind = 0;
        for(NodeArray::const_iterator n=nodes.begin(); 
            n!=nodes.end(); n++, cind++)
        {
            float dist = (*n-c).norm2();
            if(cdist < dist) {
                cdist = dist;
                m_DirNodeInd = cind;
            }
        }
    }
    return getNode(m_DirNodeInd);
}

float Model::getDirection() const {
    if(!(m_PCFlags&PC_DIR)) {
        setPropDir(m_PropVec, nodes.empty() ? 0 : 
                   mapAngle2PI((getDirNode()-getCenter()).angle()) );
        m_PCFlags|=PC_DIR;
    }
    return getPropDir(m_PropVec);
}

//-----------------------------------------------------------------------------
//--- misc functions
void Model::addNoise(float r)
{
  /*
    for(NodeArray::iterator n = nodes.begin(); n != nodes.end(); n++) {
	Point d(r-2*frand(r), r-2*frand(r));
	n->ef += d;
    }
  */
    for(NodeArray::iterator n = nodes.begin(); n != nodes.end(); n++) {
      float minlen=-1;
      for(vector<int>::const_iterator edi=n->edges.begin(); 
	  edi!=n->edges.end(); edi++) {
	float elen = edges[abs(*edi)-1].length();
	if(minlen==-1) minlen = elen;
	else if(minlen>elen) minlen=elen;
      }
      float er = r*minlen;
      Point d(er-2*frand(er), er-2*frand(er));
      n->ef += d;
    }
}

void Model::translate(const Point &t)
{
    for(NodeArray::iterator n = nodes.begin(); n != nodes.end(); n++) {
	*n += t;
    }
    invalidatePC(PC_CENTER|PC_STATS);
}

void Model::push(const Point &t)
{
    for(NodeArray::iterator n = nodes.begin(); n != nodes.end(); n++) {
	n->f += t;
    }
}

void Model::freeze()
{
    for(EdgeArray::iterator e = edges.begin(); e != edges.end(); e++) {
	e->adaptRestLength();
    }
    prepareTorque();
}

void Model::rotate(float angle, const Point &c)
{
    for(NodeArray::iterator n = nodes.begin(); n != nodes.end(); n++) {
	(Point2D&)*n = c+(*n-c).rotate(angle);
    }
    invalidatePC(PC_DIR|PC_CENTER|PC_STATS);
}

void Model::pushRotate(const Point &c, float angle)
{
    for(NodeArray::iterator n = nodes.begin(); n != nodes.end(); n++) {
	n->ef += (*n-c).flipOrtho().copyNormalized()*angle;
    }
}

void Model::attract(const Point &c, float factor)
{
    for(NodeArray::iterator n = nodes.begin(); n != nodes.end(); n++) {
	n->ef -= (*n-c)*factor;
    }
}

float Model::getLengthRatio() const {
    float lengths=0,restlengths=0;
    for(EdgeArray::const_iterator e = edges.begin(); e != edges.end(); e++) {
	lengths+=e->length();
	restlengths+=e->restlength;
    }
    return lengths/restlengths;
#ifdef NOTLENGTHWEIGHTED
    float lenrat=0;
    for(EdgeArray::const_iterator e = edges.begin(); e != edges.end(); e++) {
	lenrat += e->lengthRatio();
    }
    return lenrat/(float)edges.size();
#endif
}

float Model::getLengthVariation() const {
    float vlengths=0,restlengths=0;
    for(EdgeArray::const_iterator e = edges.begin(); e != edges.end(); e++) {
	register float dl = e->restlength-e->length();
	vlengths+=dl*dl;
	restlengths+=e->restlength;
    }
    register float n = edges.size();
    return sqrt(vlengths*n)/(restlengths);
}

float Model::getDeformation() const {
    if(edges.empty()) return 0.0;
    float vlengths=0,restlengths=0;
    float scale = 1/getLengthRatio();
    float scscale = 1/ParticleParam::global.springconst;
    for(EdgeArray::const_iterator e = edges.begin(); e != edges.end(); e++) {
	register float dl = e->restlength-e->length()*scale;
        //dl *= e->springconstant*scscale; // weight by relative springconst
	vlengths+=dl*dl;
	restlengths+=e->restlength;
    }
    register float n = edges.size();
    return sqrt(vlengths*n)/(restlengths);
}

float Model::getFullLength() const {
    float length=0;
    for(EdgeArray::const_iterator e = edges.begin(); e != edges.end(); e++) {
	length += e->length();
    }
    return length;
}

float Model::getStdRadius(const Point& center) const {
    if(m_PCFlags&PC_SCALE) return getPropScale(m_PropVec);
    float dist = 0;
    if(!nodes.empty()) {
        for(NodeArray::const_iterator n = nodes.begin(); n != nodes.end(); n++)
            dist += (*n-center).norm2();
        dist /= (float)nodes.size();
        dist = sqrt(dist);
    }
    setPropScale(m_PropVec, dist);
    m_PCFlags|=PC_SCALE;
    return dist;
}

float Model::getMaxRadius(const Point& center) const {
    if(nodes.empty()) return 0;
    float dist = 0;
    for(NodeArray::const_iterator n = nodes.begin(); n != nodes.end(); n++) {
        register float td = (*n-center).norm2();
	if(dist<td) dist=td;
    }
    return sqrt(dist);
    //return (center-getDirNode()).norm();
}

float Model::distance(const Model& rhs, enum Model::DistType kind) const
{
    switch(kind) {
        case DIST_XY:
            return (getCenter() - rhs.getCenter()).norm();
            break;
        case DIST_XYS:
        {
            float scaledist = getStdRadius() - rhs.getStdRadius();
            return sqrt((getCenter() - rhs.getCenter()).norm2() 
                        + scaledist*scaledist);
        }
        break;
        case DIST_PVEC:
        {
            PropVec prop = getProperties();
            PropVec rhsprop = rhs.getProperties();
            Point2D sd = getPropSDir(prop);
            Point2D rhssd = getPropSDir(rhsprop);
            prop[2] = sd.x; prop[3] = sd.y;
            rhsprop[2] = rhssd.x; rhsprop[3] = rhssd.y;
            return (prop - rhsprop).norm();
        }
        break;
        case DIST_POINTS:
            if(getNNodes() != rhs.getNNodes()) 
                return std::numeric_limits<float>::max();
            else {
                float dist=0;
                for(NodeArray::const_iterator thisn = nodes.begin(),
                        rhsn = rhs.nodes.begin(); 
                    thisn != nodes.end(); thisn++, rhsn++)
                {
                    dist += (*thisn-*rhsn).norm2();
                }
                return sqrt(dist);
            }
            break;
        case DIST_HPOINTS:      // Chamfer distance
        case DIST_CPOINTS:      // Hausdorff distance
        {
            if(getNNodes()==0 || rhs.getNNodes()==0) 
                return std::numeric_limits<float>::max();
            double dist=0;
            for(NodeArray::const_iterator thisn = nodes.begin(); 
                thisn != nodes.end(); thisn++)
            {
                float mindist = std::numeric_limits<float>::max();
                for(NodeArray::const_iterator rhsn = rhs.nodes.begin();
                    rhsn != rhs.nodes.end(); rhsn++)
                {
                    register float d = (*thisn-*rhsn).norm2();
                    if(d<mindist) mindist = d;
                }
                if(kind == DIST_HPOINTS && mindist>dist) dist=(double)mindist;
                if(kind == DIST_CPOINTS ) dist += sqrt((double)mindist);
            }
            if(kind == DIST_HPOINTS) dist = sqrt(dist);
            if(kind == DIST_CPOINTS) dist /= getNNodes();
            return float(dist);
        }
    }
    return 0;
}

int Model::nearestNode(const Point& pos, float& dist) const
{
    if(nodes.empty()) return -1;
    int rid=0, cid=0;
    float tdist;
    float& mindist = &dist ? dist : tdist;
    rid = 0;
    mindist = std::numeric_limits<float>::max();
    for(NodeArray::const_iterator ni = nodes.begin();
        ni != nodes.end(); ni++, cid++)
    {
        register float d = (pos-*ni).norm2();
        if(d<mindist) { mindist = d; rid = cid; }
    }
    mindist = sqrt(mindist);
    return rid;
}

void Model::adaptRestLength(float ratio)
{
    for(EdgeArray::iterator e = edges.begin(); e != edges.end(); e++) {
	e->adaptRestLength(ratio);
    }
}

void Model::adaptRestLengthSel(float ratio, const DMatrixf& selm)
{
    for(EdgeArray::iterator e = edges.begin(); e != edges.end(); e++) {
	e->adaptRestLength(ratio);
    }
}

void Model::scale(float factor, bool movepoints)
{
    for(EdgeArray::iterator e = edges.begin(); e != edges.end(); e++) {
	e->restlength*=factor;
    }
    if(movepoints) {
        Point center = getCenter();
        for(NodeArray::iterator n = nodes.begin(); 
            n != nodes.end(); n++) 
        {
            n->setPos(center + ((*n-center)*factor));
        }
        invalidatePC(PC_SCALE|PC_STATS);
    }
}

void Model::scaleSel(float factor, const DMatrixf& selm)
{
    for(EdgeArray::iterator e = edges.begin(); e != edges.end(); e++) {
        if(selm.at(e->from, e->to) > 0.001)
            e->restlength*=factor;
    }
}

void Model::adaptProportion(float ratio)
{
    float corrat = getLengthRatio();
    corrat -= (corrat-1)*(1-ratio);
    for(EdgeArray::iterator e = edges.begin(); e != edges.end(); e++) {
	e->restlength *= corrat;
    }
}

float Model::getSensorFit(dword* npts) const
{
    if(nodes.empty()) { if(npts) *npts = 0; return 0.0; }
    float fit = 0;
    dword nsns = 0;
    for(NodeArray::const_iterator n = nodes.begin(); n != nodes.end(); n++)
        if(n->sensor) {
            float i=n->sensor->getWeightedValue((int)n->x,(int)n->y);
            //float gm = n->sensor->getGradient(*n).norm();
            //float vel = n->v.norm();
            fit += i;
            nsns++;
        }
    if(nsns) fit /= (float)nsns;
    if(npts) *npts = nsns;
    return fit;
}

float Model::getEdgeSensorFit(dword* npts) const
{
    if(edges.empty()) { if(npts) *npts = 0; return 0.0; }
    float fit = 0;
    dword npoints = 0;
    for(EdgeArray::const_iterator e = edges.begin(); e != edges.end(); e++) {
	float i=e->getSensorValue(fit, npoints);
    }
    if(npoints) fit /= (float)npoints;
    if(npts) *npts = npoints;
    return fit;
}

float Model::getQualityOfFit() const
{
    if(m_PCFlags&PC_QOF) return m_QOF;
    if(nodes.empty() || edges.empty()
       || !m_Dataset || !m_Dataset->initialized()) m_QOF=0;
    else {
        dword nns=0, nes=0;
        float ns = getSensorFit(&nns);           // node sensors
        float es = getEdgeSensorFit(&nes);       // edge sensors
        m_QOF = (es*nes+ns*nns)/float(nns+nes);
        //DEBUG: use deformation*10?
        register float lv = getDeformation(); //getLengthVariation();
        m_QOF = (1-m_ShapeWeight)*m_QOF + (m_ShapeWeight)*exp(-5*abs(lv));
    }
    m_PCFlags|=PC_QOF;
    return m_QOF;
}

float Model::getLiveliness() const
{
    if(m_PCFlags&PC_LIVE) return m_Liveliness;
    m_Liveliness = 0;
    if(!nodes.empty() && !edges.empty()) {
        float dist= 0;
        Point c = getCenter();
        for(NodeArray::const_iterator n = nodes.begin(); n != nodes.end(); n++)
        {
            m_Liveliness += n->v.norm();
            dist += (c-*n).norm2();
        }
        if(dist > 0.00001) m_Liveliness /= sqrt(dist);
    }
    m_PCFlags|=PC_LIVE;
    return m_Liveliness;
}

void Model::adaptDataScale(dword ppmm) {
    if(!ppmm) {
        if(!m_Dataset) return;
        if(!(ppmm = m_Dataset->getPPMM())) return;
    }
    if(m_ppmm) scale(float(ppmm)/m_ppmm, true);
    m_ppmm = ppmm;
}

void Model::adaptProperties(const PropVec &prop) {
    if(nodes.empty()) return;
    Point2D nc = getPropPos(prop);
    float ndir = getPropDir(prop);
    float nscale = getPropScale(prop);
    Point center = getCenter();
    float cscale = getStdRadius();
    float cdir = getDirection();
    rotate(ndir-cdir);
    if(nscale>0.001 && cscale>0.0000001)
        scale(nscale/cscale, true);
    translate(nc-center);
}

const PropVec& Model::getProperties() const
{
    if((m_PCFlags&PC_PVEC) != PC_PVEC) {
        if((m_PCFlags&PC_CENTER) == 0) getCenter(); //setPropPos(m_PropVec,..
        if((m_PCFlags&PC_SCALE) == 0)  getStdRadius();
        if((m_PCFlags&PC_DIR) == 0)    getDirection();
    }
    return m_PropVec;
}

//! return property vector using millimeter scale
PropVec Model::getPropertiesMM() const
{
    PropVec prop(getProperties());
    convertPropToMM(prop);
    return prop;
}
//! adapt to a property vector using millimeter scale
void Model::adaptPropertiesMM(PropVec prop)
{
    adaptProperties(convertPropFromMM(prop));
}
Point2D& Model::convertPointFromMM(Point2D& pt) const
{
    if(m_ppmm) pt *= (float)m_ppmm;
    if(m_Dataset) pt += m_Dataset->getOrigin();
    return pt;
}
Point2D& Model::convertPointToMM(Point2D& pt) const
{
    if(m_Dataset) pt -= m_Dataset->getOrigin();
    if(m_ppmm) pt /= (float)m_ppmm;
    return pt;
}
PropVec& Model::convertPropFromMM(PropVec& prop) const
{
    Point2D pos(getPropPos(prop));
    setPropPos(prop, convertPointFromMM(pos));
    if(m_ppmm) setPropScale(prop, getPropScale(prop)*m_ppmm);
    return prop;
}
PropVec& Model::convertPropToMM(PropVec& prop) const
{
    Point2D pos(getPropPos(prop));
    setPropPos(prop, convertPointToMM(pos));
    if(m_ppmm) setPropScale(prop, getPropScale(prop)/m_ppmm);
    return prop;
}

dword Model::mergeModel( Model& mergemod)
{
    if(&mergemod != this && !mergemod.hasFlags(Model::ST_DEL)) {
        m_InstCount += mergemod.m_InstCount;
        mergemod.m_InstCount=0;
        bool mergeID = false;
        if(m_TimeStamp > mergemod.m_TimeStamp) {
            m_TimeStamp = mergemod.m_TimeStamp;
            mergeID = mergemod.m_ID != 0;
        } else mergeID = (m_ID==0 && mergemod.m_ID != 0);
        if(mergeID) {
            m_ID = mergemod.m_ID;
            mergemod.m_ID = 0;
            if(mergemod.hasFlags(Model::ST_NODEL)) {
                setFlags(Model::ST_NODEL);
            }
        }
        mergemod.setFlags(Model::ST_DEL);
    }
    return m_InstCount;
}

dword Model::getSelectedNodesN(dword state) const
{
    dword count=0;
    for(NodeArray::const_iterator n = nodes.begin(); n != nodes.end(); n++)
        if(n->hasState(state)) count++;
    return count;
}

