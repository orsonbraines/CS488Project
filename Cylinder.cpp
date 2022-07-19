#include <glm/ext.hpp>

#include "Cylinder.h"


Cylinder::Cylinder() : m_id(255) {
	m_nDivisions = 50;
	m_nVertices = (m_nDivisions + 1) * 2 + 1;
	const int floatsPerVertex = 14;
	const float lu = 2;
	const float lv = 6;

	const int vboSize = floatsPerVertex * m_nVertices * sizeof(float);
	float* vboData = new float[floatsPerVertex * m_nVertices];
	int idx = 0;

	for (int i = 0; i <= m_nDivisions; ++i) {
		float theta = float(i) / float(m_nDivisions) * 2 * glm::pi<float>();
		/* top vertex */
		// pos
		vboData[idx++] = glm::cos(theta);
		vboData[idx++] = glm::sin(theta);
		vboData[idx++] = 1.0f;
		// normal
		vboData[idx++] = glm::cos(theta);
		vboData[idx++] = glm::sin(theta);
		vboData[idx++] = 0.0f;
		// uv
		vboData[idx++] = float(i) / float(m_nDivisions) * lu;
		vboData[idx++] = 1.0f * lv;
		// udir
		vboData[idx++] = -glm::sin(theta);
		vboData[idx++] = glm::cos(theta);
		vboData[idx++] = 0.0f;
		// vdir
		vboData[idx++] = 0.0f;
		vboData[idx++] = 0.0f;
		vboData[idx++] = 1.0f;

		/*bottom vertex*/
		// pos
		vboData[idx++] = glm::cos(theta);
		vboData[idx++] = glm::sin(theta);
		vboData[idx++] = 0.0f;
		// normal
		vboData[idx++] = glm::cos(theta);
		vboData[idx++] = glm::sin(theta);
		vboData[idx++] = 0.0f;
		// uv
		vboData[idx++] = float(i) / float(m_nDivisions) * lu;
		vboData[idx++] = 0.0f;
		// udir
		vboData[idx++] = -glm::sin(theta);
		vboData[idx++] = glm::cos(theta);
		vboData[idx++] = 0.0f;
		// vdir
		vboData[idx++] = 0.0f;
		vboData[idx++] = 0.0f;
		vboData[idx++] = 1.0f;
	}

	/* top-center vertex */
	// pos
	vboData[idx++] = 0.0f;
	vboData[idx++] = 0.0f;
	vboData[idx++] = 1.0f;
	// normal
	vboData[idx++] = 0.0f;
	vboData[idx++] = 0.0f;
	vboData[idx++] = 0.0f;
	// uv
	vboData[idx++] = 0.0f;
	vboData[idx++] = 0.0f;
	// udir
	vboData[idx++] = 1.0f;
	vboData[idx++] = 0.0f;
	vboData[idx++] = 0.0f;
	// vdir
	vboData[idx++] = 0.0f;
	vboData[idx++] = 1.0f;
	vboData[idx++] = 0.0f;

	ushort* iboData = new ushort[m_nVertices + m_nDivisions + 1];
	size_t iboSize = (m_nVertices + m_nDivisions + 1) * sizeof(ushort);
	for (int i = 0; i < m_nVertices; ++i) {
		iboData[i] = i;
	}
	for (int i = 0; i <= m_nDivisions; ++i) {
		iboData[i + m_nVertices] = i*2;
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
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, floatsPerVertex  * sizeof(float), (void*)(11 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Cylinder::~Cylinder() {
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
}


CylinderInstance::CylinderInstance(Cylinder* cylinder) :
	m_cyl(cylinder),
	m_M(1.0f) {}

CylinderInstance::CylinderInstance() :
	m_cyl(nullptr),
	m_M(1.0f) {}

void CylinderInstance::draw() const {
	assert(m_cyl);
	glBindVertexArray(m_cyl->getVao());
	glDrawElements(GL_TRIANGLE_STRIP, m_cyl->getNumVertices() - 1, GL_UNSIGNED_SHORT, (void*)0);
	glDrawElements(GL_TRIANGLE_FAN, m_cyl->getNumDivisions() + 2, GL_UNSIGNED_SHORT, (void*)((m_cyl->getNumVertices() - 1)*sizeof(ushort)));
	glBindVertexArray(0);
}
