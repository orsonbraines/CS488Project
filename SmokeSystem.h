#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "Types.h"
#include "ShaderProgram.h"

struct SmokeParticle {
	glm::vec3 pos;
	glm::vec3 vel;
	uint age;
};

class SmokeSystem {
public:
	SmokeSystem(uint numParticles);
	~SmokeSystem();
	void tick();
	void initParticle(SmokeParticle &p);
	void setPV(const glm::mat4& PV) { m_PV = PV; }
	void draw();
private:
	void copyParticlesToVbo();

	glm::vec3 m_center;
	float m_radius;
	glm::vec3 m_acc1, m_acc2;
	float m_accT;
	std::vector<SmokeParticle> m_particles;
	uint m_maxAge;
	uint m_tickNum;

	ShaderProgram m_prog;
	GLint m_PVLoc;
	GLuint m_vbo, m_vao;
	std::vector<float> m_vboData;
	glm::mat4 m_PV;
};