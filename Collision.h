#pragma once

#include <glm/glm.hpp>
#include <array>

struct AABB {
	AABB();
	AABB(float left, float right, float bottom, float top, float back, float front);
	AABB(const glm::vec3 &minBound, const glm::vec3& maxBound);
	std::array<glm::vec3, 2> m_bounds;
	AABB transform(const glm::mat4 &M) const;
};

struct Sphere {
	Sphere(float radius, const glm::vec3& pos);
	float m_radius;
	glm::vec3 m_pos;
};

bool intersects(const AABB& aabb, const Sphere& sphere);