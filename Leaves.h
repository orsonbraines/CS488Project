#pragma once

#include <GL/glew.h>

#include "Types.h"
#include "Texture.h"
#include "ShaderProgram.h"

class Leaves {
public:
	Leaves(float lu, float lv);
	~Leaves();
	GLuint getVao() const { return m_vao; }
	uint getId() const { return m_id; }
	void draw(const ShaderProgram &p);
	void setUniformLocations(GLint KsUniformLoc, GLint NsUniformLoc);
private:
	GLint m_KsUniformLoc, m_NsUniformLoc;
	GLuint m_vao, m_vbo;
	uint m_id;
	Texture m_tex;
};
