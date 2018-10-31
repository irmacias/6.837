#include "Material.h"
Vector3f Material::shade(const Ray &ray,
    const Hit &hit,
    const Vector3f &dirToLight,
    const Vector3f &lightIntensity)
{

	//diffuse
	Vector3f E = ray.getDirection();
	Vector3f N = hit.getNormal().normalized();

	float dot = Vector3f::dot(dirToLight, N);
	float clamped_diffuse = 0;
	if (dot > 0) {
		clamped_diffuse = dot;
	}
	
	Vector3f i_diffuse = clamped_diffuse * lightIntensity * _diffuseColor;
	
	//specular
	Vector3f R = E - 2 * (Vector3f::dot(E, N) * N);
	dot = Vector3f::dot(dirToLight, R);
	float clamped_specular = 0;
	if (dot > 0) {
		clamped_specular = dot;
	}

	Vector3f i_specular = pow(clamped_specular, _shininess) * lightIntensity * _specularColor;
	
	return i_diffuse + i_specular;
}
