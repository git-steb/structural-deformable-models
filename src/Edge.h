/* -*- C++ -*- */
#ifndef _EDGE_H_
#define _EDGE_H_

#include <iostream>
#include <math.h>
#include "PartParam.h"
#include "Node.h"
#include "Sensor.h"
#include "Errors.h"

class Model;
//! Implements an Edge with spring functionality
class Edge {
    friend class Model;
 public:
//     Edge() {
// 	from = -1;
// 	to = -1;
// 	restlength = 0;
// 	springconstant = PART_SPRINGCONST;
// 	dampingconstant = PART_DAMPING;
// 	nodes = NULL;
//     }
    Edge(const Edge &rhs ) {
	operator=(rhs);
    }
    Edge(int _from, int _to, std::vector<Node> &_nodes, 
         const ParticleParam *phys = &ParticleParam::global, 
         int _index=-1) {
	from = _from;
	to = _to;
	nodes = &_nodes;
	adaptRestLength();
	springconstant = phys->springconst;
	dampingconstant = phys->damping;
        edgesensor = 0;
	index = _index;
    }
    Edge(int _from, int _to, std::vector<Node> &_nodes,
	 float restlength, float spring_k=1, int _index=-1) {
	from = _from;
	to = _to;
	nodes = &_nodes;
	this->restlength = restlength;
	this->springconstant = spring_k;
        edgesensor = 0;
	this->index = _index;
    }

    virtual ~Edge() {}

    void setNodesList(std::vector<Node> &nodes) {
	this->nodes = &nodes;
    }

    Edge& operator=(const Edge &rhs) {
        if(&rhs == this) return *this;
	from = rhs.from;
	to = rhs.to;
	nodes = rhs.nodes;
	restlength = rhs.restlength;
	springconstant = rhs.springconstant;
	dampingconstant = rhs.dampingconstant;
        edgesensor = rhs.edgesensor;
	index = rhs.index;
	return *this;
    }

    const Point dir() const {
	if(from==-1 || to==-1) return Point();
	return toNode()-fromNode();
    }
    const Point idir() const {
	if(from==-1 || to==-1) return Point();
	return fromNode()-toNode();
    }

    float length2() const {
	if(from==-1 || to==-1) return 0;
	return dir().norm2();
    }

    float length() const {
	return (float)sqrt(length2());
    }

    float lengthRatio() const {
	return length()/restlength;
    }

    float adaptRestLength() {
	return restlength = length();
    }
 
    float adaptRestLength(float ratio) {
	return restlength -= (restlength-length())*ratio;
    }
    float getAngle() const {
	return dir().angleX();
    }
    float adaptAngle() {
	return xangle = getAngle();
    }
    friend std::ostream& operator<<(std::ostream &os, const Edge &p) {
	os << "e " << p.from+1 << " " << p.to+1; 
        if(p.springconstant != ParticleParam::global.springconst)
	    os << " sc " << p.springconstant;
	if(p.dampingconstant != ParticleParam::global.damping)
	    os << " dc " << p.dampingconstant; 
	if(p.length() != p.restlength) os << " rl " << p.restlength;
	if(p.edgesensor>0) os << " es f " << p.edgesensor;
	if(p.edgesensor<0) os << " es t " << -p.edgesensor;
	os << std::endl;
	return os;
    }

    Node& fromNode() { return (*nodes)[from]; }
    Node& toNode() { return (*nodes)[to]; }
    const Node& fromNode() const { return (*nodes)[from]; }
    const Node& toNode() const { return (*nodes)[to]; }

    dword getSensorValue(float& sum, dword& nsamples) const;
 public:
    int from, to;
    float xangle; // angle to x-axis
    float cxangle; // current angle to x-axis
    std::vector<Node> *nodes;
    float restlength; // consider using a restlength2 instead
    //! spring constant
    float springconstant;
    //! damping constant
    float dampingconstant;
    int   edgesensor; //!< 0-none, >0-use from sensor n times, <0 use to sensor

 protected:
    //! index in the model (Model.edges)
    int index;
};

inline dword Edge::getSensorValue(float& sum, dword& nsamples) const
{
    if(!edgesensor) return 0;
    const Node& from = fromNode();
    const Node& to = toNode();
    const Sensor* sensor = edgesensor > 0 ? from.sensor : to.sensor;
    if(!sensor) return 0;
    dword ntimes = edgesensor > 0 ? dword(edgesensor) : dword(-edgesensor);
    Point x(from);
    Point xinc(to); xinc -= from; xinc /= float(ntimes+1);
    x += xinc;
    for(dword i=0; i<ntimes; i++, x+=xinc) {
        sum += sensor->getWeightedValue((int)x.x, (int)x.y);
    }
    nsamples += ntimes;
    return ntimes;
}

#endif
