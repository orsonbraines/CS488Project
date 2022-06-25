#pragma once

#include <glm/glm.hpp>

class Sun {
public:
	Sun();
	glm::mat4 getDirMat() const;
	glm::mat4 getV(const glm::vec3 &pos) const;
	glm::mat4 getP() const;
	glm::vec3 getLightDir() const;
private:
	float theta, phi;
};