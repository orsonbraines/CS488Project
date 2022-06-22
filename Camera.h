#pragma once

#include <glm/ext.hpp>

class Camera {
public:
	Camera();
	glm::mat4 getV();
	glm::mat4 getP();
	glm::mat4 getVInv();
	glm::vec3 getViewDir();

	glm::vec3 pos;
	float pitch, yaw;
	float nearZ, farZ;
	float fovy;
	float aspect;
};