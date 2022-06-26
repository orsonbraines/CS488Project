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
        char buf[2048];
        glGetProgramInfoLog(m_programId, 2048, nullptr, buf);

        glDeleteShader(vs);
        glDeleteShader(fs);
        throw GraphicsException("link error: " + std::string(buf));
    }

    glDetachShader(m_programId, vs);
    glDetachShader(m_programId, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    int numUniforms, maxNameLen;

    glGetProgramiv(m_programId, GL_ACTIVE_UNIFORMS, &numUniforms);
    glGetProgramiv(m_programId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLen);

    char *name = new char[maxNameLen + 1];
    
    for (int i = 0; i < numUniforms; ++i) {
        glGetActiveUniformName(m_programId, i, maxNameLen + 1, nullptr, name);
        m_uniforms[std::string(name)] = i;
    }

    delete[] name;
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

GLint ShaderProgram::operator[](const std::string& s) const {
    auto it = m_uniforms.find(s);
    if (it == m_uniforms.end()) {
        return -1;
    }
    return it->second;
}
