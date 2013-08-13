#ifndef CS488_MESH_HPP
#define CS488_MESH_HPP

#include <vector>
#include <iosfwd>
#include "primitive.hpp"
#include "algebra.hpp"
#include "ray.hpp"
#include <limits>
#include <limits.h>

/************************************************************************************
 *** NOTE: Here I will be following the triangle meshing method from the book -   ***
 ***       "Realistic Ray Tracing" by Peter Shirley and R.Keith Morley from 7.1.3 ***
 ***       . However, some changes are adjusted since we are using faces here and ***
 ***       also bounding box is used.                                             ***
 ************************************************************************************/

extern bool BOUNDING;

// A polygonal mesh.
class Mesh : public Primitive {
public:
	// New triangle structure
	struct Triangle {
		Triangle( Point3D p0, Point3D p1, Point3D p2 ) {
			v0 = p0; v1 = p1; v2 = p2;
		}
		Point3D v0;
		Point3D v1;
		Point3D v2;
	};

	Mesh(const std::vector<Point3D>& verts,
	     const std::vector< std::vector<int> >& faces);

	typedef std::vector<int> Face;
	virtual bool intersect( Ray &r, Intersection &i );
	virtual bool getColour( Point3D in_point, Colour &colour );
	bool triIntersect( Ray &r, Triangle &t );
	virtual bool isHierachical() { return false; }
	virtual void updatePosition( Point3D new_pos ) {}
	virtual void updateSize( Vector3D scale ) {}
private:
	std::vector<Point3D> m_verts;
	std::vector<Face> m_faces;

	friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);

	void setBoundingSphere();
	void setNormals();

	// Bounding sphere
	NonhierSphere *bounding_sphere;

	// Normal of all the faces
	std::vector<Vector3D> face_normals;
};

#endif
