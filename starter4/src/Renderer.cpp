#include "Renderer.h"

#include "ArgParser.h"
#include "Camera.h"
#include "Image.h"
#include "Ray.h"
#include "VecUtils.h"

#include <limits>


Renderer::Renderer(const ArgParser &args) :
    _args(args),
    _scene(args.input_file)
{
}

void
Renderer::Render()
{
    int w = _args.width;
    int h = _args.height;

    Image image(w, h);
    Image nimage(w, h);
    Image dimage(w, h);

    // loop through all the pixels in the image
    // generate all the samples

    // This look generates camera rays and callse traceRay.
    // It also write to the color, normal, and depth images.
    // You should understand what this code does.
    Camera* cam = _scene.getCamera();
    for (int y = 0; y < h; ++y) {
        float ndcy = 2 * (y / (h - 1.0f)) - 1.0f;
        for (int x = 0; x < w; ++x) {
            float ndcx = 2 * (x / (w - 1.0f)) - 1.0f;
            // Use PerspectiveCamera to generate a ray.
            // You should understand what generateRay() does.
            Ray r = cam->generateRay(Vector2f(ndcx, ndcy));

            Hit h;
            Vector3f color = traceRay(r, cam->getTMin(), _args.bounces, h);

            image.setPixel(x, y, color);
            nimage.setPixel(x, y, (h.getNormal() + 1.0f) / 2.0f);
            float range = (_args.depth_max - _args.depth_min);
            if (range) {
                dimage.setPixel(x, y, Vector3f((h.t - _args.depth_min) / range));
            }
        }
    }
    // END SOLN

    // save the files 
    if (_args.output_file.size()) {
        image.savePNG(_args.output_file);
    }
    if (_args.depth_file.size()) {
        dimage.savePNG(_args.depth_file);
    }
    if (_args.normals_file.size()) {
        nimage.savePNG(_args.normals_file);
    }
}



Vector3f
Renderer::traceRay(const Ray &r,
    float tmin,
    int bounces,
    Hit &h) const
{
    // The starter code only implements basic drawing of sphere primitives.
    // You will implement phong shading, recursive ray tracing, and shadow rays.

    // TODO: IMPLEMENT 
	if (_scene.getGroup()->intersect(r, tmin, h)) {
        Vector3f diffuseColor = h.getMaterial()->getDiffuseColor();
		Vector3f I = _scene.getAmbientLight() * diffuseColor;
		Vector3f p = r.pointAtParameter(h.getT());

		Vector3f specularColor = h.getMaterial()->getSpecularColor();
		bool isSpecular = specularColor[0] > 0 || specularColor[1] > 0 || specularColor[2] > 0;
		if (isSpecular && bounces > 0) {
			Vector3f E = r.getDirection();
			Vector3f N = h.getNormal();
			Vector3f R = E - 2 * (Vector3f::dot(E, N) * N);
			
			Ray reflectedRay = Ray(p, R.normalized());
			Hit newHit = Hit();
			Vector3f reflection = traceRay(reflectedRay, 0.0001, bounces - 1, newHit);
			I += specularColor * reflection;
		}
		for (int i = 0; i < _scene.getNumLights(); ++i) {
			Light* light = _scene.getLight(i);

			Vector3f directionToLight = Vector3f(0, 0, 0);
			Vector3f intensity = Vector3f(0, 0, 0);
			float distance;
			light->getIllumination(p, directionToLight, intensity, distance);
			Vector3f shading = h.getMaterial()->shade(r, h, directionToLight, intensity);
			directionToLight = directionToLight.normalized();
			if (_args.shadows) {
				Ray surfaceToLight = Ray(p, directionToLight);
				Hit shadowHit = Hit();
				if (_scene.getGroup()->intersect(surfaceToLight, 0.0001, shadowHit) && shadowHit.getT() < distance) {
					continue;
				}
			}

			I += shading;
		}
		return I;
    } else {
        return _scene.getBackgroundColor(r.getDirection());
    };
}

