#include "sphere.h"

//constructor given  center, radius, and material
sphere::sphere(glm::vec3 c, float r, int m, scene* s) : rtObject(s)
{
	center = c;
	radius = r;
	matIndex = m;
	myScene = s;
}

float sphere::testIntersection(glm::vec3 eye, glm::vec3 dir)
{
	//see the book for a description of how to use the quadratic rule to solve
	//for the intersection(s) of a line and a sphere, implement it here and
	//return the minimum positive distance or 9999999 if none

	glm::vec3 e = eye;
	glm::vec3 c = center;
	glm::vec3 d = dir;
	float R = radius;
	float A = dot(d, d);
	float B = 2.0f * dot(d, (e - c));
	float C = dot((e - c), (e - c)) - R * R;

	float discr = B * B - 4.0f * A * C;

	if (discr < 0) {
		return 9999999;
	}

	float t1 = (-B + sqrt(discr)) / (2.0f * (A));

	float t2 = (-B - sqrt(discr)) / (2.0f * (A));

	if (t2 < 0 && t1 < 0) {
		return 9999999;
	}
	if (t1 < t2) {
		if (t1 < 0) {
			return t2;
		}
		return t1;
	}
	if (t2 < 0) {
		return t1;
	}
	return t2;
}

glm::vec3 sphere::getNormal(glm::vec3 eye, glm::vec3 dir, float dist)
{
	//once you can test for intersection,
	//simply add (distance * view direction) to the eye location to get surface location,
	//then subtract the center location of the sphere to get the normal out from the sphere
	glm::vec3 normal = ((eye + dir * dist) - center);

	//dont forget to normalize
	normal = glm::normalize(normal);
	
	return normal;
}

glm::vec2 sphere::getTextureCoords(glm::vec3 eye, glm::vec3 dir, float dist)
{
	//find the normal as in getNormal
	glm::vec3 normal = getNormal(eye, dir, dist);

	//use these to find spherical coordinates
	glm::vec3 x(1, 0, 0);
	glm::vec3 z(0, 0, 1);

	//phi is the angle down from z
	//theta is the angle from x curving toward y
	//hint: angle between two vectors is the acos() of the dot product

	//find phi using normal and z
	float phi = acos(dot(normal, z) / (length(normal) * length(z)));

	//find the x-y projection of the normal
	glm::vec3 proj = normal - dot(normal, z) / (glm::length(z) * glm::length(z)) * z;

	//find theta using the x-y projection and x
	float theta = acos(dot(proj, x) / (length(proj) * length(x)));


	//if x-y projection is in quadrant 3 or 4, then theta=2*PI-theta
	if (proj.y < 0) {
		theta = 2.0 * 3.1416 - theta;
	}
	
	//give textured spheres the illusion of rotation by rotating texture
	theta += (float)frame * 0.05f;
	while (theta > 2.0f * 3.1416f) {
		theta -= (2.0f * 3.1416f);
	}
	
	//return coordinates scaled to be between 0 and 1
	glm::vec2 coords = glm::vec2(theta / (2.0 * 3.1416), phi / 3.1416);
	return coords;
}