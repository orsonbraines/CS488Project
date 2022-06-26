#pragma once

#include <glm/glm.hpp>

class Sun {
public:
	Sun();
	glm::mat4 getDirMat() const;
	glm::mat4 getV(const glm::vec3 &pos) const;
	glm::mat4 getP() const;
	glm::vec3 getLightDir() const;
	void setTheta(float theta) { m_theta = theta; }
	void setPhi(float phi) { m_phi = phi; }
	float getTheta() { return m_theta; }
	float getPhi() { return m_phi; }
private:
	float m_theta, m_phi;
};