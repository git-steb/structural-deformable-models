#ifndef _PARTPARAM_H_
#define _PARTPARAM_H_

#include <iostream>

//spring model parameters
#define PART_SPRINGCONST	50.0f
#define PART_DAMPING		5.0f
#define PART_MASS		1.0f
#define PART_GRAV		0.0f
#define PART_VISCOUS		2.0f
#define PART_IMGFORCE           70.0f
#define PART_TORQUE		100.0f

/* old set
#define PART_SPRINGCONST	0.1f
#define PART_DAMPING		0.1f
#define PART_MASS		0.05f
#define PART_GRAV		0.0f
#define PART_VISCOUS		0.1f
#define PART_IMGFORCE           1.0f
#define PART_TORQUE		5.0f
*/

class ParticleParam {
public:
    ParticleParam() {
        gravitational = PART_GRAV;
        viscousdrag = PART_VISCOUS;
        springconst = PART_SPRINGCONST;
        damping = PART_DAMPING;
        mass = PART_MASS;
        imgforce = PART_IMGFORCE;
        torque = PART_TORQUE;
    }

    friend std::ostream& operator<<(std::ostream &os, const ParticleParam &p) {
	if(p.springconst == PART_SPRINGCONST)
	    os << "#";
        os << "p springconst " << p.springconst << std::endl;
	if(p.damping == PART_DAMPING)
	    os << "#";
        os << "p damping     " << p.damping << std::endl;
	if(p.mass == PART_MASS)
	    os << "#";
        os << "p mass        " << p.mass << std::endl;
	if(p.gravitational == PART_GRAV)
	    os << "#";
        os << "p grav        " << p.gravitational << std::endl;
	if(p.viscousdrag == PART_VISCOUS)
	    os << "#";
        os << "p viscousdrag " << p.viscousdrag << std::endl;
	if(p.imgforce == PART_IMGFORCE)
	    os << "#";
        os << "p imageforce  " << p.imgforce << std::endl;
	if(p.torque == PART_TORQUE)
	    os << "#";
        os << "p torque      " << p.torque << std::endl;
        return os;
    }

    float gravitational;
    float viscousdrag;
    float springconst;
    float damping;
    float mass;
    float imgforce;
    float torque;

    //static member needs to be initialized somewhere so the linker
    //doesn't generate 'undefined reference'. This is done in Model.cpp
    static ParticleParam global;
};

#endif
