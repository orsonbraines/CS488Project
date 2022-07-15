#include "Skybox.h"

Skybox::Skybox() : m_prog("shaders/skybox.vs", "shaders/skybox.fs") {
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	float vboData[8 * 3] = {
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
	};
	ushort iboData[36] = {
		2,0,3,
		3,0,1,
		3,1,7,
		7,1,5,
		7,5,6,
		6,5,4,
		6,4,2,
		2,4,0,
		2,3,6,
		6,3,7,
		0,4,1,
		1,4,5
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vboData), (void*)vboData, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(iboData), (void*)iboData, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Skybox::~Skybox() {
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
}

SkyboxInstance::SkyboxInstance(Skybox* skybox, const std::string& filePrefix) : m_skybox(skybox) {
	m_tex.loadDDS(filePrefix + "PX.dds", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	m_tex.loadDDS(filePrefix + "NX.dds", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	m_tex.loadDDS(filePrefix + "PY.dds", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	m_tex.loadDDS(filePrefix + "NY.dds", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	m_tex.loadDDS(filePrefix + "PZ.dds", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	m_tex.loadDDS(filePrefix + "NZ.dds", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
}

void SkyboxInstance::draw(const glm::mat4& PV, float brightness) {
	m_skybox->m_prog.use();
	glUniformMatrix4fv(m_skybox->m_prog["PV"], 1, GL_FALSE, glm::value_ptr(PV));
	glUniform1f(m_skybox->m_prog["brightness"], brightness);
	glActiveTexture(GL_TEXTURE0);
	m_tex.bind();
	glUniform1i(m_skybox->m_prog["skybox"], 0);
	glBindVertexArray(m_skybox->m_vao);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
	glBindVertexArray(0);
}
