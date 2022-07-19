#include "Collision.h"


AABB::AABB(float left, float right, float bottom, float top, float back, float front) : 
	m_bounds{glm::vec3(left, bottom, back), glm::vec3(right, top, front)} {}

AABB::AABB(const glm::vec3& minBound, const glm::vec3& maxBound) :
	m_bounds{ minBound, maxBound } {}

AABB::AABB() :
	m_bounds{ glm::vec3(0.0f), glm::vec3(1.0f) } {}

AABB AABB::transform(const glm::mat4 &M) const {
	glm::vec3 minBound, maxBound;

	for (int i = 0; i < 8; ++i) {
		glm::vec4 p((i & 1) ? m_bounds[0].x : m_bounds[1].x, (i & 2) ? m_bounds[0].y : m_bounds[1].y, (i & 4) ? m_bounds[0].z : m_bounds[1].z, 1.0f);
		p = M * p;
		if (i == 0) {
			minBound = glm::vec3(p);
			maxBound = glm::vec3(p);
		} else {
			minBound = glm::min(minBound, glm::vec3(p));
			maxBound = glm::max(maxBound, glm::vec3(p));
		}
	}

	return AABB(minBound, maxBound);
}

Sphere::Sphere(float radius, const glm::vec3& pos) : m_radius(radius), m_pos(pos) {}


// based on the pseudo-code in the Lec16 sides
bool intersects(const AABB& aabb, const Sphere& sphere) {
	float d = 0;
	for (int i = 0; i < 3; i++) {
		float e = sphere.m_pos[i] - aabb.m_bounds[0][i];
		if (e < 0) {
			if (e < -sphere.m_radius) {
				return false;
			}
			d += e * e;
		}
		e = sphere.m_pos[i] - aabb.m_bounds[1][i];
		if (e > 0) {
			if (e > sphere.m_radius) {
				return false;
			}
			d += e * e;
		}
		if (d > sphere.m_radius * sphere.m_radius) {
			return false;
		}
	}
	return true;
}