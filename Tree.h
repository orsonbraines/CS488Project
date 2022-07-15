#pragma once

#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>

#include "Cylinder.h"
#include "ShaderProgram.h"

class Scene;

class TreeInstance {
public:
	TreeInstance(Scene *scene, Cylinder *cyl, const glm::vec3 &pos);
	void draw(const ShaderProgram& p, const glm::mat4& P, const glm::mat4& V, bool shadow) const;
	void setUniformLocations(GLint KdUniformLoc, GLint KsUniformLoc, GLint NsUniformLoc);
	const glm::mat4& getM() { return m_cyls.front().getM(); }
private:
	std::vector<CylinderInstance> m_cyls;
	GLint m_KdUniformLoc, m_KsUniformLoc, m_NsUniformLoc;
	Scene* m_scene;
};