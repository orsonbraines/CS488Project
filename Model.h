#pragma once

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "ShaderProgram.h"
#include "Types.h"
#include "Material.h"
#include "Collision.h"

class Model {
public:
	Model(const std::string &fileName);
	~Model();
	const std::vector<std::pair<uint, Material>>& getMtls() { return m_mtls; }
	bool isTextured() const { return m_isTextured; }
	uint getNumIndices() const { return m_nIndices; }
	GLuint getVao() const { return m_vao; }
	uint getId() const { return m_id; }
	const AABB& getAABB() const { return m_aabb; }
private:
	GLuint m_vbo, m_ibo, m_vao;
	uint m_nIndices;
	std::vector<std::pair<uint, Material>> m_mtls;
	bool m_isTextured;
	uint m_id;
	AABB m_aabb;

	static uint s_prevId;
};

class ModelInstance {
public:
	ModelInstance();
	ModelInstance(Model* model);
	void draw() const;
	void setUniformLocations(GLint KdUniformLoc, GLint KsUniformLoc, GLint NsUniformLoc);
	const glm::mat4& getM() const { return m_M; }
	void setM(const glm::mat4& M);
	void transform(const glm::mat4& T);
	uint getId() const { return m_model->getId(); }
	const AABB& getAABB() const { return m_aabb; }
private:
	GLint m_KdUniformLoc, m_KsUniformLoc, m_NsUniformLoc;
	Model* m_model;
	glm::mat4 m_M;
	AABB m_aabb;
};