#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>

#include "Tree.h"
#include "Scene.h"

TreeInstance::TreeInstance() : m_scene(nullptr), m_leaves(nullptr), m_KdUniformLoc(-1), m_KsUniformLoc(-1), m_NsUniformLoc(-1) {}

TreeInstance::TreeInstance(Scene *scene, Cylinder* cyl, Leaves *leaves, const glm::vec3& pos) : m_scene(scene), m_leaves(leaves), m_KdUniformLoc(-1), m_KsUniformLoc(-1), m_NsUniformLoc(-1) {
	CylinderInstance trunk(cyl);
	trunk.transform(glm::rotate(glm::radians(-90.0f), glm::vec3(1, 0, 0)));
	trunk.transform(glm::scale(glm::vec3(0.3f, 4.0f, 0.3f)));
	trunk.transform(glm::translate(pos));
	m_cyls.push_back(trunk);

	addBranch(cyl, pos, 2.0f, 0.0f);
	addBranch(cyl, pos, 2.5f, glm::radians(120.f));
	addBranch(cyl, pos, 3.0f, glm::radians(240.f));
}

void TreeInstance::setUniformLocations(GLint KdUniformLoc, GLint KsUniformLoc, GLint NsUniformLoc) {
	m_KdUniformLoc = KdUniformLoc;
	m_KsUniformLoc = KsUniformLoc;
	m_NsUniformLoc = NsUniformLoc;
}

void TreeInstance::draw(const ShaderProgram& p, const glm::mat4& P, const glm::mat4& V, bool shadow) const {
	glUniform3f(m_KsUniformLoc, 0, 0, 0);
	glUniform1f(m_NsUniformLoc, 0);
	glUniform3f(m_KdUniformLoc, 0.238235f, 0.16078f, 0.0745098f);
	for (const CylinderInstance& c : m_cyls) {
		if (shadow) {
			m_scene->setPVM(p, P, V, c.getM());
		}
		else {
			m_scene->setMatrixUniforms(p, P, V, c.getM());
		}
		c.draw();
	}
}

void TreeInstance::drawLeaves(const ShaderProgram& p, const glm::mat4& P, const glm::mat4& V) const {
	for (const glm::mat4& M : m_leafMats) {
		m_scene->setMatrixUniforms(p, P, V, M);
		m_leaves->draw(p);
	}
}

void TreeInstance::addBranch(Cylinder* cyl, const glm::vec3& pos, float h, float theta) {
	CylinderInstance branch(cyl);
	branch.transform(glm::scale(glm::vec3(0.1f, 0.1f, 2.0f)));
	branch.transform(glm::rotate(theta, glm::vec3(0, 1, 0)));
	branch.transform(glm::translate(pos + glm::vec3(0, h, 0)));
	m_cyls.push_back(branch);
	for (int i = 0; i < 2; ++i) {
		m_leafMats.push_back(glm::translate(pos + glm::vec3(0, h + 0.05f, 0))
			* glm::rotate(theta - glm::pi<float>() / 2.0f, glm::vec3(0, 1, 0))
			* glm::rotate(i * glm::pi<float>(), glm::vec3(1, 0, 0))
			* glm::scale(glm::vec3(2.0f, 0.3f, 1.0f)));
	}	
}

bool TreeInstance::collides(const Sphere& s) const {
	for (const CylinderInstance& c : m_cyls) {
		if (intersects(c.getAABB(), s)) {
			return true;
		}
	}
	return false;
}