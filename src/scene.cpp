#include "scene.hpp"
#include <iostream>

/*********************************************************************************************
 *** NOTE: Most of the codes here are direct copy from my own assignment from CS488 W12 A3 ***
 *********************************************************************************************/

SceneNode::SceneNode(const std::string& name)
	: m_name(name)
{
	rotation = Vector3D();
	update = false;
}

SceneNode::~SceneNode()
{
}

bool SceneNode::intersect( Ray &r, Intersection &i ) {
	// New ray to trace after transformation, notice inverse matrices are used because of tracing backward
	Point3D pos = m_invtrans * r.start_point;
	Vector3D dir = m_invtrans * r.direction;
	Ray ray( pos, dir );
	bool ret = false;
	double closest = std::numeric_limits<double>::max();
	
	// Iterate the child list
	for( ChildList::const_iterator it = m_children.begin(); it != m_children.end(); it++ ) {
		Intersection isect;
		bool hit = (*it)->intersect( ray , isect );
		if ( hit && isect.distance <= closest ) {
			i = isect;
			closest = isect.distance;
		}
		ret = ret | hit;
	}

	// Now check if we hit any primitives, update intersection info
	if ( ret ) {
		i.intersect = m_trans * i.intersect;
		i.normal = (m_invtrans.transpose()) * i.normal;	
	}

	return ret;
}

void SceneNode::rotate(char axis, double angle)
{
#ifdef DEBUG1
	std::cerr << "Stub: Rotate " << m_name << " around " << axis << " by " << angle << std::endl;
#endif
	Matrix4x4 r;
	double angle_radian = TO_RADIAN * angle;
	switch ( axis ) {
	case 'x':
		rotation[0] += angle;
		if ( rotation[0] > 360.0 ) rotation[0] -= 360.0;
		if ( rotation[0] < -360.0 ) rotation[0] += 360.0;
		r[1][1] = cos( angle_radian );
		r[1][2] = -sin( angle_radian );
		r[2][1] = sin( angle_radian );
		r[2][2] = cos( angle_radian);
		break;
	case 'y':
		rotation[1] += angle;
		if ( rotation[1] > 360.0 ) rotation[1] -= 360.0;
		if ( rotation[1] < -360.0 ) rotation[1] += 360.0;
		r[0][0] = cos( angle_radian );
		r[0][2] = sin( angle_radian );
		r[2][0] = -sin( angle_radian );
		r[2][2] = cos( angle_radian );
		break;
	case 'z':
		r[0][0] = cos( angle_radian );
		r[0][1] = -sin( angle_radian );
		r[1][0] = sin( angle_radian );
		r[1][1] = cos( angle_radian );
		break;
	default:
		std::cerr << "Error while rotating" << std::endl;
		break;
	}

	// Apply the rotation
	set_transform( m_trans * r );
	m_invtrans = m_trans.invert();

	// Check limits
	if ( this->is_joint() ) ((JointNode *)this)->checkLimits();
}

void SceneNode::scale(const Vector3D& amount)
{
#ifdef DEBUG1
	std::cerr << "Stub: Scale " << m_name << " by " << amount << std::endl;
#endif
	Matrix4x4 s;
	s[0][0] = amount[0];
	s[1][1] = amount[1];
	s[2][2] = amount[2];
  
	// Apply scaling
	set_transform( m_trans * s );
	m_invtrans = m_trans.invert();

	// Scale value for some primitives
	scale_value = amount;
	update = true;
}

void SceneNode::translate(const Vector3D& amount)
{
#ifdef DEBUG1
	std::cerr << "Stub: Translate " << m_name << " by " << amount << std::endl;
#endif
	Matrix4x4 t;
	t[0][3] = amount[0];
	t[1][3] = amount[1];
	t[2][3] = amount[2];

	// Apply translation
	set_transform( m_trans * t );
	m_invtrans = m_trans.invert();

	// For texture mapping
	update = true;
}

bool SceneNode::is_joint() const
{
	return false;
}

JointNode::JointNode(const std::string& name)
	: SceneNode(name)
{
}

JointNode::~JointNode()
{
}

