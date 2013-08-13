#ifndef CS488_A5_HPP
#define CS488_A5_HPP

#include <string>
#include "algebra.hpp"
#include "scene.hpp"
#include "light.hpp"
#include "image.hpp"
#include <pthread.h>
#include <stdlib.h>				// rand
#include <time.h>				// time
#include <vector>				// vector
#include <assert.h>

// The source code of kd-tree is from
// http://code.google.com/p/kdtree/
#include "kdtree.h"

// For determining the random position of ray at each cell during anti-aliasing
#ifndef STOCHASTIC_OFFSET
#define STOCHASTIC_OFFSET 1000
#endif

#ifndef CONSTANT_E
#define CONSTANT_E 2.718281828459045235
#endif

#ifndef SQUARE_ROOT_TWO_PI
#define SQUARE_ROOT_TWO_PI 2.5066282746310005024157
#endif

#ifndef NUM_PHOTONS
#define NUM_PHOTONS 100000
#endif

extern int AA_value;
extern int MAX_RECURSIVE_LEVEL;
extern bool PHOTON_MAPPING;
extern int NUM_THREADS;

void a5_render(// What to render
               SceneNode* root,
               // Where to output the image
               const std::string& filename,
               // Image size
               int width, int height,
               // Viewing parameters
               const Point3D& eye, const Vector3D& view,
               const Vector3D& up, double fov,
               // Lighting parameters
               const Colour& ambient,
               const std::list<Light*>& lights
               );

class Renderer {
public:
	// Constructor
	Renderer( // What to render
               SceneNode* root,
               // Where to output the image
               const std::string& filename,
               // Image size
               int width, int height,
               // Viewing parameters
               const Point3D& eye, const Vector3D& view,
               const Vector3D& up, double fov,
               // Lighting parameters
               const Colour& ambient,
               const std::list<Light*>& lights );
	// Render function
	void render( unsigned int id );

	// For anti-aliasing
	struct Weight {
		double distance;
		Colour colour;
		Weight( double d, Colour c ) : distance( d ), colour( c ) {} 
	};

	// Photon mapping
	void mapping( unsigned int id );

private:
	SceneNode *root;
	std::string filename;
	int width, height;
	double m_width, m_height, fov;
	Point3D eye;
	Vector3D view, up;
	Colour ambient;
	std::list<Light*> lights;
	Vector3D w, u, v;
	double m_fov, focal;

	// For displaying ray tracing percentage
	unsigned int total_pixel;
	unsigned int proceed_pixel;
	unsigned int percent;	
	unsigned int height_offset;

	// For photon mapping
	unsigned int photon_offset;
	unsigned int total_intensity;
	
	Image *img;

	// Photon map
#ifndef KDTREE
	std::list<Photon> photon_map;
#endif
#ifdef KDTREE
	kdtree *photon_map;
#endif

	// Destructor
	virtual ~Renderer();
	void checkLighting( Ray &ray, Intersection &intersection, Colour &colour );
	Colour rayTrace( Ray &ray, Colour colour, unsigned int level_count, Intersection &intersection, bool is_photon );
	void percentDone();
};

struct Parameters {
	unsigned id;
	Renderer *renderer;
	Parameters() {}
	Parameters( unsigned int i, Renderer *r ) : id( i ), renderer( r ) {}
};

void* start( void *p );
bool compare_lights( const Light* l1, const Light* l2 );
double getDistance( const Point3D &p1, const Point3D &p2 );
Vector3D randomDirection();

#endif
