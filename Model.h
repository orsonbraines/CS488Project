#pragma once

#include <GL/glew.h>

#include "ShaderProgram.h"
#include "Types.h"

class Model {
public:
	Model(const std::string &fileName);
	~Model();
	void draw();
private:
	GLuint m_vbo, m_ibo, m_vao;
	uint m_nVerts, m_nIndices;
};