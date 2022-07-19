#pragma once

#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>

#include "Cylinder.h"
#include "Leaves.h"
#include "ShaderProgram.h"

class Scene;

class TreeInstance {
public:
	TreeInstance();
	TreeInstance(Scene *scene, Cylinder *cyl, Leaves *leaves, const glm::vec3 &pos);
	void draw(const ShaderProgram& p, const glm::mat4& P, const glm::mat4& V, bool shadow) const;
	void drawLeaves(const ShaderProgram& p, const glm::mat4& P, const glm::mat4& V) const;
	void setUniformLocations(GLint KdUniformLoc, GLint KsUniformLoc, GLint NsUniformLoc);
	const glm::mat4& getM() { return m_cyls.front().getM(); }
	bool collides(const Sphere& s) const;
private:
	void addBranch(Cylinder *cyl, const glm::vec3& pos, float h, float theta);
	std::vector<CylinderInstance> m_cyls;
	std::vector<glm::mat4> m_leafMats;
	GLint m_KdUniformLoc, m_KsUniformLoc, m_NsUniformLoc;
	Scene* m_scene;
	Leaves* m_leaves;
};