#include "Sensor.h"
#include "Node.h"
#include "glutils.h"

using namespace std;

Node::Node(const ParticleParam *phys) 
    : Point(0,0), sensor(getZeroSensor()),
      mass(phys->mass), index(-1), state(ST_NONE) 
{
}

//! Constructor
Node::Node(float _x, float _y, const ParticleParam *phys) 
    : Point(_x, _y), sensor(getZeroSensor()), mass(phys->mass), 
      index(-1), state(ST_NONE) 
{
}

//! Assignment operator
Node& Node::operator=(const Node &rhs) {
    if(&rhs == this) return *this;
    Point::operator=(rhs);
    mass = rhs.mass;
    f = rhs.f;
    v = rhs.v;
    index = rhs.index;
    sensor = rhs.sensor;
    edges = rhs.edges;
    oangle = rhs.oangle;
    sensorID = rhs.sensorID;
    ef = 0.f;
    state = rhs.state;
    return *this;
}

//! output operator
ostream& operator<<(ostream &os, const Node &n) {
    os << "v ";
    operator<<(os, Point(n));
    if(n.sensor) {
        os << " s " << n.sensor->getID();
    }
    if(n.mass != ParticleParam::global.mass)
        os << " m " << n.mass;
    os << endl;
    return os;
}

void Node::attachSensor(const Sensor *_sensor) {
    sensor = _sensor;
    sensorID = sensor->getID();
    if(!((Sensor*)sensor)->isUpdate(Sensor::UPD_MINMAX)) {
        ((Sensor*)sensor)->enableUpdate(Sensor::UPD_MINMAX);
        ((Sensor*)sensor)->setModified(Sensor::UPD_MINMAX);
    }
    const PPSensor *pps = dynamic_cast<const PPSensor*>(_sensor);
    if(pps && pps->getPPState()==PPSensor::PP_DONT)
        ((PPSensor*)pps)->togglePP(PPSensor::PP_DO);
}

void Node::addSensorForce() {
    if(sensor) {
        Point sf = sensor->getWeightedGradient((int)x,(int)y);
        f += sf;
    }
}

void Node::draw() const {
    if(state) {
        const int HLSIZE = 5;
        if(hasState(ST_HIGHLIGHT)) {
            glBegin(GL_QUADS); {
                glColor4f(1,1,0,0.5);
                glVertex2f(x-HLSIZE, y-HLSIZE);
                glVertex2f(x-HLSIZE, y+HLSIZE);
                glVertex2f(x+HLSIZE, y+HLSIZE);
                glVertex2f(x+HLSIZE, y-HLSIZE);
            } glEnd();
            sglBitmapStringOutlined(sensorID.c_str(), (int)x, (int)y);
        }
        if(hasState(ST_SELECT)) {
            glBegin(GL_LINE_STRIP); {
                glColor4f(1,0,1,1);
                glVertex2f(x-HLSIZE, y-HLSIZE);
                glVertex2f(x-HLSIZE, y+HLSIZE);
                glVertex2f(x+HLSIZE, y+HLSIZE);
                glVertex2f(x+HLSIZE, y-HLSIZE);
                glVertex2f(x-HLSIZE, y-HLSIZE);
            } glEnd();
        }
    }
}
