#pragma once
#include <GL/glew.h>
#include <string>


class ShaderProgram {
public:
	ShaderProgram(const std::string& vsFilePath, const std::string& fsFilePath);
	~ShaderProgram();
	void use();
	GLuint getId() const;
private:
	GLuint m_programId;
};