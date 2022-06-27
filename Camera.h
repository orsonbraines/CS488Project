#pragma once

#include <glm/ext.hpp>

class Camera {
public:
	Camera();
	glm::mat4 getV();
	glm::mat4 getP();
	glm::mat4 getVInv();
	glm::vec3 getViewDir();

	glm::vec3 m_pos;
	float m_pitch, m_yaw;
	float m_nearZ, m_farZ;
	float m_fovy;
	float m_aspect;
};