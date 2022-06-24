#pragma once

#include <string>
#include <GL/glew.h>

class Texture {
public:
	Texture();
	~Texture();
	void bind();
	void setMinFilter(GLint minFilter);
	void setMagFilter(GLint magFilter);
	void setWrapS(GLint wrapS);
	void setWrapT(GLint wrapT);
	void loadBMP(const std::string& texFilePath);
	void loadDDS(const std::string& texFilePath);
private:
	GLuint m_texId;
	GLint m_minFilter;
	GLint m_magFilter;
	GLint m_wrapS;
	GLint m_wrapT;
};