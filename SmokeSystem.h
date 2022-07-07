#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "Types.h"
#include "ShaderProgram.h"

struct SmokeParticle {
	glm::vec3 pos;
	glm::vec3 vel;
	float age;
};

class SmokeSystem {
public:
	SmokeSystem(uint numParticles);
	~SmokeSystem();
	void tick(float t);
	void initParticle(SmokeParticle &p, float age);
	void setPV(const glm::mat4& PV) { m_PV = PV; }
	void setVinv(const glm::mat4& Vinv) { m_Vinv = Vinv; }
	void draw();
private:
	void copyParticlesToVbo();
	glm::vec3 getRandAccel();

	glm::vec3 m_center;
	float m_radius;
	glm::vec3 m_acc1, m_acc2;
	float m_vy;
	float m_accT;
	std::vector<SmokeParticle> m_particles;
	float m_maxAge;

	ShaderProgram m_prog;
	GLuint m_staticVbo, m_dynamicVbo, m_vao;
	std::vector<float> m_vboData;
	glm::mat4 m_PV, m_Vinv;
};