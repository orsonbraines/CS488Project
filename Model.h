#pragma once

#include <GL/glew.h>

#include "ShaderProgram.h"
#include "Types.h"
#include "Material.h"

class Model {
public:
	Model(const std::string &fileName);
	~Model();
	void draw();
	GLint m_KdUniformLoc, m_KsUniformLoc, m_NsUniformLoc;
private:
	GLuint m_vbo, m_ibo, m_vao;
	uint m_nIndices;
	std::vector<std::pair<uint, Material>> m_mtls;
	bool m_isTextured;
};