/* -*- C++ -*- */
#ifndef _NODE_H_
#define _NODE_H_

#include <iostream>
#include <vector>
#include "Point.h"
#include "PartParam.h"

#include "types_fwd.h"

//! Implements a Node used by Model and Edge
class Node : public Point {
    friend class Model;
public:
    enum NodeState { ST_NONE=0, ST_HIGHLIGHT=1, ST_SELECT=2 };

    //! Default constructor
    Node(const ParticleParam *phys = &ParticleParam::global);
    //! Assignment constructor
    Node(const Node & rhs) { operator=(rhs); }
    //! Constructor
    Node(float _x, float _y, const ParticleParam *phys=&ParticleParam::global);
    //! Destructor
    virtual ~Node() {};
    //! Assignment operator
    Node& operator=(const Node &rhs);
    Node& setPos(const Point& p) {
        Point::operator=(p);
        return *this;
    }
    //! \returns the index in Model.nodes array
    int getIndex() const { return index; }
    //! output operator
    friend std::ostream& operator<<(std::ostream &os, const Node &n);
    void attachSensor(sensor_cptr _sensor);
    void addSensorForce();
    void enableState(dword nstate) { state |= nstate; }
    void disableState(dword nstate=0xffffffff) { state &= ~nstate; }
    void toggleState(dword nstate) { state ^= nstate; }
    bool hasState(dword nstate=0xffffffff) const { return state & nstate; }
    void draw() const;
    sensor_cptr sensor;
    std::string sensorID;

public:
    //! Mass
    float mass;

protected:
    //! Velocity and Force
    Point v, f;
    //! extern force
    Point ef;
    //! Index in the list (Model.nodes)
    int index;
    float oangle;
    float cangle;
    float nedges;
    std::vector<int> edges;
    dword state;
};

#endif
