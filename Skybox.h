#pragma once

#include <GL/glew.h>
#include <glm/ext.hpp>

#include "Texture.h"
#include "ShaderProgram.h"

class Skybox {
public:
	Skybox();
	~Skybox();
	void draw(const glm::mat4 & PV);
private:
	CubemapTexture m_tex;
	ShaderProgram m_prog;
	GLuint m_vao, m_vbo, m_ibo;
};