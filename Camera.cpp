#include "Camera.h"

#include <glm/gtx/transform.hpp>

Camera::Camera() :  m_pos(0.0f), 
					m_pitch(0.0f), 
					m_yaw(0.0f), 
					m_nearZ(0.1f), 
					m_farZ(10.0f), 
					m_fovy(glm::radians(50.0f)), 
					m_aspect(1.0) {}

glm::mat4 Camera::getV() {
	return glm::rotate(-m_pitch, glm::vec3(1, 0, 0)) * glm::rotate(-m_yaw, glm::vec3(0, 1, 0)) * glm::translate(-m_pos);
}

glm::mat4 Camera::getP() {
	return glm::perspective(m_fovy, m_aspect, m_nearZ, m_farZ);
}

glm::mat4 Camera::getVInv() {
	return glm::translate(m_pos) * glm::rotate(m_yaw, glm::vec3(0, 1, 0)) * glm::rotate(m_pitch, glm::vec3(1, 0, 0));
}

glm::vec3 Camera::getViewDir() {
	return -glm::vec3(getVInv()[3]);
}