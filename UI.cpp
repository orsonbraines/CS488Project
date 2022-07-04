#include <vector>
#include <glm/ext.hpp>

#include "UI.h"

UI::UI(Scene* scene) : m_prog("shaders/2dColour.vs", "shaders/2dColour.fs"), m_scene(scene), m_clockSubdivisions(60), m_aspectRatio(1.0f), m_fbWidth(1), m_fbHeight(1) {
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	float clockSubdivisionAngle = 2 * glm::pi<float>() / m_clockSubdivisions;
	int clockOffset = 0;
	float clockTickLength = 0.2f;
	float clockShortArmLength = 0.5f;
	float clockLongArmLength = 0.75f;
	std::vector<float> vboData;
	vboData.push_back(0.0f);
	vboData.push_back(0.0f);
	// clock circle
	for (int i = 0; i <= m_clockSubdivisions; ++i) {
		vboData.push_back(glm::cos(i * clockSubdivisionAngle));
		vboData.push_back(glm::sin(i * clockSubdivisionAngle));
	}
	m_clockTickOffset = vboData.size() / 2;
	// clock ticks
	for (int i = 0; i < 12; ++i) {
		vboData.push_back(glm::cos(i * glm::pi<float>() / 6.0f));
		vboData.push_back(glm::sin(i * glm::pi<float>() / 6.0f));
		vboData.push_back((1.0f - clockTickLength) * glm::cos(i * glm::pi<float>() / 6.0f));
		vboData.push_back((1.0f - clockTickLength) * glm::sin(i * glm::pi<float>() / 6.0f));
	}
	// clock arms
	vboData.push_back(0.0f);
	vboData.push_back(0.0f);
	vboData.push_back(clockShortArmLength);
	vboData.push_back(0.0f);
	vboData.push_back(0.0f);
	vboData.push_back(0.0f);
	vboData.push_back(clockLongArmLength);
	vboData.push_back(0.0f);

	glBufferData(GL_ARRAY_BUFFER, vboData.size() * sizeof(float), (void*)vboData.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

UI::~UI() {
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
}

void UI::setFbSize(int fbWidth, int fbHeight) { 
	m_aspectRatio = float(fbWidth) / float(fbHeight);
	m_fbWidth = fbWidth;
	m_fbHeight = fbHeight;
}

void UI::draw() {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_CULL_FACE);
	m_prog.use();
	float pixelSizeX = 2.0f / float(m_fbWidth);
	float pixelSizeY = 2.0f / float(m_fbHeight);
	glm::mat3 M = glm::mat3(glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(-1.0f + 10 * pixelSizeX + m_clockSize / m_aspectRatio, -1.0f + 10 * pixelSizeY + m_clockSize, 1.0f))
		* glm::mat3(glm::vec3(m_clockSize / m_aspectRatio, 0, 0), glm::vec3(0, m_clockSize, 0), glm::vec3(0, 0, 1.0f));
	glUniformMatrix3fv(m_prog["M"], 1, GL_FALSE, glm::value_ptr(M));
	glUniform4f(m_prog["colour"], 1.0f, 1.0f, 1.0f, 1.0f);
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 62);
	glLineWidth(3.0f);
	glUniform4f(m_prog["colour"], 0.0f, 0.0f, 0.0f, 1.0f);
	glDrawArrays(GL_LINE_STRIP, 1, 61);
	glDrawArrays(GL_LINES, m_clockTickOffset, 24);
	
	float time = m_scene->getDaytime();
	float shortTheta = time * glm::pi<float>() / 6.0f;
	float longTheta = time * glm::pi<float>() * 2.0f;
	glm::mat3 rotM = M * glm::mat3(glm::vec3(glm::cos(shortTheta), -glm::sin(shortTheta), 0), glm::vec3(glm::sin(shortTheta), glm::cos(shortTheta), 0), glm::vec3(0, 0, 1.0f));
	glUniformMatrix3fv(m_prog["M"], 1, GL_FALSE, glm::value_ptr(rotM));
	glDrawArrays(GL_LINES, m_clockTickOffset + 24, 2);
	rotM = M * glm::mat3(glm::vec3(glm::cos(longTheta), -glm::sin(longTheta), 0), glm::vec3(glm::sin(longTheta), glm::cos(longTheta), 0), glm::vec3(0, 0, 1.0f));
	glUniformMatrix3fv(m_prog["M"], 1, GL_FALSE, glm::value_ptr(rotM));
	glDrawArrays(GL_LINES, m_clockTickOffset + 26, 2);
	glBindVertexArray(0);
}