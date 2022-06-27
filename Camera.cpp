#include "Camera.h"

#include <glm/gtx/transform.hpp>

Camera::Camera() : pos(0.0f), pitch(0.0f), yaw(0.0f), nearZ(0.1f), farZ(10.0f), fovy(glm::radians(50.0f)), aspect(1.0) {

}

glm::mat4 Camera::getV() {
	return glm::rotate(-pitch, glm::vec3(1, 0, 0)) * glm::rotate(-yaw, glm::vec3(0, 1, 0)) * glm::translate(-pos);
}

glm::mat4 Camera::getP() {
	return glm::perspective(fovy, aspect, nearZ, farZ);
}

glm::mat4 Camera::getVInv() {
	return glm::translate(pos) * glm::rotate(yaw, glm::vec3(0, 1, 0)) * glm::rotate(pitch, glm::vec3(1, 0, 0));
}

glm::vec3 Camera::getViewDir() {
	return -glm::vec3(getVInv()[3]);
}