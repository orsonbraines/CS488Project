#include "ShaderProgram.h"
#include "Util.h"

ShaderProgram::ShaderProgram(const std::string& vsFilePath, const std::string& fsFilePath) {
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vsSource = readFile(vsFilePath);
    const char* vsSourceCstr = vsSource.c_str();
    glShaderSource(vs, 1, &vsSourceCstr, nullptr);
    glCompileShader(vs);
    int res;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &res);
    if (res == GL_FALSE) {
        char buf[2048];
        glGetShaderInfoLog(vs, 2048, nullptr, buf);
        glDeleteShader(vs);
        throw GraphicsException(std::string(buf));
    }
    std::string fsSource = readFile(fsFilePath);
    const char* fsSourceCstr = fsSource.c_str();
    glShaderSource(fs, 1, &fsSourceCstr, nullptr);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &res);
    if (res == GL_FALSE) {
        char buf[2048];
        glGetShaderInfoLog(fs, 2048, nullptr, buf);
        glDeleteShader(vs);
        glDeleteShader(fs);
        throw GraphicsException(std::string(buf));
    }

    m_programId = glCreateProgram();
    glAttachShader(m_programId, vs);
    glAttachShader(m_programId, fs);
    glLinkProgram(m_programId);
    glGetProgramiv(m_programId, GL_LINK_STATUS, &res);

    if (res == GL_FALSE) {
        glDeleteShader(vs);
        glDeleteShader(fs);
        throw GraphicsException("Something went wrong linking the shader program");
    }

    glDetachShader(m_programId, vs);
    glDetachShader(m_programId, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);
}

ShaderProgram::~ShaderProgram() {
	glDeleteProgram(m_programId);
}

void ShaderProgram::use() {
	glUseProgram(m_programId);
}

GLuint ShaderProgram::getId() const {
	return m_programId;
}
