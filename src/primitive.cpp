#include "primitive.hpp"

Primitive::~Primitive()
{
}

bool Primitive::set_texture( Image *texture ) {
	m_texture = texture;
	return true;
}

Sphere::Sphere() {
	sphere = new NonhierSphere( Point3D(0, 0, 0), 1 );
}

bool Sphere::intersect( Ray &r, Intersection &i ) {
	return sphere->intersect( r, i );
}

bool Sphere::getColour( Point3D in_point, Colour &colour ) {
	sphere->set_texture( m_texture );
	return sphere->getColour( in_point, colour );
}

Sphere::~Sphere()
{
}

Cube::Cube() {
	cube = new NonhierBox( Point3D(0, 0, 0), 1 );
}

bool Cube::intersect( Ray &r, Intersection &i ) {
	return cube->intersect( r, i );
}

bool Cube::getColour( Point3D in_point, Colour &colour ) {
	cube->set_texture( m_texture );
	return cube->getColour( in_point, colour );
}

Cube::~Cube()
{
}

bool NonhierSphere::intersect( Ray &r, Intersection &i ) {
	// Quadratic formula coefficients
	double A = r.direction.dot( r.direction );
	Vector3D dist = r.start_point - m_pos;
	double B = 2 * r.direction.dot( dist );
	double C = dist.dot( dist ) - pow( m_radius, 2 );

	// Check discriminant
	double discriminant = pow( B, 2 ) - 4 * A * C;
	if ( discriminant < 0 ) { return false; }

	// Solve the formula
	double roots[2];
	int size = quadraticRoots( A, B, C, roots );

	bool ret = false;
	if ( size == 0 ) {
		return ret;
	} else {
		if ( size == 2 ) {
			// Now find the closer point
			double temp = std::min( roots[0], roots[1] );
			if ( temp > TOO_CLOSE ) {
				Point3D in = r.start_point + temp * r.direction;
				i.interval[0] = in;
				i.distance = temp;
				ret = true;
			}

			// Save interval
			temp = std::max( roots[0], roots[1] );
			if ( temp > TOO_CLOSE ) {
				Point3D in = r.start_point + temp * r.direction;
				i.interval[1] = in;
			} else {
				i.interval[1] = i.interval[0];
			}
		} else if ( roots[0] > TOO_CLOSE ) {
			// Save interval
			Point3D in = r.start_point + roots[0] * r.direction;
			i.interval[0] = in;
			i.interval[1] = in;
			i.distance = roots[0];
			ret = true;
		} else {
			ret = false;
		}		
	}
	
	// Update the intersection info
	// Point of intersection
	Point3D p = r.start_point + i.distance * r.direction;
	i.intersect = p;
	i.normal =  p - m_pos ;
	i.normal.normalize();
	i.incoming = r.direction;

	return ret;
}


/***********************************************************************************************
 *** The calculation is taken from http://www.cs.unc.edu/~rademach/xroads-RT/RTarticle.html. ***
 *** At lesson "Texture mapping spheres".                                                    ***
 ***********************************************************************************************/
bool NonhierSphere::getColour( Point3D in_point, Colour &colour ) {
	if ( m_texture != NULL ) {
		// Vector pointing to our point
		Vector3D Vp = in_point - m_pos_text;
		Vp.normalize();

		double phi = acos( -Vn.dot( Vp ) );
		double v = 1 - phi / M_PI;
		double temp = acos( Vp.dot( Ve ) / sin( phi ) );
		double theta = temp / ( 2 * M_PI );
		double u;
		Vector3D check = Vn.cross( Ve );
		if ( check.dot( Vp ) > 0 ) {
			u = theta;
		} else {
			u = 1 - theta;
		}

		int texture_x = (int)(u * m_texture->width());
		int texture_y = (int)(v * m_texture->height());
		Colour c( (*m_texture)(texture_x, texture_y, 0),
				  (*m_texture)(texture_x, texture_y, 1),
				  (*m_texture)(texture_x, texture_y, 2) );
		colour = c;
		return true;
	}
	return false;
}

NonhierSphere::~NonhierSphere()
{
}


NonhierBox::NonhierBox(const Point3D& pos, double size)
	: m_pos(pos), m_size(size)
{
	m_pos_text = pos;
	m_size_text = size;
	updateBounds();
}

