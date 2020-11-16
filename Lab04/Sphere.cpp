//============================================================
// STUDENT NAME: 
// STUDENT NO.: 
// NUS EMAIL ADDRESS: 
// COMMENTS TO GRADER: 
// 
// ============================================================

#include <cmath>
#include "Sphere.h"

using namespace std;

double isNegative(double d) {
	return d < 0 ? 0 : d;
}


bool Sphere::hit(const Ray& r, double tmin, double tmax, SurfaceHitRecord& rec) const
{
	// r = r_p + alpha d;
	// norm(r - r_c) - R^2 = 0; // tbh i didn't uds how I can use this from the tutorial
	// The solution was based on lecture 9 slide 24

	Vector3d rayOrigin = r.origin() - center;							// vector from center of circle to origin
	double a = 1;                                                       // ray direction . ray direction = 1
	double b = 2 * dot(r.direction(), rayOrigin);                       // 2 * ray direction . ray origin
	double c = dot(rayOrigin, rayOrigin) - radius * radius;             // ray origin . ray origin - r^2
	double d = b * b - 4 * a * c;                                       // b^2 - 4ac

	if (d < 0) { // 0 real solution
		return false;
	}

	// 1 or more real solution
	double rootD = sqrt(d);                                         // square root d
	double t1 = isNegative((-b + rootD) / (2 * a));
	double t2 = isNegative((-b - rootD) / (2 * a));

	// by de Morgan's theorem, equivalent to !t1 && !t2
	if (!(t1 || t2)) { // if both are 0, means the ray is not in the right direction
		return false;
	}

	// shortest distance from ray to the circle
	double shortestT = t1 > t2 ? t2 : t1;

	// check if the point is within t min and t max
	if (shortestT >= tmin && shortestT <= tmax) {
		// we have a hit -- populate hit record
		rec.t = shortestT;
		rec.p = r.pointAtParam(shortestT);
		rec.normal = (rec.p - center).unitVector();
		rec.mat_ptr = matp;
		return true;
	}


	return false; // You can remove/change this if needed.
}


bool Sphere::shadowHit(const Ray& r, double tmin, double tmax) const
{
	Vector3d rayOrigin = r.origin() - center;							// vector from center of circle to origin
	double a = 1;                                                       // ray direction . ray direction = 1
	double b = 2 * dot(r.direction(), rayOrigin);                       // 2 * ray direction . ray origin
	double c = dot(rayOrigin, rayOrigin) - radius * radius;             // ray origin . ray origin - r^2
	double d = b * b - 4 * a * c;                                       // b^2 - 4ac

	if (d >= 0) { // 1 or more real solution

		double rootD = sqrt(d);                                         // square root d
		double t1 = isNegative((-b + rootD) / (2 * a));
		double t2 = isNegative((-b - rootD) / (2 * a));

		// shortest distance from ray to the circle
		double shortestT = t1 > t2 ? t2 : t1;

		return (shortestT >= tmin && shortestT <= tmax);
	}

	// 0 solution
	return false; // You can remove/change this if needed.
}
