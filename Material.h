#pragma once

#include <glm/glm.hpp>

struct Material {
	Material() : Kd(), Ks(), Ns(0.0f) {}
	glm::vec3 Kd;
	glm::vec3 Ks;
	float Ns;
};