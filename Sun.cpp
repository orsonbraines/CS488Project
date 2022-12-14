#include "Sun.h"

#include <glm/gtx/transform.hpp>

Sun::Sun() : m_phi(glm::radians(30.0f)), m_time(12.0f), m_rate(1.0f) {

}

float Sun::getTheta() const { return (m_time - 12.0f) * glm::pi<float>() / 12.0f; }

glm::mat4 Sun::getDirMat() const {
	glm::mat4 m(glm::vec4(1,0,0,0), glm::vec4(0,0,-1,0), glm::vec4(0,1,0,0), glm::vec4(0,0,0,1));
	return glm::rotate(getTheta(), glm::vec3(0, 0, 1)) * glm::rotate(m_phi, glm::vec3(1, 0, 0)) * m;
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

glm::vec3 Sun::getColour() const {
	return glm::max(glm::cos(getTheta()), 0.0f) * glm::vec3(0.8f);
}

glm::vec3 Sun::getAmbientColour() const {
	glm::vec3 dark(0.12f, 0.12f, 0.12f);
	glm::vec3 light(0.4f, 0.4f, 0.4f);
	if (m_time <= 6.0f) {
		return dark;
	}
	else if (m_time <= 8.0f) {
		float t = (m_time - 6.0f) / (8.0f - 6.0f);
		return t * light + (1 - t) * dark;
	}
	else if (m_time <= 16.0f) {
		return light;
	}
	else if (m_time <= 18.0f) {
		float t = (m_time - 16.0f) / (18.0f - 16.0f);
		return t * dark + (1 - t) * light;
	}
	else {
		return dark;
	}
}

float Sun::getBrightness() const {
	if (m_time <= 6.0f) {
		return 0.0f;
	}
	else if (m_time <= 8.0f) {
		float t = (m_time - 6.0f) / (8.0f - 6.0f);
		return t;
	}
	else if (m_time <= 16.0f) {
		return 1.0f;
	}
	else if (m_time <= 18.0f) {
		float t = (m_time - 16.0f) / (18.0f - 16.0f);
		return 1 - t;
	}
	else {
		return 0.0f;
	}
}

bool Sun::isNight() const {
	return m_time <= 6.0f || m_time >= 18.0f;
}

// at rate = 1.0, six hours passes per one minute real time
void Sun::tick(float t) { 
	m_time += t / 10.0f * m_rate;
	if (m_time >= 24.0f) {
		m_time -= 24.0f;
	}
}

void Sun::multiplyRate(float m) {
	const float maxRate = 16.0f;
	const float minRate = 1.0 / 16.0f;
	m_rate *= m;
	if (m_rate > maxRate) {
		m_rate = maxRate;
	}
	if (m_rate < minRate) {
		m_rate = minRate;
	}
}

void Sun::changeTime(float delta) {
	m_time += delta;
	while (m_time < 0.0f) {
		m_time += 24.0f;
	}
	while (m_time >= 24.0f) {
		m_time -= 24.0f;
	}
}