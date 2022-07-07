#include <cstdlib>
#include <glm/ext.hpp>

#include "SmokeSystem.h"

static const int numCircleTriangles = 6;

SmokeSystem::SmokeSystem(uint numParticles) : 
							m_center(0.0f, 0.0f, 0.0f),
							m_radius(0.25f),
							m_accT(0.0f),
							m_vy(1.0f),
							m_particles(numParticles),
							m_maxAge(10.0),
							m_prog("shaders/smoke.vs", "shaders/smoke.fs"),
							m_PV(1.0f),
							m_vboData(numParticles * 4)
{
	m_acc1 = getRandAccel();
	m_acc2 = getRandAccel();

	for (int i = 0; i < m_particles.size(); ++i) {
		initParticle(m_particles[i], float(i) * m_maxAge / m_particles.size());
	}

	// get into a reasonable starting config
	for (int i = 0; i < 100; ++i) {
		tick(float(m_maxAge) / 100);
	}

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glGenBuffers(1, &m_dynamicVbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_dynamicVbo);
	glBufferStorage(GL_ARRAY_BUFFER, m_vboData.size() * sizeof(float), nullptr, GL_DYNAMIC_STORAGE_BIT);

	copyParticlesToVbo();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribDivisor(0, 1);
	glVertexAttribDivisor(1, 1);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &m_staticVbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_staticVbo);

	float staticVboData[3 * (numCircleTriangles + 2)];
	staticVboData[0] = 0.0f;
	staticVboData[1] = 0.0f;
	staticVboData[2] = 1.0f;
	for (int i = 0; i <= numCircleTriangles; ++i) {
		// x, y, alpha
		staticVboData[3 * i + 3] = glm::cos(i * 2.0f * glm::pi<float>() / numCircleTriangles);
		staticVboData[3 * i + 4] = glm::sin(i * 2.0f * glm::pi<float>() / numCircleTriangles);
		staticVboData[3 * i + 5] = 0.0f;
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(staticVboData), (void*)staticVboData, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(2 * sizeof(float)));
	glVertexAttribDivisor(2, 0);
	glVertexAttribDivisor(3, 0);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);


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

	glNamedBufferSubData(m_dynamicVbo, 0, m_vboData.size() * sizeof(float), (void*)m_vboData.data());
}

void SmokeSystem::draw() {
	copyParticlesToVbo();
	m_prog.use();
	glUniformMatrix4fv(m_prog["PV"], 1, GL_FALSE, glm::value_ptr(m_PV));
	glUniformMatrix4fv(m_prog["Vinv"], 1, GL_FALSE, glm::value_ptr(m_Vinv));
	glUniform1f(m_prog["radius"], 0.1f);
	glBindVertexArray(m_vao);

	//glPointSize(10);
	glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, numCircleTriangles + 2, m_particles.size());

	glBindVertexArray(0);
}

SmokeSystem::~SmokeSystem() {
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_dynamicVbo);
}

glm::vec3 SmokeSystem::getRandAccel() {
	float mult = 0.1f;
	float r1 = (float(rand()) / float(RAND_MAX) * 2.0f - 1.0f) * mult;
	float r2 = (float(rand()) / float(RAND_MAX) * 2.0f - 1.0f) * mult;
	return glm::vec3(r1, 0.0f, r2);
}

void SmokeSystem::tick(float t) {
	// pick a new wind direction every second
	if (m_accT >= 1.0f) {
		m_acc1 = m_acc2;
		m_acc2 = getRandAccel();
		m_accT -= 1.0f;
	}

	m_accT += t;
	// linear interpolate the 2 wind dircetions
	glm::vec3 acc = m_acc2 * m_accT + m_acc1 * (1.0f - m_accT);

	for (int i = 0; i < m_particles.size(); ++i) {
		if (m_particles[i].age > m_maxAge) {
			initParticle(m_particles[i], m_particles[i].age - m_maxAge);
		}
		m_particles[i].vel += acc * t;
		m_particles[i].pos += m_particles[i].vel * t;
		m_particles[i].age += t;
	}
}

void SmokeSystem::initParticle(SmokeParticle& p, float age) {
	p.age = age;
	float theta = float(rand()) / float(RAND_MAX) * 2.0f * glm::pi<float>();
	float mag = glm::sqrt(float(rand()) / float(RAND_MAX));
	p.pos = m_center + glm::vec3(glm::cos(theta) * m_radius * mag, m_vy * age, glm::sin(theta) * m_radius * mag);
	theta = float(rand()) / float(RAND_MAX) * 2.0f * glm::pi<float>();
	mag = glm::sqrt(float(rand()) / float(RAND_MAX) * 0.01f);
	p.vel = glm::vec3(glm::cos(theta) * mag, m_vy, glm::sin(theta) * mag);
}