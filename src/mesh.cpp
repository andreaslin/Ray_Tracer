#include "mesh.hpp"
#include <iostream>

Mesh::Mesh(const std::vector<Point3D>& verts,
           const std::vector< std::vector<int> >& faces)
	: Primitive(), m_verts(verts),
	  m_faces(faces)
{
	this->setBoundingSphere();	
	this->setNormals();
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
	std::cerr << "mesh({";
	for (std::vector<Point3D>::const_iterator I = mesh.m_verts.begin(); I != mesh.m_verts.end(); ++I) {
		if (I != mesh.m_verts.begin()) std::cerr << ",\n      ";
		std::cerr << *I;
	}
	std::cerr << "},\n\n     {";
  
	for (std::vector<Mesh::Face>::const_iterator I = mesh.m_faces.begin(); I != mesh.m_faces.end(); ++I) {
		if (I != mesh.m_faces.begin()) std::cerr << ",\n      ";
		std::cerr << "[";
		for (Mesh::Face::const_iterator J = I->begin(); J != I->end(); ++J) {
			if (J != I->begin()) std::cerr << ", ";
			std::cerr << *J;
		}
		std::cerr << "]";
	}
	std::cerr << "});" << std::endl;
	return out;
}

bool Mesh::intersect( Ray &r, Intersection &i ) {
	// First test if the ray hit the bounding sphere, only care if it hits
	// so the hitting information is not important here
	Intersection temp = i;
	bool hit = false;

	// NOTE: Have to check if the starting point of the ray is alreay inside the bounding sphere
	// if it is, it's still consider to be hitting the bounding sphere
	bool inside = false;
	Point3D center = bounding_sphere->getCenter();
	double radius = bounding_sphere->getRadius();
	inside = ( r.start_point[0] > center[0] - radius ) && ( r.start_point[0] < center[0] + radius ) &&
		( r.start_point[1] > center[1] - radius ) && ( r.start_point[1] < center[1] + radius ) &&
		( r.start_point[2] > center[2] - radius ) && ( r.start_point[2] < center[2] + radius );
	
	hit = bounding_sphere->intersect( r, temp );
	// Does not hit the bounding sphere and not inside the sphere
	if ( !hit && !inside ) return false; 

	if ( BOUNDING ) i = temp;		// Show bounding sphere

	bool ret = false;
	// Go through each face, and see which face has the closest intersection
	for( unsigned int j = 0; j < m_faces.size(); j += 1 ) {
		Face face = m_faces[j];
		Vector3D normal = face_normals[j];

		double l_dot_n = normal.dot(r.direction);
		if ( l_dot_n == 0 ) { continue; } 	// Ray parallel to face, go to the next face
		
		// Solve for d, refer to Line-plane intersection from Wikipedia
		double dis = ( (m_verts[face[0]] - r.start_point).dot( normal ) ) / l_dot_n;
		dis -= 1e-8;				  			// Close distance
		if ( dis <= 0 ) { continue; } 			// negative distance or line on the plane
		if ( dis > i.distance ) { continue; } 	// Closer intersection exists
		
		// Intersection point
		Point3D intersect = r.start_point + dis * r.direction;
		
		// Pick a vertex as the origin of all triangles and check which triangle the ray hits
		Point3D p0 = m_verts[face[0]];
		for( unsigned int k = 1; k < face.size() - 1; k += 1 ) {
			Point3D p1 = m_verts[face[k]];
			Point3D p2 = m_verts[face[k + 1]];
			Triangle tri( p0, p1, p2 );
			if ( triIntersect( r, tri ) ) {
				i.normal = normal;
				i.intersect = intersect;
				i.distance = dis;
				ret = true;
				break;
			}
		}
	}
	
	// Return hitting or not based on if the bounding sphere is shown
	if ( BOUNDING ) {
		return ret | hit;
	} else {
		return ret;
	}
}

bool Mesh::triIntersect( Ray &r, Triangle &t ) {
	Point3D p0 = t.v0;
	Point3D p1 = t.v1;
	Point3D p2 = t.v2;

	double A = p0[0] - p1[0];
	double B = p0[1] - p1[1];
	double C = p0[2] - p1[2];

	double D = p0[0] - p2[0];
	double E = p0[1] - p2[1];
	double F = p0[2] - p2[2];

	double G = r.direction[0];
	double H = r.direction[1];
	double I = r.direction[2];

	double J = p0[0] - r.start_point[0];
	double K = p0[1] - r.start_point[1];
	double L = p0[2] - r.start_point[2];

	double EIHF = E * I - H * F;
	double GFDI = G * F - D * I;
	double DHEG = D * H - E * G;

	double denom = ( A * EIHF + B * GFDI + C * DHEG );
	double beta = ( J * EIHF + K * GFDI + L * DHEG ) / denom;	
	if ( beta < 0.0 || beta > 1.0 ) return false;

	double AKJB = A * K - J * B;
	double JCAL = J * C - A * L;
	double BLKC = B * L - K * C;
	
	double gamma = ( I * AKJB + H * JCAL + G * BLKC ) / denom;
	if ( gamma < 0.0 || beta + gamma > 1.0 ) return false;

	return true;
}

void Mesh::setBoundingSphere() {
	double d_max = std::numeric_limits<double>::max();
	double d_min = -d_max;

	double p_min[] = { d_max, d_max, d_max };
	double p_max[] = { d_min, d_min, d_min };

	// Find the min and max from all three axises from all vertices
	for( std::vector<Point3D>::const_iterator it = m_verts.begin(); it != m_verts.end(); it++ ) {
		Point3D ver = (*it);
		// Check bounds
		for( unsigned int i = 0; i < 3; i += 1 ) {
			if ( ver[i] < p_min[i] ) p_min[i] = ver[i];
			if ( ver[i] > p_max[i] ) p_max[i] = ver[i];
		}
	}
	// Calculate the sphere information and save the bounding sphere
	Point3D center( ( p_min[0] + p_max[0] ) / 2, 
	                ( p_min[1] + p_max[1] ) / 2,
	                ( p_min[2] + p_max[2] ) / 2 );
	Point3D p_min_( p_min[0], p_min[1], p_min[2] );
	Point3D p_max_( p_max[0], p_max[1], p_max[2] );
	bounding_sphere = new NonhierSphere( center, ( p_max_ - p_min_ ).length() / 2  );
}

void Mesh::setNormals() {
	Vector3D v0, v1, normal;
	Face face;
	for( unsigned int i = 0; i < m_faces.size(); i += 1 ) {
		face = m_faces[i];
		// Pick any two vector from the face, and get the normal
		v0 = m_verts[face[1]] - m_verts[face[0]];
		v1 = m_verts[face[2]] - m_verts[face[0]];
		normal = v0.cross(v1);
		normal.normalize();
		face_normals.push_back( normal );
	}
}

bool Mesh::getColour( Point3D in_point, Colour &colour ) {
	return false;
}
