#include <cstdlib>
#include <glm/ext.hpp>

#include "SmokeSystem.h"

SmokeSystem::SmokeSystem(uint numParticles) : 
							m_center(0.0f, 0.0f, 0.0f),
							m_radius(0.25f),
							m_accT(0.0f),
							m_particles(numParticles),
							m_maxAge(1000),
							m_tickNum(0),
							m_prog("shaders/smoke.vs", "shaders/smoke.fs"),
							m_PV(1.0f),
							m_vboData(numParticles * 4)
{
	float r1 = (float(rand()) / float(RAND_MAX) * 2.0f - 1.0f) * 0.00001f;
	float r2 = (float(rand()) / float(RAND_MAX) * 2.0f - 1.0f) * 0.00001f;
	m_acc1 = glm::vec3(r1, 0.0f, r2);
	r1 = (float(rand()) / float(RAND_MAX) * 2.0f - 1.0f) * 0.00001f;
	r2 = (float(rand()) / float(RAND_MAX) * 2.0f - 1.0f) * 0.00001f;
	m_acc2 = glm::vec3(r1, 0.0f, r2);

	for (int i = 0; i < m_particles.size(); ++i) {
		initParticle(m_particles[i]);
		m_particles[i].age = i % m_maxAge;
	}

	// get into a reasonable starting config
	for (int i = 0; i < m_maxAge; ++i) {
		tick();
	}

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferStorage(GL_ARRAY_BUFFER, m_vboData.size() * sizeof(float), nullptr, GL_DYNAMIC_STORAGE_BIT);

	copyParticlesToVbo();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SmokeSystem::copyParticlesToVbo() {
	for (int i = 0; i < m_particles.size(); ++i) {
		m_vboData[4 * i] = m_particles[i].pos.x;
		m_vboData[4 * i + 1] = m_particles[i].pos.y;
		m_vboData[4 * i + 2] = m_particles[i].pos.z;
		m_vboData[4 * i + 3] = 0.05f * (1.0f - float(m_particles[i].age) / float(m_maxAge));
	}

	glNamedBufferSubData(m_vbo, 0, m_vboData.size() * sizeof(float), (void*)m_vboData.data());
}

void SmokeSystem::draw() {
	copyParticlesToVbo();
	m_prog.use();
	glUniformMatrix4fv(m_prog["PV"], 1, GL_FALSE, glm::value_ptr(m_PV));
	glBindVertexArray(m_vao);

	glPointSize(10);
	glDrawArrays(GL_POINTS, 0, m_particles.size());

	glBindVertexArray(0);
}

SmokeSystem::~SmokeSystem() {
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
}

void SmokeSystem::tick() {
	if (m_accT >= 1.0f) {
		float r1 = (float(rand()) / float(RAND_MAX) * 2.0f - 1.0f) * 0.00001f;
		float r2 = (float(rand()) / float(RAND_MAX) * 2.0f - 1.0f) * 0.00001f;
		m_acc1 = m_acc2;
		m_acc2 = glm::vec3(r1, 0.0f, r2);
		m_accT -= 1.0f;
	}

	m_accT += 0.01f;
	glm::vec3 acc = m_acc2 * m_accT + m_acc1 * (1.0f - m_accT);

	for (int i = 0; i < m_particles.size(); ++i) {
		if (m_particles[i].age > m_maxAge) {
			initParticle(m_particles[i]);
		}
		m_particles[i].vel += acc;
		m_particles[i].pos += m_particles[i].vel;
		++m_particles[i].age;
	}
}

void SmokeSystem::initParticle(SmokeParticle& p) {
	p.age = 0;
	float theta = float(rand()) / float(RAND_MAX) * 2.0f * glm::pi<float>();
	p.pos = m_center + glm::vec3(glm::cos(theta) * m_radius, 0, glm::sin(theta) * m_radius);
	theta = float(rand()) / float(RAND_MAX) * 2.0f * glm::pi<float>();
	p.vel = glm::vec3(glm::cos(theta) * 0.001f, 0.01f, glm::sin(theta) * 0.001f);
}