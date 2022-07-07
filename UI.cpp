#include <vector>
#include <glm/ext.hpp>

#include "UI.h"
#include "Util.h"

UI::UI(Scene* scene) :
	m_prog("shaders/2dColour.vs", "shaders/2dColour.fs"),
	m_textProg("shaders/2dTexture.vs", "shaders/2dAlphaTexture.fs"),
	m_texFont(),
	m_scene(scene),
	m_clockSubdivisions(60), 
	m_aspectRatio(1.0f), 
	m_fbWidth(1), 
	m_fbHeight(1),
	m_textDataVboSize(2048)
{
	m_texFont.setMagFilter(GL_NEAREST);
	m_texFont.setMinFilter(GL_NEAREST);
	m_texFont.loadBMP("textures/font.bmp");

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

	glGenVertexArrays(1, &m_textVao);
	glBindVertexArray(m_textVao);
	glGenBuffers(1, &m_textVbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_textVbo);

	glBufferData(GL_ARRAY_BUFFER, m_textDataVboSize * sizeof(float), nullptr, GL_STREAM_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);


	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

UI::~UI() {
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_textVao);
	glDeleteBuffers(1, &m_textVao);
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
	float shortTheta = time * glm::pi<float>() / 6.0f - glm::pi<float>() / 2.0f;
	float longTheta = time * glm::pi<float>() * 2.0f - glm::pi<float>() / 2.0f;
	glm::mat3 rotM = M * glm::mat3(glm::vec3(glm::cos(shortTheta), -glm::sin(shortTheta), 0), glm::vec3(glm::sin(shortTheta), glm::cos(shortTheta), 0), glm::vec3(0, 0, 1.0f));
	glUniformMatrix3fv(m_prog["M"], 1, GL_FALSE, glm::value_ptr(rotM));
	glDrawArrays(GL_LINES, m_clockTickOffset + 24, 2);
	rotM = M * glm::mat3(glm::vec3(glm::cos(longTheta), -glm::sin(longTheta), 0), glm::vec3(glm::sin(longTheta), glm::cos(longTheta), 0), glm::vec3(0, 0, 1.0f));
	glUniformMatrix3fv(m_prog["M"], 1, GL_FALSE, glm::value_ptr(rotM));
	glDrawArrays(GL_LINES, m_clockTickOffset + 26, 2);

	m_textProg.use();
	glBindVertexArray(m_textVao);
	glBindBuffer(GL_ARRAY_BUFFER, m_textVbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_textData.size() * sizeof(float), (void*)m_textData.data());
	glEnable(GL_BLEND);
	glm::mat3 I(1.0f);
	glUniformMatrix3fv(m_textProg["M"], 1, GL_FALSE, glm::value_ptr(I));
	glUniform3f(m_textProg["colour"], 1.0f, 0.0f, 0.0f);
	glUniform1i(m_textProg["sampler"], 0);
	glActiveTexture(GL_TEXTURE0);
	m_texFont.bind();
	glDrawArrays(GL_TRIANGLES, 0, m_textData.size() / 4);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	m_textData.clear();
}

void UI::addText(const std::string& txt, float left, float bot, float scale) {
	float pixelSizeX = 2.0f / float(m_fbWidth);
	float pixelSizeY = 2.0f / float(m_fbHeight);
	float charWidthPix = 10.0f;
	float charWidthU = charWidthPix / 512.0f;
	float charTopV = 4.0f / 256.0f;
	float charBotV = 20.0f / 256.0f;
	float charHeightPix = 16.0f;

	for (char c : txt) {
		// 1st triangle
		m_textData.push_back(left);
		m_textData.push_back(bot);
		m_textData.push_back((c & 0xf) / 16.0f);
		m_textData.push_back(1.0f - (c >> 4) / 8.0f - charBotV);

		m_textData.push_back(left + charWidthPix * pixelSizeX * scale);
		m_textData.push_back(bot);
		m_textData.push_back((c & 0xf) / 16.0f + charWidthU);
		m_textData.push_back(1.0f - (c >> 4) / 8.0f - charBotV);

		m_textData.push_back(left);
		m_textData.push_back(bot + charHeightPix * pixelSizeY * scale);
		m_textData.push_back((c & 0xf) / 16.0f);
		m_textData.push_back(1.0f - (c >> 4) / 8.0f - charTopV);

		// second triangle
		m_textData.push_back(left);
		m_textData.push_back(bot + charHeightPix * pixelSizeY * scale);
		m_textData.push_back((c & 0xf) / 16.0f);
		m_textData.push_back(1.0f - (c >> 4) / 8.0f - charTopV);

		m_textData.push_back(left + charWidthPix * pixelSizeX * scale);
		m_textData.push_back(bot);
		m_textData.push_back((c & 0xf) / 16.0f + charWidthU);
		m_textData.push_back(1.0f - (c >> 4) / 8.0f - charBotV);

		m_textData.push_back(left + charWidthPix * pixelSizeX * scale);
		m_textData.push_back(bot + charHeightPix * pixelSizeY * scale);
		m_textData.push_back((c & 0xf) / 16.0f + charWidthU);
		m_textData.push_back(1.0f - (c >> 4) / 8.0f - charTopV);

		left += charWidthPix * pixelSizeX * scale;
	}

	if (m_textData.size() > m_textDataVboSize) {
		throw GraphicsException("Text overflowwing vbo");
	}
}