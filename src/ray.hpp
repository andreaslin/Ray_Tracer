#ifndef CS488_RAY_HPP
#define CS488_RAY_HPP

#include "algebra.hpp"
#include <limits>
#include "primitive.hpp"
#include <list>

class Material;
class GeometryNode;
class Primitive;

/******************************************************************
 *** Class definition of ray and intersection that will be used ***
 *** during the ray tracing 									***
 ******************************************************************/

class Ray {
public:
	// Constructors
	Ray() : start_point( Point3D() ), direction( Vector3D() ) {}
	Ray(const Point3D &s, const Vector3D&d ) : start_point(s), direction(d) {}

	Point3D start_point;		// Starting point of the ray
	Vector3D direction;			// Direction it's going to travel

};

class Intersection {
public:
	// Constructors
	Intersection() { 
		// NOTE: Remember to initialize the distance or meshing will not work!!
		distance = std::numeric_limits<double>::max(); 
		nearest = NULL;
		primitive = NULL;
		double maximum = std::numeric_limits<double>::max();
		interval[0] = Point3D( -maximum, -maximum, -maximum );
		interval[1] = Point3D( maximum, maximum, maximum );
	}
	Vector3D normal;
	Point3D intersect;
	double distance;			// NOTE: This is not the real distance but the coefficient
								//       for calculating the distance
	Vector3D incoming;			// Incoming direction for photon mapping
	Material *material;
	GeometryNode *nearest;
	Primitive *primitive;
	Point3D interval[2];
};

// NEW photon class
class Photon {
public:
	Photon() : colour( 0.0 ) {}
	Point3D final_pos;			// Finall position of photon
	Vector3D incoming_dir;		// Incoming Direction
	Colour colour;				// Colour of the photon
	
};

#endif
