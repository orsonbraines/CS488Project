#include <GL/glew.h>
#include <iostream>

#include "Util.h"

std::string readFile(const std::string& fileName) {
	std::ifstream in(fileName);
	if (!in) {
		throw GraphicsException("Unable to open: " + fileName);
	}

	std::string lines = "";
	std::string line;
	while (std::getline(in, line)) {
		lines += line;
		lines += "\n";
	}
	return lines;
}

void loggingCallback(GLenum source​, GLenum type​, GLuint id​, GLenum severity​, GLsizei length​, const GLchar* message​, const void* userParam​) {
	std::cerr << "OpenGL: " << message​ << std::endl;
}

GraphicsException::GraphicsException(const std::string& msg) : m_msg(msg) {}

const char* GraphicsException::what() const noexcept {
	return m_msg.c_str();
}