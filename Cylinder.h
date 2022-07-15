#pragma once

#include <GL/glew.h>

#include "Types.h"

class Cylinder {
public:
	Cylinder();
	~Cylinder();
	uint getNumVertices() const { return m_nVertices; }
	uint getNumDivisions() const { return m_nDivisions; }
	GLuint getVao() const { return m_vao; }
	uint getId() const { return m_id; }
private:
	GLuint m_vao, m_vbo, m_ibo;
	uint m_nVertices, m_nDivisions;
	uint m_id;
};

class CylinderInstance {
public:
	CylinderInstance();
	CylinderInstance(Cylinder* cylinder);
	void setCylinder(Cylinder* cylinder) { m_cyl = cylinder; }
	void draw() const;
	const glm::mat4& getM() const { return m_M; }
	void setM(const glm::mat4& M) { m_M = M; }
	void transform(const glm::mat4& T) { m_M = T * m_M; }
private:
	Cylinder* m_cyl;
	glm::mat4 m_M;
};