bool JointNode::is_joint() const
{
	return true;
}

void JointNode::set_joint_x(double min, double init, double max)
{
	m_joint_x.min = min;
	m_joint_x.init = init;
	m_joint_x.max = max;
}

void JointNode::set_joint_y(double min, double init, double max)
{
	m_joint_y.min = min;
	m_joint_y.init = init;
	m_joint_y.max = max;
}

void JointNode::checkLimits() {
	// Checking the maximum rotation angle of x and y
	if ( rotation[0] > m_joint_x.max ) {
		rotate('x', m_joint_x.max - rotation[0] );
		rotation[0] = m_joint_x.max;
	} else if ( rotation[0] < m_joint_x.min ) {
		rotate('x', m_joint_x.min - rotation[0] );
		rotation[0] = m_joint_x.min;
	}
	if ( rotation[1] > m_joint_y.max ) {
		rotate('y', m_joint_y.max - rotation[1] );
		rotation[1] = m_joint_y.max;
	} else if ( rotation[1] < m_joint_y.min ) {
		rotate('y', m_joint_y.min - rotation[1] );
		rotation[1] = m_joint_y.min;			
	}	
}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
	: SceneNode(name),
	  m_primitive(primitive), m_texture(NULL)
{
}

GeometryNode::~GeometryNode()
{
}

bool GeometryNode::intersect( Ray &r, Intersection &i ) {
	// Identical to what SceneNode has done, only without triversing the child list (no child)
	// New ray to trace after transformation, notice inverse matrices are used because of tracing backward
	Point3D pos = m_invtrans * r.start_point;
	Vector3D dir = m_invtrans * r.direction;
	Ray ray( pos, dir );
	if ( !m_primitive ) return false;
	bool ret = m_primitive->intersect( ray, i );

	// For hierachical node, transformation has to be udpated to get the texture mapping correct
	if ( update && m_primitive->isHierachical() ) {
		Point3D p( m_trans[0][3], m_trans[1][3], m_trans[2][3] );
		m_primitive->updatePosition( p );
		m_primitive->updateSize( scale_value );
		update = false;
	}

	// Now check if we hit any primitives, update intersection info
	if ( ret ) {
		i.nearest = this;
		i.material = m_material;
		i.intersect = m_trans * i.intersect;
		i.normal = m_invtrans.transpose() * i.normal;
		i.primitive = m_primitive;
		// Update interval
		i.interval[0] = m_trans * i.interval[0];
		i.interval[1] = m_trans * i.interval[1];
	}

	return ret;
}
 
UnionNode::UnionNode(const std::string& name) : 
	SceneNode( name ) {
}

// Interact function
bool UnionNode::intersect( Ray &r, Intersection &i ) {
	Point3D pos = m_invtrans * r.start_point;
	Vector3D dir = m_invtrans * r.direction;
	Ray ray( pos, dir );
	bool ret = false;
	Point3D inter_min;
	Point3D inter_max;
	Intersection inter[2];
	unsigned int count = 0;

	// Iterate the child list, expect to only have two childern
	for( ChildList::const_iterator it = m_children.begin(); it != m_children.end(); it++ ) {
		Intersection isect;
		bool hit = (*it)->intersect( ray , isect );
		if ( hit ) {
			inter[count] = isect;
		}
		count += 1;
		ret = ret | hit;
	}

	if ( ret ) {
		inter_min = ( inter[0].interval[0][2] > inter[1].interval[0][2] ? 
		              inter[0].interval[0] : inter[1].interval[0] );
		inter_max = ( inter[0].interval[1][2] > inter[1].interval[1][2] ? 
		              inter[1].interval[1] : inter[0].interval[1] );
		if ( inter_min[2] == inter[0].interval[0][2] ) {
			i = inter[0];
		} else {
			i = inter[1];
		}

		// Update interval
		i.interval[0] = m_trans * inter_min;
		i.interval[1] = m_trans * inter_max;
		i.intersect = m_trans * inter_min;
		i.normal = (m_invtrans.transpose()) * i.normal;	
	}

	return ret;
}

IntersectNode::IntersectNode(const std::string& name) : 
	SceneNode( name ) {
}

