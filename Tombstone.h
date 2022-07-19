#pragma once

#include <GL/glew.h>

#include "Types.h"

class Tombstone {
public:
	Tombstone();
	~Tombstone();
	uint getNumVertices() const { return m_nVertices; }
	GLuint getVao() const { return m_vao; }
	uint getId() const { return m_id; }
private:
	GLuint m_vao, m_vbo, m_ibo;
	uint m_nVertices;
	uint m_id;
};

class TombstoneInstance {
public:
	TombstoneInstance();
	TombstoneInstance(Tombstone* tombstone);
	void draw() const;
	const glm::mat4& getM() const { return m_M; }
	void setM(const glm::mat4& M) { m_M = M; }
	void transform(const glm::mat4& T) { m_M = T * m_M; }
	void setUniformLocations(GLint KdUniformLoc, GLint KsUniformLoc, GLint NsUniformLoc);
private:
	GLint m_KdUniformLoc, m_KsUniformLoc, m_NsUniformLoc;
	Tombstone* m_tomb;
	glm::mat4 m_M;
};