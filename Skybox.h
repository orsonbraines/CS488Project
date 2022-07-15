#pragma once

#include <GL/glew.h>
#include <glm/ext.hpp>
#include <string>

#include "Texture.h"
#include "ShaderProgram.h"

class SkyboxInstance;

class Skybox {
public:
	Skybox();
	~Skybox();
private:
	ShaderProgram m_prog;
	GLuint m_vao, m_vbo, m_ibo;
	friend class SkyboxInstance;
};

class SkyboxInstance {
public:
	SkyboxInstance(Skybox *skybox, const std::string &filePrefix);
	void draw(const glm::mat4& PV, float brightness);
private:
	Skybox* m_skybox;
	CubemapTexture m_tex;
};