bool NonhierBox::intersect( Ray &r, Intersection &i ) {
	// I've followed the Ray-Box intersection algorithm given from this website:
	// http://cs.fit.edu/~wds/classes/adv-graphics/raytrace/raytrace.html
	double t_far = std::numeric_limits<double>::max();
	double t_near = -t_far;


	// Top right corner of box, m_pos is the bottom left
	double top_right[3];
	top_right[0] = m_pos[0] + m_size;
	top_right[1] = m_pos[1] + m_size;
	top_right[2] = m_pos[2] + m_size;

	// Normal of the light after hitting a plane
	Vector3D normal;

	// Plane hit
	unsigned int plane_hit;

	// Check all three: in_point and z planes ( checking two planes at once )
	for ( unsigned int j = 0; j < 3; j += 1 ) {
		// NOTE: These two are x0, y0 and z0 not the actual coordinate
		double ray_origin = r.start_point[j];
		double ray_direction = r.direction[j];
		if ( ray_direction == 0 ) {		// If ray is parallel to the plane
			if ( ray_origin < m_pos[j] || ray_origin > top_right[j] ) { // No intersection
				return false;
			}
		} else {
			double t1 = ( m_pos[j] - ray_origin ) / ray_direction;
			double t2 = ( top_right[j] - ray_origin ) / ray_direction;
			if ( t1 > t2 ) { 	// Swap if t1 > t2
				double temp;
				temp = t1;
				t1 = t2;
				t2 = temp;
			}
			if ( t1 > t_near ) { 
				t_near = t1; 
				plane_hit = j;							// Record plane hit
			}
			if ( t2 < t_far ) { t_far = t2; }
			if ( t_near > t_far ) { return false; } // Box is missed
			if ( t_far < 0.0 ) { return false; }	// Box behind the ray, missed
		
		}	
	}
	t_near -= TOO_CLOSE;
	t_far -= TOO_CLOSE;
	
	// Calculate the point of interseciton
	Point3D in;
	in = r.start_point + t_near * r.direction;
	i.interval[0] = in;
	in = r.start_point + t_far * r.direction;
	i.interval[1] = in;

	i.intersect = Point3D( r.start_point[0] + r.direction[0] * t_near,
	                       r.start_point[1] + r.direction[1] * t_near,
	                       r.start_point[2] + r.direction[2] * t_near );
	i.distance = t_near;

	// NOTE: Normal of the interseciton will only be in one of in_point or z direction.
	if ( plane_hit == 0 ) {		
		normal = Vector3D( -r.direction[0], 0.0, 0.0 );
	} else if ( plane_hit == 1 ) {
		normal = Vector3D( 0.0, -r.direction[1], 0.0 );
	} else if ( plane_hit == 2 ) {
		normal = Vector3D( 0.0, 0.0, -r.direction[2] );
	}
	normal.normalize();
	i.normal = normal;
	i.incoming = r.direction;
	
	return true;
}

bool NonhierBox::getColour( Point3D in_point, Colour &colour ) {
	// The mapping is applied to each face
	if ( m_texture != NULL ) {
		// NOTE: The computation error does effect the check here!
		double in_x, in_y, in_z;
		in_x = in_point[0];
		in_y = in_point[1];
		in_z = in_point[2];

		int texture_x = 0, texture_y = 0;		
		double x_ratio, y_ratio;
		x_ratio = m_texture->width() / m_size_text;
		y_ratio = m_texture->height() / m_size_text;
		// Determine the plane that the intersection point is on
		if ( in_x > m_pos_min_down[0] && in_x < m_pos_min_up[0] ) {
			texture_x = (int)( x_ratio * ( in_z - m_pos_text[2] ) );
			texture_y = (int)( y_ratio * ( in_y - m_pos_text[1] ) );
		} else if ( in_x > m_pos_max_down[0] && in_x < m_pos_max_up[0] ) {
			texture_x = (int)( x_ratio * ( in_z - m_pos_text[2] ) );
			texture_y = (int)( y_ratio * ( in_y - m_pos_text[1] ) );
		} else if ( in_y > m_pos_min_down[1] && in_y < m_pos_min_up[1] ) {
			texture_x = (int)( x_ratio * ( in_x - m_pos_text[0] ) );
			texture_y = (int)( y_ratio * ( in_z - m_pos_text[2] ) );
		} else if ( in_y > m_pos_max_down[1] && in_y < m_pos_max_up[1] ) {
			texture_x = (int)( x_ratio * ( in_x - m_pos_text[0] ) );
			texture_y = (int)( y_ratio * ( in_z - m_pos_text[2] ) );
		} else if ( in_z > m_pos_min_down[2] && in_z < m_pos_min_up[2] ) {
			texture_x = (int)( x_ratio * ( in_x - m_pos_text[0] ) );
			texture_y = (int)( y_ratio * ( in_y - m_pos_text[1] ) );
		} else if ( in_z > m_pos_max_down[2] && in_z < m_pos_max_up[2] ) {
			texture_x = (int)( x_ratio * ( in_x - m_pos_text[0] ) );
			texture_y = (int)( y_ratio * ( in_y - m_pos_text[1] ) );
		} else {
			// Point not on the plane?
		}

		texture_x = std::abs( texture_x );
		texture_y = std::abs( texture_y );
		texture_y = m_texture->height() - texture_y;

		Colour c( (*m_texture)(texture_x, texture_y, 0),
				  (*m_texture)(texture_x, texture_y, 1),
				  (*m_texture)(texture_x, texture_y, 2) );
		colour = c;
		return true;
	}
	return false;
}

