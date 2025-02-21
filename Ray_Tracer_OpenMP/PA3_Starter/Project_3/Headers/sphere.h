#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "rtObject.h"
#include "scene.h"

class sphere : public rtObject
{
public:
	//constructor given  center, radius, and material
	sphere(glm::vec3 c, float r, int m, scene* s);

	//see descriptions in rtObject
	float testIntersection(glm::vec3 eye, glm::vec3 dir);
	glm::vec3 getNormal(glm::vec3 eye, glm::vec3 dir, float dist);
	glm::vec2 getTextureCoords(glm::vec3 eye, glm::vec3 dir, float dist);

private:
	glm::vec3 center;
	float radius;
};