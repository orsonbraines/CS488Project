#pragma once

#include <string>
#include <GL/glew.h>

class Texture {
public:
	Texture(const std::string &texFilePath);
	~Texture();
	void bind();
private:
	void loadBMP(const std::string& texFilePath);
	void loadDDS(const std::string& texFilePath);
	GLuint m_texId;
};