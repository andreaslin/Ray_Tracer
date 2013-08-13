#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include "algebra.hpp"
#include "ray.hpp"
#include "polyroots.hpp"
#include <limits>
#include "image.hpp"

#ifndef TOO_CLOSE 
#define TOO_CLOSE 0.00000001
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Primitive {
public:
	Primitive() { m_texture = NULL; }
	virtual ~Primitive();
	virtual bool intersect( Ray &r, Intersection &i ) = 0;
	virtual bool getColour( Point3D in_point, Colour &colour ) = 0;
	bool set_texture( Image *texture );
	bool has_texture() { return m_texture != NULL; }
	virtual bool isHierachical() = 0;
	virtual void updatePosition( Point3D new_pos ) = 0;
	virtual void updateSize( Vector3D scale ) = 0;
protected:
	Image *m_texture;
};

class NonhierSphere : public Primitive {
public:
	NonhierSphere(const Point3D& pos, double radius)
		: m_pos(pos), m_radius(radius)
	{
		Point3D north_pole = m_pos;
		north_pole[1] = north_pole[1] + m_radius;
		Vn = north_pole - m_pos;
		Vn.normalize();
		// Point used for the equator defines the starting point of the mapping
		Point3D equator = m_pos;
		equator[2] = equator[2] - m_radius;
		Ve = equator - m_pos;
		Ve.normalize();
		m_pos_text = m_pos;
		m_radius_text = m_radius;
	}
	virtual ~NonhierSphere();
	virtual bool intersect( Ray &r, Intersection &i );
	virtual bool getColour( Point3D in_point, Colour &colour );
	Point3D getCenter() { return m_pos; }
	double getRadius() { return m_radius; }
	virtual bool isHierachical() { return false; }
	virtual void updatePosition( Point3D new_pos ) { m_pos_text = new_pos; }
	virtual void updateSize( Vector3D scale ) { m_radius_text = scale[0]; }

private:
	Point3D m_pos;
	double m_radius;
	Point3D m_pos_text;			// Position for texture mapping, not for transformation
	double m_radius_text;		// Radius for texture mapping
	Vector3D Vn;				// Vector pointing to north-pole
	Vector3D Ve;				// Vector pointing to equator
};

class NonhierBox : public Primitive {
public:
	NonhierBox(const Point3D& pos, double size);
	virtual ~NonhierBox();
	virtual bool intersect( Ray &r, Intersection &i );
	virtual bool getColour( Point3D in_point, Colour &colour );
	virtual bool isHierachical() { return false; }
	virtual void updatePosition( Point3D new_pos ) { 
		m_pos_text = new_pos; 
		updateBounds();
	}
	virtual void updateSize( Vector3D scale ) { 
		m_size_text = scale[0]; 
		updateBounds();
	}
	void updateBounds();

private:
	Point3D m_pos;
	double m_size;
	Point3D m_pos_text;
	double m_size_text;
	Vector3D m_pos_min_up, m_pos_min_down;
	Vector3D m_pos_max_up, m_pos_max_down;
};
class Sphere : public Primitive {
public:
	Sphere();
	virtual ~Sphere();
	virtual bool intersect( Ray &r, Intersection &i );
	virtual bool getColour( Point3D in_point, Colour &colour );
	virtual void updatePosition( Point3D new_pos ) { sphere->updatePosition( new_pos ); }
	virtual bool isHierachical() { return true; }
	virtual void updateSize( Vector3D scale ) { sphere->updateSize( scale ); }
private:
	NonhierSphere *sphere;
};

class Cube : public Primitive {
public:
	Cube();
	virtual ~Cube();
	virtual bool intersect( Ray &r, Intersection &i );
	virtual bool getColour( Point3D in_point, Colour &colour );
	virtual void updatePosition( Point3D new_pos ) { cube->updatePosition( new_pos ); }
	virtual bool isHierachical() { return true; }
	virtual void updateSize( Vector3D scale ) { cube->updateSize( scale ); }
private:
	NonhierBox *cube;
};


// New close-ended cylinder class
class Cylinder : public Primitive {
public:
	Cylinder();
	virtual ~Cylinder();
	virtual bool intersect( Ray &r, Intersection &i );
	virtual bool getColour( Point3D in_point, Colour &colour );
	virtual bool isHierachical() { return true; }
	virtual void updatePosition( Point3D new_pos ) { m_pos_text = new_pos; }
	virtual void updateSize( Vector3D scale ) { m_height_text = scale[2]; };
private:
	Point3D m_pos;
	double m_radius;
	Point3D m_pos_text;
	double m_height_text;
	double m_height;
	double z_min, z_max;
};

// New cone class
class Cone : public Primitive {
public:
	Cone();
	virtual ~Cone();
	virtual bool intersect( Ray &r, Intersection &i );
	virtual bool getColour( Point3D in_point, Colour &colour );
	virtual bool isHierachical() { return true; }
	virtual void updatePosition( Point3D new_pos ) { m_pos_text = new_pos; }
	virtual void updateSize( Vector3D scale ) { m_height_text = scale[2]; };
private:
	Point3D m_pos;
	double m_radius;
	Point3D m_pos_text;
	double m_height_text;
	double m_height;
	double z_min, z_max;
};

#endif
