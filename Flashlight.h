#pragma once

#include <glm/glm.hpp>

#include "Camera.h"


class Flashlight {
public:
	Flashlight(Camera& cam) : m_cam(cam), m_yaw(0.0f), m_pitch(0.0f) {}
	glm::vec3 getPos() const;
	glm::vec3 getDir() const;
	glm::vec3 getColour() const;
	float getHardCutoff() const;
	float getSoftCutoff() const;
	void changeAngle(float yaw, float pitch);
private:
	Camera& m_cam;
	float m_yaw, m_pitch;
};