// Interact function
bool IntersectNode::intersect( Ray &r, Intersection &i ) {
	Point3D pos = m_invtrans * r.start_point;
	Vector3D dir = m_invtrans * r.direction;
	Ray ray( pos, dir );
	bool ret = false;
	Point3D inter_min;
	Point3D inter_max;
	Intersection inter[2];
	unsigned int count = 0;

	// Iterate the child list, expect to only have two childern
	// NOTE: The primitive of the right child will dominate the result primitive
	for( ChildList::const_iterator it = m_children.begin(); it != m_children.end(); it++ ) {
		Intersection isect;
		bool hit = (*it)->intersect( ray , isect );
		if ( hit ) {			
			inter[count] = isect;
		}
		count += 1;
		ret = ret | hit;
	}

	if ( ret ) {
		// Find the intersection
		if ( inter[1].interval[0][2] > inter[0].interval[0][2] && 
		     inter[1].interval[1][2] < inter[0].interval[0][2] ) {
			inter_min = inter[0].interval[0];
			inter_max = ( inter[1].interval[1][2] > inter[0].interval[1][2] ? 
			              inter[1].interval[1] : inter[0].interval[1] );
		} else if ( inter[0].interval[0][2] > inter[1].interval[0][2] && 
		            inter[0].interval[1][2] < inter[1].interval[0][2] ) {
			inter_min = inter[1].interval[0];
			inter_max = ( inter[1].interval[1][2] < inter[0].interval[1][2] ?
			              inter[0].interval[1] : inter[1].interval[1] );
		} else {
			return false;		// No intersection
		}

		if ( inter_min[2] == inter[0].interval[0][2] ) {
			i = inter[0];
		} else {
			i = inter[1];
		}
		
		// Update interval
		// NOTE: When using intersection, texture mapping shuold not be applied.
		i.interval[0] = m_trans * inter_min;
		i.interval[1] = m_trans * inter_max;
		i.intersect = m_trans * inter_min;
		i.normal = (m_invtrans.transpose()) * i.normal;	
	}

	return ret;
}

DifferenceNode::DifferenceNode(const std::string& name) : 
	SceneNode( name ) {
}

// Interact function
bool DifferenceNode::intersect( Ray &r, Intersection &i ) {
	Point3D pos = m_invtrans * r.start_point;
	Vector3D dir = m_invtrans * r.direction;
	Ray ray( pos, dir );
	bool ret = false;
	Point3D inter_min;
	Point3D inter_max;
	Intersection inter[2];
	unsigned int count = 0;

	// Iterate the child list, expect to only have two childern
	// NOTE: Again we keep the right child's primitive
	for( ChildList::const_iterator it = m_children.begin(); it != m_children.end(); it++ ) {
		Intersection isect;
		bool hit = (*it)->intersect( ray , isect );
		if ( hit ) {	
			inter[count] = isect;
		}
		count += 1;
		ret = ret | hit;
	}

	if ( ret ) {
		if ( inter[1].interval[0][2] > inter[0].interval[0][2] ) {
			inter_min = inter[1].interval[0];
			inter_max = ( inter[1].interval[1][2] < inter[0].interval[0][2] ? 
			              inter[1].interval[1] : inter[0].interval[0] );
		} else if ( inter[0].interval[1][2] > inter[1].interval[1][2] ) {
			inter_min = inter[0].interval[1];
			inter_max = ( inter[1].interval[0][2] < inter[0].interval[1][2] ?
			              inter[1].interval[0] : inter[1].interval[1] );
		} else {
			return false;
		}

		if ( inter_min[2] == inter[0].interval[0][2] ) {
			i = inter[0];
		} else {
			i = inter[1];
		}
		
		// Update interval
		i.interval[0] = m_trans * inter_min;
		i.interval[1] = m_trans * inter_max;
		if ( inter_min[2] > 0 ) {
			i.intersect = m_trans * inter_max;
		} else {
			i.intersect = m_trans * inter_min;
		}
		i.normal = (m_invtrans.transpose()) * i.normal;	
	}

	return ret;
}
