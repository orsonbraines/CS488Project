#pragma once

#include <string>
#include <GL/glew.h>

class Texture {
public:
	Texture(const std::string &texFilePath);
	~Texture();
	void bind();
private:
	GLuint m_texId;
};