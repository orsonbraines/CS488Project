#pragma once
#include <GL/glew.h>
#include <string>
#include <map>


class ShaderProgram {
public:
	ShaderProgram(const std::string& vsFilePath, const std::string& fsFilePath);
	~ShaderProgram();
	void use();
	GLuint getId() const;
	GLint operator[](const std::string &s) const;
private:
	GLuint m_programId;
	std::map<std::string, GLint> m_uniforms;
};