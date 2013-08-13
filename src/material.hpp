#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include "algebra.hpp"

class Material {
public:
	virtual ~Material();
	virtual void apply_gl() const = 0;
	virtual Colour getDiffuse() = 0;
	virtual Colour getSpecular() = 0;
	virtual double getShininess() = 0;
	virtual double getRefractIndex() = 0;
	virtual double getReflectIndex() = 0;

protected:
	Material()
	{
	}
};

class PhongMaterial : public Material {
public:
	PhongMaterial(const Colour& kd, const Colour& ks, double shininess, double refract, double reflect);
	virtual ~PhongMaterial();

	virtual void apply_gl() const;
	virtual Colour getDiffuse() { return m_kd; }
	virtual Colour getSpecular() { return m_ks; }
	virtual double getShininess() { return m_shininess; }
	virtual double getRefractIndex() { return m_refract_index; }
	virtual double getReflectIndex() { return m_reflect_index; }

private:
	Colour m_kd;
	Colour m_ks;

	double m_shininess;
	double m_refract_index;		// Below 1 indicates that the object is not transparent
	double m_reflect_index;
};


#endif
