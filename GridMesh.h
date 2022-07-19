#pragma once

#include <GL/glew.h>
#include "Types.h"
#include "Collision.h"

class GridMesh {
public:
	GridMesh(uint xDivs, uint yDivs);
	~GridMesh();
	void draw();
	const AABB& getAABB() const { return m_aabb; }
private:
	GLuint m_vbo, m_ibo, m_vao;
	uint m_xDivs, m_yDivs;
	uint m_numIndices;
	AABB m_aabb;
};