NonhierBox::~NonhierBox()
{
}

void NonhierBox::updateBounds() {
	double epsilon = 0.1;	// This is the maximum epsilon accepts for error
	Point3D ep_point( epsilon, epsilon, epsilon );
	Point3D neg_ep_point( -epsilon, -epsilon, -epsilon );
	Vector3D size_point( m_size_text, m_size_text, m_size_text );
	m_pos_min_down = m_pos_text - ep_point;
	m_pos_min_up = m_pos_text - neg_ep_point;
	m_pos_max_down = m_pos_min_down + size_point;
	m_pos_max_up = m_pos_min_up + size_point;
}

Cylinder::Cylinder() {
	m_pos = Point3D( 0.0, 0.0, 0.0 );
	m_radius = 1;
	m_height = 1;
	z_min = m_pos[0];
	z_max = m_pos[0] + m_height;
}

Cylinder::~Cylinder() {}

// The algorithm is from: 
// http://www.cl.cam.ac.uk/teaching/1999/AGraphHCI/SMAG/node2.html
bool Cylinder::intersect( Ray &r, Intersection &i ) {
	// Quadratic formula coefficients
	Vector3D dir;
	Point3D start;
	dir = r.direction;
	start = r.start_point;
	double A = pow( dir[0], 2 ) + pow( dir[1], 2 );
	double B = 2 * ( start[0] * dir[0] + start[1] * dir[1] );
	double C = pow( start[0], 2 ) + pow( start[1], 2 ) - pow( m_radius, 2 );

	// Check discriminant
	double discriminant = pow( B, 2 ) - 4 * A * C;
	if ( discriminant < 0 ) { return false; }

	// Solve the formula
	double roots[2];
	int size = quadraticRoots( A, B, C, roots );

	bool ret = false;
	double def = std::numeric_limits<double>::max();
	double inter_min = def;
	double inter_max = -def;
	double radius_square = pow( m_radius, 2 );
	double z1, z2;

	if ( size == 0 ) {
		// No intersection
		return false;
	} else {
		if ( size == 2 ) {
			// Check z with z_min and z_max
			z1 = start[2] + roots[0] * dir[2];
			z2 = start[2] + roots[1] * dir[2];
			// Test root 0
			if ( roots[0] > TOO_CLOSE && z1 > z_min && z1 < z_max ) {
				inter_min = roots[0];
				inter_max = roots[0];
			}
			// Test root 1
			if ( roots[1] > TOO_CLOSE && z2 > z_min && z2 < z_max ) {
				if ( roots[1] < inter_min ) inter_min = roots[1];
			} 

			if ( inter_min > TOO_CLOSE && inter_min != def ) {
				i.distance = inter_min;
				ret = true;
			}
		} else if ( roots[0] > TOO_CLOSE ) {
			z1 = start[2] + roots[0] * dir[2];
			if ( z1 > z_min && z1 < z_max ) {
				inter_min = roots[0];
				inter_max = roots[0];
			}

			if ( inter_min > TOO_CLOSE && inter_min != def ) {
				i.distance = inter_min;
				ret = true;
			}
		}
	}
   
	if ( ret ) {
		// Update intersection info
		Point3D p = r.start_point + i.distance * r.direction;
		i.intersect = p;
		i.normal = p - m_pos;
		i.normal.normalize();
		i.incoming = r.direction;
	}


	// NOTE: The normal of the plane is kinda weird, need to double check
	// Need to test positive-z and negative-z caps for closed end
	// Positive-z cap
	Vector3D normal( 0.0, 0.0, 1.0 );
	Point3D top_center = m_pos;
	top_center[2] = top_center[2] + m_height;
	double check_dir = normal.dot( dir );
	double t3 = std::numeric_limits<double>::max();
	if ( check_dir != 0 ) {
		t3 = ( top_center - start ).dot( normal ) / check_dir;
		Point3D p = start + t3 * dir;
		double xy_square = pow( p[0], 2 ) + pow( p[1], 2 );
		if ( t3 > TOO_CLOSE && xy_square < radius_square ) {
		    if ( t3 < inter_min ) {
				inter_min = t3;				
				i.distance = t3;
				i.intersect = p;
				i.normal = p - Point3D( 0.0, 0.0, p[2] );
				i.normal.normalize();
				i.incoming = r.direction;
				ret = true;
			} else if ( t3 > inter_max ) {
				inter_max = t3;
			}
		}			
	}
	
	// Negative-z cap
	normal = Vector3D( 0.0, 0.0, -1.0 );
	top_center = m_pos;
	check_dir = normal.dot( dir );
	double t4 = std::numeric_limits<double>::max();
	if ( check_dir != 0 ) {
		t4 = ( top_center - start ).dot( normal ) / check_dir;
		Point3D p = start + t4 * dir;
		double xy_square = pow( p[0], 2 ) + pow( p[1], 2 );
		if ( t4 > TOO_CLOSE && xy_square < radius_square ) {
		    if ( t4 < inter_min ) {
				inter_min = t4;
				i.distance = t4;
				i.intersect = p;
				i.normal = p - Point3D( 0.0, 0.0, p[2] );
				i.normal.normalize();
				i.incoming = r.direction;
				ret = true;
			} else if ( t4 > inter_max ) {
				inter_max = t4;
			}
		}
	}

	// Intersection interval
	if ( ret ) {
		Point3D inter = start + inter_min * dir;
		i.interval[0] = inter;
		inter = start + inter_max * dir;
		i.interval[1] = inter;
	}
	
	return ret;
}

