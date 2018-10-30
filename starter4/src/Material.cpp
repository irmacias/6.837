#include "Material.h"
Vector3f Material::shade(const Ray &ray,
    const Hit &hit,
    const Vector3f &dirToLight,
    const Vector3f &lightIntensity)
{

	//diffuse
	Vector3f E = ray.getDirection();
	Vector3f N = hit.getNormal();

	float dot = Vector3f().dot(dirToLight, N);
	float clamped_diffuse = 0;
	if (dot > 0) {
		clamped_diffuse = dot;
	}
	Vector3f i_diffuse = clamped_diffuse * _diffuseColor * lightIntensity;
	
	//specular
	Vector3f R = E - 2 * (Vector3f().dot(E, N) * N);
	float dot = Vector3f().dot(R, hit.getNormal());
	float clamped_specular = 0;
	if (dot > 0) {
		clamped_specular = dot;
	}

	Vector3f i_specular = pow(clamped_specular, _shininess) * _specularColor * lightIntensity;
	
	return i_diffuse + i_specular;
}
