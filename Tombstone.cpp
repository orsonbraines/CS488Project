#include <glm/ext.hpp>

#include "Tombstone.h"

Tombstone::Tombstone() : m_id(222) {
	const int floatsPerVertex = 14;
	const int nVertsArch = 21;
	const float archHeight = 0.3f;
	m_nVertices = (nVertsArch + 2) * 4;

	const int vboSize = floatsPerVertex * m_nVertices * sizeof(float);
	float* vboData = new float[floatsPerVertex * m_nVertices];
	int idx = 0;

	for (int z = 0; z < 2; ++z) {
		/* bottom left vertex */
		// pos
		vboData[idx++] = 0.0f;
		vboData[idx++] = 0.0f;
		vboData[idx++] = z;
		// normal
		vboData[idx++] = 0.0f;
		vboData[idx++] = 0.0f;
		vboData[idx++] = z == 1 ? 1.0f : -1.0f;
		// uv
		// we only want the front face to be bump mapped, so we give the other face out of range uv's 
		vboData[idx++] = z == 1 ? 0.0f : 2.0f;
		vboData[idx++] = z == 1 ? 0.0f : 2.0f;
		// udir
		vboData[idx++] = 1.0f;
		vboData[idx++] = 0.0f;
		vboData[idx++] = 0.0f;
		// vdir
		vboData[idx++] = 0.0f;
		vboData[idx++] = 1.0f;
		vboData[idx++] = 0.0f;

		/* bottom right vertex */
		// pos
		vboData[idx++] = 1.0f;
		vboData[idx++] = 0.0f;
		vboData[idx++] = z;
		// normal
		vboData[idx++] = 0.0f;
		vboData[idx++] = 0.0f;
		vboData[idx++] = z == 1 ? 1.0f : -1.0f;
		// uv
		vboData[idx++] = z == 1 ? 1.0f : 2.0f;
		vboData[idx++] = z == 1 ? 0.0f : 2.0f;
		// udir
		vboData[idx++] = 1.0f;
		vboData[idx++] = 0.0f;
		vboData[idx++] = 0.0f;
		// vdir
		vboData[idx++] = 0.0f;
		vboData[idx++] = 1.0f;
		vboData[idx++] = 0.0f;

		for (int i = 0; i < nVertsArch; ++i) {
			float theta = i / float(nVertsArch - 1) * glm::pi<float>();
			/* arch vertex */
			// pos
			vboData[idx++] = 0.5f + 0.5 * glm::cos(theta);
			vboData[idx++] = 1.0f + archHeight * glm::sin(theta);
			vboData[idx++] = z;
			// normal
			vboData[idx++] = 0.0f;
			vboData[idx++] = 0.0f;
			vboData[idx++] = z == 1 ? 1.0f : -1.0f;
			// uv
			vboData[idx++] = z == 1 ? 0.5f + 0.5 * glm::cos(theta) : 2.0f;
			vboData[idx++] = z == 1 ? 1.0f + archHeight * glm::sin(theta) : 2.0f;
			// udir
			vboData[idx++] = 1.0f;
			vboData[idx++] = 0.0f;
			vboData[idx++] = 0.0f;
			// vdir
			vboData[idx++] = 0.0f;
			vboData[idx++] = 1.0f;
			vboData[idx++] = 0.0f;
		}
	}

	// Repeat the vertices with diff normals for the edges
	for (int z = 0; z < 2; ++z) {
		/* bottom left vertex */
		// pos
		vboData[idx++] = 0.0f;
		vboData[idx++] = 0.0f;
		vboData[idx++] = z;
		// normal
		vboData[idx++] = -1.0f;
		vboData[idx++] = 0.0f;
		vboData[idx++] = 0.0f;
		// uv
		vboData[idx++] = 2.0f;
		vboData[idx++] = 2.0f;
		// udir
		vboData[idx++] = 1.0f;
		vboData[idx++] = 0.0f;
		vboData[idx++] = 0.0f;
		// vdir
		vboData[idx++] = 0.0f;
		vboData[idx++] = 1.0f;
		vboData[idx++] = 0.0f;

		/* bottom right vertex */
		// pos
		vboData[idx++] = 1.0f;
		vboData[idx++] = 0.0f;
		vboData[idx++] = z;
		// normal
		vboData[idx++] = 1.0f;
		vboData[idx++] = 0.0f;
		vboData[idx++] = 0.0f;
		// uv
		vboData[idx++] = 2.0f;
		vboData[idx++] = 2.0f;
		// udir
		vboData[idx++] = 1.0f;
		vboData[idx++] = 0.0f;
		vboData[idx++] = 0.0f;
		// vdir
		vboData[idx++] = 0.0f;
		vboData[idx++] = 1.0f;
		vboData[idx++] = 0.0f;

		for (int i = 0; i < nVertsArch; ++i) {
			float theta = i / float(nVertsArch - 1) * glm::pi<float>();
			/* arch vertex */
			// pos
			vboData[idx++] = 0.5f + 0.5 * glm::cos(theta);
			vboData[idx++] = 1.0f + archHeight * glm::sin(theta);
			vboData[idx++] = z;
			// normal
			vboData[idx++] = glm::cos(theta);
			vboData[idx++] = archHeight * glm::sin(theta);
			vboData[idx++] = 0.0f;
			// uv
			vboData[idx++] = 2;
			vboData[idx++] = 2;
			// udir
			vboData[idx++] = 1.0f;
			vboData[idx++] = 0.0f;
			vboData[idx++] = 0.0f;
			// vdir
			vboData[idx++] = 0.0f;
			vboData[idx++] = 1.0f;
			vboData[idx++] = 0.0f;
		}
	}

	ushort* iboData = new ushort[m_nVertices];
	size_t iboSize = (m_nVertices) * sizeof(ushort);
	const int vertsPerFace = m_nVertices / 4;
	idx = 0;
	for (int i = 0; i < vertsPerFace; ++i) {
		iboData[idx++] = vertsPerFace - 1 - i;
	}
	for (int i = vertsPerFace; i < 2*vertsPerFace; ++i) {
		iboData[idx++] = i;
	}
	iboData[idx++] = 2 * vertsPerFace;
	iboData[idx++] = 3 * vertsPerFace;
	for (int i = 1; i < vertsPerFace; ++i) {
		iboData[idx++] = 3 * vertsPerFace - i;
		iboData[idx++] = m_nVertices - i;
	}

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vboSize, (void*)vboData, GL_STATIC_DRAW);
	delete[] vboData;

	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, iboSize, (void*)iboData, GL_STATIC_DRAW);
	delete[] iboData;


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, floatsPerVertex * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, floatsPerVertex * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, floatsPerVertex * sizeof(float), (void*)(6 * sizeof(float)));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, floatsPerVertex * sizeof(float), (void*)(8 * sizeof(float)));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, floatsPerVertex * sizeof(float), (void*)(11 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Tombstone::~Tombstone() {
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
}

TombstoneInstance::TombstoneInstance() : 
	m_tomb(nullptr), 
	m_M(1.0f),	
	m_KdUniformLoc(-1),
	m_KsUniformLoc(-1),
	m_NsUniformLoc(-1) {}

TombstoneInstance::TombstoneInstance(Tombstone* tombstone) : 
	m_KdUniformLoc(-1),
	m_KsUniformLoc(-1),
	m_NsUniformLoc(-1), 
	m_tomb(tombstone), 
	m_M(1.0f) {}

void TombstoneInstance::setUniformLocations(GLint KdUniformLoc, GLint KsUniformLoc, GLint NsUniformLoc) {
	m_KdUniformLoc = KdUniformLoc;
	m_KsUniformLoc = KsUniformLoc;
	m_NsUniformLoc = NsUniformLoc;
}

void TombstoneInstance::draw() const {
	assert(m_tomb);
	glUniform3f(m_KsUniformLoc, 0, 0, 0);
	glUniform1f(m_NsUniformLoc, 0);
	glUniform3f(m_KdUniformLoc, 0.2f, 0.2f, 0.2f);
	glBindVertexArray(m_tomb->getVao());
	const int vertsPerFace = m_tomb->getNumVertices() / 4;
	glDrawElements(GL_TRIANGLE_FAN, vertsPerFace, GL_UNSIGNED_SHORT, (void*)0);
	glDrawElements(GL_TRIANGLE_FAN, vertsPerFace, GL_UNSIGNED_SHORT, (void*)(vertsPerFace * sizeof(ushort)));
	glDrawElements(GL_TRIANGLE_STRIP, 2 * vertsPerFace, GL_UNSIGNED_SHORT, (void*)(2 * vertsPerFace * sizeof(ushort)));
	glBindVertexArray(0);
}