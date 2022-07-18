#include <glm/gtx/transform.hpp>

#include "Flashlight.h"

glm::vec3 Flashlight::getPos() const {
    glm::mat3 dirs(m_cam.getVInv());
    glm::vec3 right = dirs[0];
    glm::vec3 up = dirs[1];
    return m_cam.m_pos - 1.0f * up + 0.5f * right;
}

glm::vec3 Flashlight::getDir() const {
    return (m_cam.getVInv() * glm::rotate(m_yaw, glm::vec3(0, 1, 0)) * glm::rotate(m_pitch, glm::vec3(1, 0, 0)))[2];
}

glm::mat4 Flashlight::getV() const {
    return glm::rotate(-m_pitch, glm::vec3(1, 0, 0)) * glm::rotate(-m_yaw, glm::vec3(0, 1, 0)) * glm::rotate(-m_cam.m_pitch, glm::vec3(1, 0, 0)) * glm::rotate(-m_cam.m_yaw, glm::vec3(0, 1, 0)) * glm::translate(-getPos());
    //return m_cam.getV();
}

glm::mat4 Flashlight::getP() const {
    return glm::perspective(glm::radians(60.0f), 1.0f, 0.5f, 15.0f);
}

glm::vec3 Flashlight::getColour() const {
    return m_on ? glm::vec3(2.0f, 2.0f, 2.0f) : glm::vec3(0.0f);
}

float Flashlight::getHardCutoff() const {
    return glm::cos(glm::radians(30.0f));
}

float Flashlight::getSoftCutoff() const {
    return glm::cos(glm::radians(20.0f));
}

void Flashlight::changeAngle(float yaw, float pitch) {
    m_yaw = glm::clamp(m_yaw + yaw, -glm::radians(45.0f), glm::radians(45.0f));
    m_pitch = glm::clamp(m_pitch + pitch, -glm::radians(30.0f), glm::radians(30.0f));
}