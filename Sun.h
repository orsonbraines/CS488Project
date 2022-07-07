#pragma once

#include <glm/glm.hpp>

class Sun {
public:
	Sun();
	glm::mat4 getDirMat() const;
	glm::mat4 getV(const glm::vec3 &pos) const;
	glm::mat4 getP() const;
	glm::vec3 getLightDir() const;
	glm::vec3 getColour() const;
	glm::vec3 getAmbientColour() const;
	void setPhi(float phi) { m_phi = phi; }
	float getTheta() const;
	float getPhi() const { return m_phi; }
	float getTime() { return m_time; }
	float getRate() { return m_rate; }
	void tick(float t);
	void multiplyRate(float m);
	void changeTime(float delta);
private:
	float m_phi;
	float m_time;
	float m_rate;
};