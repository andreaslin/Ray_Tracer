#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include "algebra.hpp"
#include "primitive.hpp"
#include "material.hpp"
#include "image.hpp"
#include <cstring>
#include <limits>

class SceneNode {
public:
	SceneNode(const std::string& name);
	virtual ~SceneNode();

	const Matrix4x4& get_transform() const { return m_trans; }
	const Matrix4x4& get_inverse() const { return m_invtrans; }
  
	void set_transform(const Matrix4x4& m)
	{
		m_trans = m;
		m_invtrans = m.invert();
	}

	void set_transform(const Matrix4x4& m, const Matrix4x4& i)
	{
		m_trans = m;
		m_invtrans = i;
	}

	void add_child(SceneNode* child)
	{
		m_children.push_back(child);
	}

	void remove_child(SceneNode* child)
	{
		m_children.remove(child);
	}

	// Callbacks to be implemented.
	// These will be called from Lua.
	void rotate(char axis, double angle);
	void scale(const Vector3D& amount);
	void translate(const Vector3D& amount);

	// Returns true if and only if this node is a JointNode
	virtual bool is_joint() const;

	// Return name of the node
	std::string get_name() { return m_name; }

	// Rotation variables for checking limits
	Vector3D rotation;  

	void set_rotation( const Vector3D &r ) { rotation = r; }

	const Vector3D& get_rotation() const { return rotation; }

	// Interact function
	virtual bool intersect( Ray &r, Intersection &i );

protected:
  
	// Useful for picking
	int m_id;
	std::string m_name;

	// Transformations
	Matrix4x4 m_trans;
	Matrix4x4 m_invtrans;

	// Hierarchy
	typedef std::list<SceneNode*> ChildList;
	ChildList m_children;

	// NOTE: For primitives with one size, the x scalor dominates the scaling
	// Scale for udpating the radius of some primitives
	Vector3D scale_value;
	bool update;
};

class JointNode : public SceneNode {
public:
	JointNode(const std::string& name);
	virtual ~JointNode();

	virtual bool is_joint() const;

	void set_joint_x(double min, double init, double max);
	void set_joint_y(double min, double init, double max);

	struct JointRange {
		double min, init, max;
	};

	void checkLimits(); 		// Check the limits of x and y angle
  
protected:

	JointRange m_joint_x, m_joint_y;
};

class GeometryNode : public SceneNode {
public:
	GeometryNode(const std::string& name,
	             Primitive* primitive);
	virtual ~GeometryNode();

	const Material* get_material() const;
	Material* get_material();

	void set_material(Material* material)
	{
		m_material = material;
	}

	void set_texture( const std::string& filename ) {
		Image *m_texture = new Image();
		// Try opening the file and save the image to the primitive itself
		if ( !m_texture->loadPng( filename ) ) {
			std::cerr << "Error while loading texture file " << filename << ". Cannot find the file?" << std::endl;
			return;
		}
		if ( m_primitive->set_texture( m_texture ) ) {
			//			std::cerr << "Error while setting texture file " << filename << std::endl;
			return;
		}
	}

	// Interact function
	virtual bool intersect( Ray &r, Intersection &i );

protected:
	Material* m_material;
	Primitive* m_primitive;
	Image *m_texture;
};

// Union 
class UnionNode : public SceneNode {
public:
	UnionNode(const std::string& name);
	// Interact function
	virtual bool intersect( Ray &r, Intersection &i );
private:
	
};

// Intersect
class IntersectNode : public SceneNode {
public:
	IntersectNode(const std::string& name);
	// Interact function
	virtual bool intersect( Ray &r, Intersection &i );
private:
	
};

// Difference
class DifferenceNode : public SceneNode {
public:
	DifferenceNode(const std::string& name);
	// Interact function
	virtual bool intersect( Ray &r, Intersection &i );
private:
	
};

#endif
