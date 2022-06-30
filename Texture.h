#pragma once

#include <string>
#include <GL/glew.h>

class Texture {
public:
	Texture();
	~Texture();
	void bind() const;
	void setMinFilter(GLint minFilter);
	void setMagFilter(GLint magFilter);
	void setWrapS(GLint wrapS);
	void setWrapT(GLint wrapT);
	void setBorderColour(float r, float g, float b, float a);
	void loadBMP(const std::string& texFilePath);
	void loadDDS(const std::string& texFilePath);
	GLuint getId() const { return m_texId; }
private:
	GLuint m_texId;
	GLint m_minFilter;
	GLint m_magFilter;
	GLint m_wrapS;
	GLint m_wrapT;
};