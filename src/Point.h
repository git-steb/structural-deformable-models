/* -*- C++ -*- */
#ifndef _POINT2D_H_
#define _POINT2D_H_

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <math.h>
#include <iostream>
//#include <cmath>
#include "common.h"
#include "mathutil.h"

//2-dimensional point and vector
class Point2D {
public:
    Point2D() : x(0), y(0) {};
    Point2D(const Point2D & rhs) {
        operator=(rhs); }
    Point2D(float _x, float _y) : x(_x), y(_y) {}
    Point2D(const float *val) {
        operator=(val); }

    virtual ~Point2D() {};

    Point2D& operator=(const Point2D& rhs) {
        x=rhs.x; y=rhs.y;
        return *this;
    }

    Point2D& operator=(float rhs) {
        x = y = rhs;
        return *this;
    }

    Point2D& operator=(const float *rhs) {
        this->x = rhs[0]; this->y = rhs[1];
        return *this;
    }
    Point2D& operator+=(const Point2D& rhs) {
        x+=rhs.x; y+=rhs.y;
        return *this;
    }
    Point2D& operator-=(const Point2D& rhs) {
        x-=rhs.x; y-=rhs.y;
        return *this;
    }

    Point2D& operator*=(const Point2D& rhs) {
        x*=rhs.x; y*=rhs.y;
        return *this;
    }

    Point2D& operator*=(const float f) {
        x*=f; y*=f;
        return *this;
    }

    Point2D& operator/=(const Point2D &rhs) {
        x/=rhs.x; y/=rhs.y;
        return *this;
    }
    Point2D& operator/=(const float f) {
        x/=f; y/=f;
        return *this;
    }

    float operator*(const Point2D& rhs) const {
        return x*rhs.x + y*rhs.y;
    }

    const Point2D& const_times(const Point2D& rhs) const {
        Point2D p(*this);
        return p.times(rhs);
    }

    Point2D& times(const Point2D& rhs) {
        return *this *= rhs;
    }

    float dot(const Point2D& rhs) const {
        return x*rhs.x+y*rhs.y;
    }
    float crossZ(const Point2D& rhs) const {
        return y*rhs.x-x*rhs.y;
    }
    float sinAngle(const Point2D& rhs) const {
        // z-component of the cross product
        return (x*rhs.y-y*rhs.x)/sqrt(norm2()*rhs.norm2());
    }

    float angle(const Point2D& rhs=Point2D(1,0)) const {
        float dv = crossZ(rhs);
        float dh = dot(rhs);
        return atan2(dv,dh);
#ifdef _THERE_IS_NO_ATAN2_
        float dvp,dvn;
        if(dv > 0.0) {
            dvp = 1.0;
            dvn = 0.0;
        } else {
            dvp = 0.0;
            dvn = 1.0;
        }
        float dhn = (dh < 0) ? 1.0 : 0.0;
        float psi;
        if (dh == 0.0)
            psi = M_PI_2 * (dvp + 3.0 * dvn);
        else if (dv == 0.0)
            psi = M_PI * dhn;
        else
            psi = atan(dv/dh) + M_PI * dhn * dvp
                + (M_PI + M_PI * (dh > 0.0)) * dvn;
        return psi * (180.0*M_1_PI);
#endif
    }

    float sinAngleX() const {
        return -y/norm();
    }

    float angleX() const {
        //return asin(sinAngleX());
        return angle(Point2D(1,0));
    }


    Point2D operator*(float rhs) const {
        Point2D p(*this);
        p *= rhs;
        return p;
    }

    Point2D operator+(const Point2D &rhs) const {
        Point2D p(*this);
        p += rhs;
        return p;
    }

    Point2D operator-(const Point2D &rhs) const {
        Point2D p(*this);
        p -= rhs;
        return p;
    }

    Point2D operator/(const Point2D &rhs) const {
        Point2D p(*this);
        p /= rhs;
        return p;
    }

    Point2D operator/(float rhs) const {
        Point2D p(*this);
        p *= 1/rhs;
        return p;
    }

    //! returns 1-norm
    float sum() const {
        return x+y;
    }

    //! returns squared 2-norm
    float norm2() const {
        return x*x+y*y;
    }

    //! returns 2-norm aka length or absolute
    float norm() const {
        return (float)sqrt(x*x+y*y);
    }

    //! normalizes the vector; returns old norm
    float normalize() {
        float n = norm();
        if (n>0.0f) operator*=(1/n);
        return n;
    }

    bool clamp(float x0,float y0,float x1,float y1)
        {
            return ::clamp(x,y,x0,y0,x1,y1);
        }

    friend std::ostream& operator<<(std::ostream &os, const Point2D &p) {
        os << p.x << " " << p.y;
        return os;
    }

    friend std::istream& operator>>(std::istream &is, Point2D &p) {
        is >> p.x;
        is >> p.y;
        return is;
    }

    //! Issue a glVertex2f call.
    void glVertex() const {
        glVertex2f(x, y);
    }

    const Point2D flipOrtho() const {
        return Point2D(-y,x);
    }

    const Point2D copyNormalized() const {
        Point2D p(*this);
        p.normalize();
        return p;
    }
    const Point2D rotate(float angle) const {
        float ca = cos(angle);
        float sa = sin(angle);
        return Point2D(x*ca-y*sa,x*sa+y*ca);
    }
    friend bool operator==(const Point2D& lhs, const Point2D& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }
    friend bool operator!=(const Point2D& lhs, const Point2D& rhs) {
        return !operator==(lhs, rhs);
    }

public:
    float x,y;
};

//2D point with index
class IPoint2D : public Point2D
{
public:
    IPoint2D() : Point2D() {};
    IPoint2D(int _i, float _x, float _y) : Point2D(_x, _y), index(_i) {}
    IPoint2D(int _i, const Point2D& p) {
        operator=(p);
        index = _i;
    }
    IPoint2D& operator= (const IPoint2D & rhs) {
        Point2D::operator=(rhs);
        index = rhs.index;
        return *this;
    }
    IPoint2D& operator= (const Point2D & rhs) {
        Point2D::operator=(rhs);
        index = -1;
        return *this;
    }
public:
    int index;
};

typedef Point2D Point;

#endif