bool Cylinder::getColour( Point3D in_point, Colour &colour ) {
	if ( m_texture != NULL ) {
		Point3D top_center = m_pos_text;
		top_center[2] = top_center[2] + m_height_text;
		Vector3D Vn = top_center - m_pos_text;
		Vn.normalize();
		
		Point3D origin = m_pos_text;
		origin[1] = origin[1] + m_radius;
		Vector3D Ve = origin - m_pos_text;
		Ve.normalize();

		Vector3D Vp = in_point - m_pos_text;
		Vp.normalize();

		double phi = acos( -Vn.dot( Vp ) );
		double temp = acos( Vp.dot( Ve ) / sin( phi ) );
		double theta = temp / ( 2 * M_PI );
		double u;
		Vector3D check = Vn.cross( Ve );
		if ( check.dot( Vp ) > 0 ) {
			u = theta;
		} else {
			u = 1 - theta;
		}

		double t = (in_point[2] - m_pos_text[2]) / m_height_text;

		int texture_x = (int)( m_texture->width() * u );
		int texture_y = (int)( m_texture->height() * t );

		texture_x = std::abs( texture_x );
		texture_y = std::abs( texture_y );

		Colour c( (*m_texture)(texture_x, texture_y, 0),
				  (*m_texture)(texture_x, texture_y, 1),
				  (*m_texture)(texture_x, texture_y, 2) );
		colour = c;
	}
	return true;
}

Cone::Cone() {
	m_pos = Point3D( 0.0, 0.0, 0.0 );
	m_radius = 1.0;
	m_height = 1.0;
	z_min = m_pos[2];
	z_max = z_min + m_height;
}

Cone::~Cone() {}

