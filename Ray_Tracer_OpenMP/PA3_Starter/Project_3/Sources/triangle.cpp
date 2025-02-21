#include "triangle.h"

//constructor given  center, radius, and material
triangle::triangle(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, float tx0, float tx1, float tx2, float ty0, float ty1, float ty2, int m, scene* s) : rtObject(s)
{
	point0 = p0;
	point1 = p1;
	point2 = p2;

	texX0 = tx0;
	texX1 = tx1;
	texX2 = tx2;
	texY0 = ty0;
	texY1 = ty1;
	texY2 = ty2;
	matIndex = m;
	myScene = s;
}

float triangle::testIntersection(glm::vec3 eye, glm::vec3 dir)
{
	//see the book/slides for a description of how to use Cramer's rule to solve
	//for the intersection(s) of a line and a plane, implement it here and
	//return the minimum distance (if barycentric coordinates indicate it hit
	//the triangle) otherwise 9999999
	glm::vec3 a = point0;
	glm::vec3 b = point1;
	glm::vec3 c = point2;

	float t = glm::determinant(glm::mat3(   a.x - b.x, a.x - c.x, a.x - eye.x,
											a.y - b.y, a.y - c.y, a.y - eye.y,
											a.z - b.z, a.z - c.z, a.z - eye.z)) /

				glm::determinant(glm::mat3( a.x - b.x, a.x - c.x, dir.x,
											a.y - b.y, a.y - c.y, dir.y,
											a.z - b.z, a.z - c.z, dir.z));
	if (t < 0 || t>9999999) {
		return 9999999;
	}


	float gamma = glm::determinant(glm::mat3(   a.x - b.x, a.x - eye.x, dir.x,
												a.y - b.y, a.y - eye.y, dir.y,
												a.z - b.z, a.z - eye.z, dir.z)) /

					glm::determinant(glm::mat3( a.x - b.x, a.x - c.x, dir.x,
												a.y - b.y, a.y - c.y, dir.y,
												a.z - b.z, a.z - c.z, dir.z));
	if (gamma < 0 || gamma > 1) {
		return 9999999;
	}


	float beta = glm::determinant(glm::mat3(a.x - eye.x, a.x - c.x, dir.x,
											a.y - eye.y, a.y - c.y, dir.y,
											a.z - eye.z, a.z - c.z, dir.z)) /

				glm::determinant(glm::mat3( a.x - b.x, a.x - c.x, dir.x,
											a.y - b.y, a.y - c.y, dir.y,
											a.z - b.z, a.z - c.z, dir.z));
	if (beta < 0 || beta > 1 - gamma) {
		return 9999999;
	}

	return t;
}

glm::vec3 triangle::getNormal(glm::vec3 eye, glm::vec3 dir, float dist)
{
	//construct the barycentric coordinates for the plane
	glm::vec3 bary1 = point1 - point0;
	glm::vec3 bary2 = point2 - point0;

	//cross them to get the normal to the plane
	//note that the normal points in the direction given by right-hand rule
	//(this can be important for refraction to know whether you are entering or leaving a material)
	glm::vec3 normal = glm::normalize(glm::cross(bary1, bary2));
	return normal;
}

glm::vec2 triangle::getTextureCoords(glm::vec3 eye, glm::vec3 dir, float dist)
{
	//find alpha and beta (parametric distance along barycentric coordinates)
	//use these in combination with the known texture surface location of the vertices
	//to find the texture surface location of the point you are seeing
	glm::vec3 a = point0;
	glm::vec3 b = point1;
	glm::vec3 c = point2;
	float beta = glm::determinant(glm::mat3(a.x - eye.x, a.x - c.x, dir.x,
											a.y - eye.y, a.y - c.y, dir.y,
											a.z - eye.z, a.z - c.z, dir.z)) /

				glm::determinant(glm::mat3(	a.x - b.x, a.x - c.x, dir.x,
											a.y - b.y, a.y - c.y, dir.y,
											a.z - b.z, a.z - c.z, dir.z));

	float gamma = glm::determinant(glm::mat3(	a.x - b.x, a.x - eye.x, dir.x,
												a.y - b.y, a.y - eye.y, dir.y,
												a.z - b.z, a.z - eye.z, dir.z)) /

					glm::determinant(glm::mat3(	a.x - b.x, a.x - c.x, dir.x,
												a.y - b.y, a.y - c.y, dir.y,
												a.z - b.z, a.z - c.z, dir.z));
	float alpha = 1 - beta - gamma;
	float texX = alpha * texX0 + beta * texX1 + gamma * texX2;
	float texY = alpha * texY0 + beta * texY1 + gamma * texY2;

	glm::vec2 coords = glm::vec2 (texX, texY);
	return coords;
}