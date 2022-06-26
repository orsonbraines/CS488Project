#include "Sun.h"

#include <glm/gtx/transform.hpp>

Sun::Sun() : m_theta(glm::radians(30.0f)), m_phi(glm::radians(30.0f)) {

}

glm::mat4 Sun::getDirMat() const {
	glm::mat4 m(glm::vec4(1,0,0,0), glm::vec4(0,0,-1,0), glm::vec4(0,1,0,0), glm::vec4(0,0,0,1));
	return glm::rotate(m_theta, glm::vec3(0,0,1)) * glm::rotate(m_phi, glm::vec3(1, 0, 0)) * m;
}

glm::mat4 Sun::getV(const glm::vec3& pos) const {
	// inv(rotation * translate)
	return glm::transpose(getDirMat()) * glm::translate(-pos);
}

glm::mat4 Sun::getP() const {
	glm::mat4 P = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, -20.0f, 20.0f);
	return P;
}

// direction to the light
glm::vec3 Sun::getLightDir() const {
	return glm::vec3(getDirMat()[2]);
}