bool Cone::intersect( Ray &r, Intersection &i ) {
	// Quadratic formula coefficients
	Vector3D dir;
	Point3D start;
	dir = r.direction;
	start = r.start_point;
	double A = pow( dir[0], 2 ) + pow( dir[1], 2 ) - pow( dir[2], 2 );
	double B = 2 * ( start[0] * dir[0] + start[1] * dir[1] - start[2] * dir[2] );
	double C = pow( start[0], 2 ) + pow( start[1], 2 ) - pow( start[2], 2 );

	// Check discriminant
	double discriminant = pow( B, 2 ) - 4 * A * C;
	if ( discriminant < 0 ) { return false; }

	// Solve the formula
	double roots[2];
	int size = quadraticRoots( A, B, C, roots );

	bool ret = false;
	double def = std::numeric_limits<double>::max();
	double inter_min = def;
	double inter_max = -def;
	double radius_square = pow( m_radius, 2 );
	double z1, z2;
	
	if ( size == 0 ) {
		// No intersection
		return false;
	} else {
		if ( size == 2 ) {
			// Check z with z_min and z_max
			z1 = start[2] + roots[0] * dir[2];
			z2 = start[2] + roots[1] * dir[2];
			// Test root 0
			if ( roots[0] > TOO_CLOSE && z1 > z_min && z1 < z_max ) {
				inter_min = roots[0];
				inter_max = roots[0];
			}
			// Test root 1
			if ( roots[1] > TOO_CLOSE && z2 > z_min && z2 < z_max ) {
				if ( roots[1] < inter_min ) inter_min = roots[1];
			} 

			if ( inter_min > TOO_CLOSE && inter_min != def ) {
				i.distance = inter_min;
				ret = true;
			}
		} else if ( roots[0] > TOO_CLOSE ) {
			z1 = start[2] + roots[0] * dir[2];
			if ( z1 > z_min && z1 < z_max ) {
				inter_min = roots[0];
				inter_max = roots[0];
			}

			if ( inter_min > TOO_CLOSE && inter_min != def ) {
				i.distance = inter_min;
				ret = true;
			}
		}
	}

	if ( ret ) {
		// Update intersection info
		Point3D p = r.start_point + i.distance * r.direction;
		i.intersect = p;
		i.normal = p - m_pos;
		i.normal.normalize();
		i.incoming = r.direction;
	}

	// NOTE: The normal of the plane is kinda weird, need to double check
	// Need to test positive-z cap for closed end
	// Positive-z cap
	Vector3D normal( 0.0, 0.0, 1.0 );
	Point3D top_center = m_pos;
	top_center[2] = top_center[2] + m_height;
	double check_dir = normal.dot( dir );
	double t3 = std::numeric_limits<double>::max();
	if ( check_dir != 0 ) {
		t3 = ( top_center - start ).dot( normal ) / check_dir;
		Point3D p = start + t3 * dir;
		double xy_square = pow( p[0], 2 ) + pow( p[1], 2 );
		if ( t3 > TOO_CLOSE && xy_square < radius_square ) {
		    if ( t3 < inter_min ) {
				inter_min = t3;				
				i.distance = t3;
				i.intersect = p;
				i.normal = p - Point3D( 0.0, 0.0, p[2] );
				i.normal.normalize();
				i.incoming = r.direction;
				ret = true;
			} else if ( t3 > inter_max ) {
				inter_max = t3;
			}
		}			
	}

	// Intersection interval
	if ( ret ) {
		Point3D inter = start + inter_min * dir;
		i.interval[0] = inter;
		inter = start + inter_max * dir;
		i.interval[1] = inter;
	}

	return ret;
}


// NEED TO CHECK
bool Cone::getColour( Point3D in_point, Colour &colour ) {
	if ( m_texture != NULL ) {
		Point3D top_center = m_pos_text;
		top_center[2] = top_center[2] + m_height_text;
		Vector3D Vn = top_center - m_pos_text;
		Vn.normalize();
		
		Point3D origin = m_pos_text;
		origin[1] = origin[1] + m_radius;
		Vector3D Ve = origin - m_pos_text;
		Ve.normalize();

		Vector3D Vp = in_point - m_pos_text;
		Vp.normalize();

		double phi = acos( -Vn.dot( Vp ) );
		double temp = acos( Vp.dot( Ve ) / sin( phi ) );
		double theta = temp / ( 2 * M_PI );
		double u;
		Vector3D check = Vn.cross( Ve );
		if ( check.dot( Vp ) > 0 ) {
			u = theta;
		} else {
			u = 1 - theta;
		}

		double t = (in_point[2] - m_pos_text[2]) / m_height_text;

		int texture_x = (int)( m_texture->width() * u );
		int texture_y = (int)( m_texture->height() * t );

		texture_x = std::abs( texture_x );
		texture_y = std::abs( texture_y );

		Colour c( (*m_texture)(texture_x, texture_y, 0),
				  (*m_texture)(texture_x, texture_y, 1),
				  (*m_texture)(texture_x, texture_y, 2) );
		colour = c;
	}
	return true;
}
