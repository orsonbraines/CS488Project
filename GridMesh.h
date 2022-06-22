#pragma once

#include <GL/glew.h>
#include "Types.h"

class GridMesh {
public:
	GridMesh(uint xDivs, uint yDivs);
	~GridMesh();
	void draw();
private:
	GLuint m_vbo, m_ibo, m_vao;
	uint m_xDivs, m_yDivs;
	uint m_numIndices;
};