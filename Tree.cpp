#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>

#include "Tree.h"
#include "Scene.h"

TreeInstance::TreeInstance() : m_scene(nullptr), m_KdUniformLoc(-1), m_KsUniformLoc(-1), m_NsUniformLoc(-1) {}

TreeInstance::TreeInstance(Scene *scene, Cylinder* cyl, const glm::vec3& pos) : m_scene(scene), m_KdUniformLoc(-1), m_KsUniformLoc(-1), m_NsUniformLoc(-1) {
	CylinderInstance trunk(cyl);
	trunk.transform(glm::rotate(glm::radians(-90.0f), glm::vec3(1, 0, 0)));
	trunk.transform(glm::scale(glm::vec3(0.3f, 4.0f, 0.3f)));
	trunk.transform(glm::translate(pos));
	m_cyls.push_back(trunk);

	CylinderInstance branch1(cyl);	
	branch1.transform(glm::scale(glm::vec3(0.1f, 0.1f, 2.0f)));
	branch1.transform(glm::translate(pos + glm::vec3(0, 2, 0)));
	m_cyls.push_back(branch1);

	CylinderInstance branch2(cyl);
	branch2.transform(glm::scale(glm::vec3(0.1f, 0.1f, 2.0f)));
	branch2.transform(glm::rotate(glm::radians(120.0f), glm::vec3(0, 1, 0)));
	branch2.transform(glm::translate(pos + glm::vec3(0, 2.5f, 0)));
	m_cyls.push_back(branch2);

	CylinderInstance branch3(cyl);
	branch3.transform(glm::scale(glm::vec3(0.1f, 0.1f, 2.0f)));
	branch3.transform(glm::rotate(glm::radians(240.0f), glm::vec3(0, 1, 0)));
	branch3.transform(glm::translate(pos + glm::vec3(0, 3.0f, 0)));
	m_cyls.push_back(branch3);
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