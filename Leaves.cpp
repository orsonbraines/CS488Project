#include "Leaves.h"

Leaves::Leaves(float lu, float lv) : m_id(255) {
	m_tex.setWrapS(GL_REPEAT);
	m_tex.setWrapT(GL_REPEAT);
	m_tex.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	m_tex.loadDDS("textures/leaves.dds");

	float vboData[8*8];
	int idx = 0;
	// front side
	for (int i = 0; i < 4; ++i) {
		// pos
		vboData[idx++] = i % 2;
		vboData[idx++] = i / 2;
		vboData[idx++] = 0;
		// normal
		vboData[idx++] = 0;
		vboData[idx++] = 0;
		vboData[idx++] = 1;
		// u,v
		vboData[idx++] = (i % 2) * lu;
		vboData[idx++] = (i / 2) * lv;
	}
	// back side
	for (int i = 0; i < 4; ++i) {
		// pos
		vboData[idx++] = 1 - i % 2;
		vboData[idx++] = i / 2;
		vboData[idx++] = 0;
		// normal
		vboData[idx++] = 0;
		vboData[idx++] = 0;
		vboData[idx++] = -1;
		// u,v
		vboData[idx++] = (i % 2) * lu;
		vboData[idx++] = (i / 2) * lv;
	}

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vboData), (void*)vboData, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Leaves::~Leaves() {
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
}

void Leaves::draw(const ShaderProgram &p) {
	glActiveTexture(GL_TEXTURE0);
	m_tex.bind();
	glUniform1i(p["sampler"], 0);
	glUniform3f(m_KsUniformLoc, 0, 0, 0);
	glUniform1f(m_NsUniformLoc, 1.0f);
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
	glBindVertexArray(0);
}

void Leaves::setUniformLocations(GLint KsUniformLoc, GLint NsUniformLoc) {
	m_KsUniformLoc = KsUniformLoc;
	m_NsUniformLoc = NsUniformLoc;
}