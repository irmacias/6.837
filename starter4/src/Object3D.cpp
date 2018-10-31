#include "Object3D.h"

bool Sphere::intersect(const Ray &r, float tmin, Hit &h) const
{
    // BEGIN STARTER

    // We provide sphere intersection code for you.
    // You should model other intersection implementations after this one.

    // Locate intersection point ( 2 pts )
    const Vector3f &rayOrigin = r.getOrigin(); //Ray origin in the world coordinate
    const Vector3f &dir = r.getDirection();

    Vector3f origin = rayOrigin - _center;      //Ray origin in the sphere coordinate

    float a = dir.absSquared();
    float b = 2 * Vector3f::dot(dir, origin);
    float c = origin.absSquared() - _radius * _radius;

    // no intersection
    if (b * b - 4 * a * c < 0) {
        return false;
    }

    float d = sqrt(b * b - 4 * a * c);

    float tplus = (-b + d) / (2.0f*a);
    float tminus = (-b - d) / (2.0f*a);

    // the two intersections are at the camera back
    if ((tplus < tmin) && (tminus < tmin)) {
        return false;
    }

    float t = 10000;
    // the two intersections are at the camera front
    if (tminus > tmin) {
        t = tminus;
    }

    // one intersection at the front. one at the back 
    if ((tplus > tmin) && (tminus < tmin)) {
        t = tplus;
    }

    if (t < h.getT()) {
        Vector3f normal = r.pointAtParameter(t) - _center;
        normal = normal.normalized();
        h.set(t, this->material, normal);
        return true;
    }
    // END STARTER
    return false;
}

// Add object to group
void Group::addObject(Object3D *obj) {
    m_members.push_back(obj);
}

// Return number of objects in group
int Group::getGroupSize() const {
    return (int)m_members.size();
}

bool Group::intersect(const Ray &r, float tmin, Hit &h) const
{
    // BEGIN STARTER
    // we implemented this for you
    bool hit = false;
    for (Object3D* o : m_members) {
        if (o->intersect(r, tmin, h)) {
            hit = true;
        }
    }
    return hit;
    // END STARTER
}


Plane::Plane(const Vector3f &normal, float d, Material *m) : Object3D(m) {
	_normal = normal;
	_d = d;
	material = m;
}
bool Plane::intersect(const Ray &r, float tmin, Hit &h) const
{
	float numerator = _d - Vector3f().dot(_normal, r.getOrigin());
	float denominator = Vector3f().dot(_normal, r.getDirection());
	float t = numerator / denominator;

	if (t < tmin || t >= h.getT()) {
		return false;
	}
	h.set(t, material, _normal.normalized());
	return true;
}
bool Triangle::intersect(const Ray &r, float tmin, Hit &h) const 
{
	Vector3f r_d = r.getDirection();
	Vector3f r_o = r.getOrigin();

	Vector3f a1(_v[0].x() - _v[1].x(), _v[0].x() - _v[2].x(), r_d.x());
	Vector3f a2(_v[0].y() - _v[1].y(), _v[0].y() - _v[2].y(), r_d.y());
	Vector3f a3(_v[0].z() - _v[1].z(), _v[0].z() - _v[2].z(), r_d.z());

	Vector3f t1(_v[0].x() - _v[1].x(), _v[0].x() - _v[2].x(), _v[0].x() - r_o.x());
	Vector3f t2(_v[0].y() - _v[1].y(), _v[0].y() - _v[2].y(), _v[0].y() - r_o.y());
	Vector3f t3(_v[0].z() - _v[1].z(), _v[0].z() - _v[2].z(), _v[0].z() - r_o.z());

	Vector3f b1(_v[0].x() - r_o.x(), _v[0].x() - _v[2].x(), r_d.x());
	Vector3f b2(_v[0].y() - r_o.y(), _v[0].y() - _v[2].y(), r_d.y());
	Vector3f b3(_v[0].z() - r_o.z(), _v[0].z() - _v[2].z(), r_d.z());

	Vector3f y1(_v[0].x() - _v[1].x(), _v[0].x() - r_o.x(), r_d.x());
	Vector3f y2(_v[0].y() - _v[1].y(), _v[0].y() - r_o.y(), r_d.y());
	Vector3f y3(_v[0].z() - _v[1].z(), _v[0].z() - r_o.z(), r_d.z());

	Matrix3f A(a1, a2, a3);
	float detA = A.determinant();

	Matrix3f B(b1, b2, b3);
	Matrix3f T(t1, t2, t3);
	Matrix3f Y(y1, y2, y3);

	float beta = B.determinant() / detA;
	float gamma = Y.determinant() / detA;
	float alpha = 1 - beta - gamma;
	float t = T.determinant() / detA;

	if (t < tmin || t >= h.getT() || beta + gamma > 1 || beta < 0 || gamma < 0) {
		return false;
	}
	Vector3f normal = getNormal(0) * alpha + getNormal(1) * beta + getNormal(2) * gamma;
	h.set(t, material, normal.normalized());
	return true;
}


Transform::Transform(const Matrix4f &m,
    Object3D *obj) : _object(obj) {
	_transform = m;
	_invTransform = m.inverse();
	_transposedTransform = m.inverse().transposed();
}
bool Transform::intersect(const Ray &r, float tmin, Hit &h) const
{
	Vector4f direction = Vector4f(r.getDirection(), 0);
	Vector4f position = Vector4f(r.getOrigin(), 1);

	Vector3f oDirection = (_invTransform * direction).xyz();
	Vector3f oPosition = (_invTransform * position).xyz();
	Ray transformedRay = Ray(oPosition, oDirection);
	bool intersected = _object->intersect(transformedRay, tmin, h);
	if (intersected) {
		Vector4f normal = Vector4f(h.getNormal(), 0);
		Vector3f wNormal = (_transposedTransform * normal).xyz().normalized();
		h.set(h.getT(), h.getMaterial(), wNormal);
	}
	return intersected;
}