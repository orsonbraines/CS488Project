#pragma once

#include <glm/glm.hpp>

#include "Camera.h"


class Flashlight {
public:
	Flashlight(Camera& cam) : m_cam(cam), m_yaw(0.0f), m_pitch(0.0f), m_on(false) {}
	glm::vec3 getPos() const;
	glm::vec3 getDir() const;
	glm::vec3 getColour() const;
	glm::mat4 getV() const;
	glm::mat4 getP() const;
	float getHardCutoff() const;
	float getSoftCutoff() const;
	void changeAngle(float yaw, float pitch);
	void toggle() { m_on = !m_on; }
private:
	Camera& m_cam;
	float m_yaw, m_pitch;
	bool m_on;
};