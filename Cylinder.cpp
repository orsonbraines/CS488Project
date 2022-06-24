#include <glm/ext.hpp>

#include "Cylinder.h"


Cylinder::Cylinder() {
	/*layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 vs_normal;
layout(location = 2) in vec2 vs_uv;
layout(location = 3) in vec3 vs_udir;
layout(location = 4) in vec3 vs_vdir;*/

	const int nDivisions = 50;
	m_nVertices = (nDivisions + 1) * 2;
	const int floatsPerVertex = 14;
	const float lu = 4;
	const float lv = 10;

	const int vboSize = floatsPerVertex * m_nVertices * sizeof(float);

	float* vboData = new float[floatsPerVertex * m_nVertices];
	int idx = 0;

	for (int i = 0; i <= nDivisions; ++i) {
		float theta = float(i) / float(nDivisions) * 2 * glm::pi<float>();
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
		vboData[idx++] = float(i) / float(nDivisions) * lu;
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
		vboData[idx++] = float(i) / float(nDivisions) * lu;
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

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, vboSize, (void*)vboData, GL_STATIC_DRAW);

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

}

Cylinder::~Cylinder() {
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
}


CylinderInstance::CylinderInstance(Cylinder* cylinder) :
	m_cyl(cylinder),
	m_M(1.0f) {}

void CylinderInstance::draw() const {
	assert(m_cyl);
	glBindVertexArray(m_cyl->getVao());
	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_cyl->getNumVertices());
	glBindVertexArray(0);
}
