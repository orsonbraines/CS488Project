#pragma once

#include <GL/glew.h>
#include <fstream>
#include <string>
#include <exception>

#include "Types.h"

#ifdef DEBUG
#define DBG(c) c
#else
#define DBG(c) while(0){}
#endif

std::string readFile(const std::string &fileName);

void loggingCallback(GLenum source​, GLenum type​, GLuint id​, GLenum severity​, GLsizei length​, const GLchar* message​, const void* userParam​);

class GraphicsException : public std::exception {
public:
	GraphicsException(const std::string& msg);
	virtual const char* what() const noexcept override;
private:
	std::string m_msg;
};