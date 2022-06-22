#include <cassert>

#include "GridMesh.h"

GridMesh::GridMesh(uint xDivs, uint yDivs) : m_xDivs(xDivs), m_yDivs(yDivs), m_numIndices((xDivs - 1) * (yDivs - 1) * 6), m_vbo(0), m_ibo(0), m_vao(0) {
	assert(xDivs >= 2 && yDivs >= 2);
	// using ushort for ibo
	assert(xDivs * yDivs * 2 <= (1u << 16));
	
	size_t vboSize = xDivs * yDivs * 2 * sizeof(float);
	size_t iboSize = m_numIndices * sizeof(ushort);
	
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	float* vboData = new float[xDivs * yDivs * 2];
	size_t i = 0;
	for (uint y = 0; y < yDivs; ++y) {
		for (uint x = 0; x < xDivs; ++x) {
			vboData[i++] = x / float(xDivs - 1);
			vboData[i++] = y / float(yDivs - 1);
		}
	}
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vboSize, (void*)vboData, GL_STATIC_DRAW);
	delete[] vboData;

	ushort* iboData = new ushort[m_numIndices];
	i = 0;
	for (uint x = 0; x + 1 < xDivs; ++x) {
		for (uint y = 0; y + 1 < yDivs; ++y) {
			// 1st triangle
			iboData[i++] = (y + 1) * xDivs + x;
			iboData[i++] = y * xDivs + x;
			iboData[i++] = (y + 1) * xDivs + (x + 1);
			// 2nd triangle
			iboData[i++] = y * xDivs + x;
			iboData[i++] = y * xDivs + (x + 1);
			iboData[i++] = (y + 1) * xDivs + (x + 1);
		}
	}
	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, iboSize, (void*)iboData, GL_STATIC_DRAW);
	delete[] iboData;

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0); // unbind vao first so it remembers buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GridMesh::draw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_SHORT, (void*)0);
	glBindVertexArray(0);
}

GridMesh::~GridMesh() {
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
}