#include <vector>
#include <glm/ext.hpp>
#include <sstream>
#include <iomanip>

#include "UI.h"
#include "Util.h"

UI::UI(Scene* scene, AudioDevice &audioDevice) :
	m_prog("shaders/2dColour.vs", "shaders/2dColour.fs"),
	m_textProg("shaders/2dTexture.vs", "shaders/2dAlphaTexture.fs"),
	m_texFont(),
	m_scene(scene),
	m_clockSubdivisions(60), 
	m_aspectRatio(1.0f), 
	m_fbWidth(1), 
	m_fbHeight(1),
	m_textDataVboSize(2048),
	m_fps(1.0f),
	m_fpsTimer(1.0f),
	m_showCrosshairs(false),
	m_requiredIds{2,3},
	m_foundObjects{false, false},
	m_showHint(false),
	m_hintIdx(0),
	m_hints{"M2 U M2 U2 M2 U M2", "My precious"},
	m_audioDevice(audioDevice),
	m_failSound("sounds/failSound.wav"),
	m_passSound("sounds/passSound.wav"),
	m_timeSinceLastSound(0)
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

	m_crosshairsOffset = vboData.size() / 2;
	//crosshairs
	vboData.push_back(-0.1f);
	vboData.push_back(0.0f);
	vboData.push_back(0.1f);
	vboData.push_back(0.0f);
	vboData.push_back(0.0f);
	vboData.push_back(-0.1f);
	vboData.push_back(0.0f);
	vboData.push_back(0.1f);

	m_checkboxOffset = vboData.size() / 2;
	vboData.push_back(-1.0f);
	vboData.push_back(-1.0f);
	vboData.push_back(1.0f);
	vboData.push_back(-1.0f);
	vboData.push_back(-1.0f);
	vboData.push_back(1.0f);
	vboData.push_back(1.0f);
	vboData.push_back(1.0f);

	vboData.push_back(-0.9f);
	vboData.push_back(-0.9f);
	vboData.push_back(0.9f);
	vboData.push_back(0.9f);
	vboData.push_back(-0.9f);
	vboData.push_back(0.9f);
	vboData.push_back(0.9f);
	vboData.push_back(-0.9f);


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

void UI::changeHint(bool dir) {
	if (dir && m_hintIdx + 1 < m_hints.size()) {
		++m_hintIdx;
	}
	else if(!dir && m_hintIdx > 0) {
		--m_hintIdx;
	}
}

void UI::pickTarget() {
	bool success = false;
	uint id = m_scene->pickTarget();
	for (uint i = 0; i < m_requiredIds.size(); ++i) {
		if (id == m_requiredIds[i]) {
			if (m_foundObjects[i] == false) {
				m_foundObjects[i] = true;
				success = true;
			}
			break;
		}
	}

	if (m_timeSinceLastSound > 1.0f) {
		if (success) {
			m_audioDevice.playSound(m_passSound);
		}
		else {
			m_audioDevice.playSound(m_failSound);
		}
		m_timeSinceLastSound = 0.0f;
	}
}

void UI::setFbSize(int fbWidth, int fbHeight) { 
	m_aspectRatio = float(fbWidth) / float(fbHeight);
	m_fbWidth = fbWidth;
	m_fbHeight = fbHeight;
}

void UI::tick(float dt) {
	m_fpsTimer += dt;
	m_timeSinceLastSound += dt;
	for (auto it = m_messages.begin(); it != m_messages.end();) {
		TimedMessage& m = it->second;
		m.timeRemaining -= dt;
		if (m.timeRemaining < 0.0f) {
			auto nextIt = it;
			++nextIt;
			m_messages.erase(it);
			it = nextIt;
		}
		else {
			++it;
		}
	}
}

void UI::addMessage(float duration, const std::string& key, const std::string& s) {
	TimedMessage m{s, duration};
	m_messages[key] = m;
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

	if (m_showCrosshairs) {
		M = glm::mat3(glm::vec3(1.0f / m_aspectRatio, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
		glUniformMatrix3fv(m_prog["M"], 1, GL_FALSE, glm::value_ptr(M));
		glDrawArrays(GL_LINES, m_crosshairsOffset, 4);
	}

	for (uint i = 0; i < m_requiredIds.size(); ++i) {
		glUniform4f(m_prog["colour"], 0.0f, 0.0f, 0.0f, 1.0f);
		M = glm::mat3(glm::vec3(0.06f / m_aspectRatio, 0, 0), glm::vec3(0, 0.06f, 0), glm::vec3(-0.6f + 0.08f * i, -0.9f, 1));
		glUniformMatrix3fv(m_prog["M"], 1, GL_FALSE, glm::value_ptr(M));
		glDrawArrays(GL_TRIANGLE_STRIP, m_checkboxOffset, 4);
		glUniform4f(m_prog["colour"], 1.0f, 1.0f, 1.0f, 1.0f);
		M = glm::mat3(glm::vec3(0.05f / m_aspectRatio, 0, 0), glm::vec3(0, 0.05f, 0), glm::vec3(-0.6f + 0.08f * i, -0.9f, 1));
		glUniformMatrix3fv(m_prog["M"], 1, GL_FALSE, glm::value_ptr(M));
		glDrawArrays(GL_TRIANGLE_STRIP, m_checkboxOffset, 4);
		if (m_foundObjects[i]) {
			glUniform4f(m_prog["colour"], 0.0f, 0.6f, 0.0f, 1.0f);
			glLineWidth(6.0f);
			glDrawArrays(GL_LINES, m_checkboxOffset + 4, 4);
		}
	}

	if (m_showHint) {
		glUniform4f(m_prog["colour"], 0.0f, 0.0f, 0.0f, 1.0f);
		M = glm::mat3(glm::vec3(0.5f, 0, 0), glm::vec3(0, 0.5f, 0), glm::vec3(0, 0, 1));
		glUniformMatrix3fv(m_prog["M"], 1, GL_FALSE, glm::value_ptr(M));
		glDrawArrays(GL_TRIANGLE_STRIP, m_checkboxOffset, 4);
		glUniform4f(m_prog["colour"], 1.0f, 1.0f, 1.0f, 1.0f);
		M = glm::mat3(glm::vec3(0.5f - 4 * pixelSizeX, 0, 0), glm::vec3(0, 0.5f - 4 * pixelSizeY, 0), glm::vec3(0, 0, 1));
		glUniformMatrix3fv(m_prog["M"], 1, GL_FALSE, glm::value_ptr(M));
		glDrawArrays(GL_TRIANGLE_STRIP, m_checkboxOffset, 4);
		addText(m_hints[m_hintIdx], -0.48f, 0.5f - 22 * pixelSizeY, 1.0f);
	}

	// Update the text data
	// only update the fps counter every second so it doesn't flicker
	if (m_fpsTimer > 0.25f) {
		std::ostringstream msgstream;
		msgstream << std::fixed << std::setprecision(1) << "FPS: " << m_fps;
		m_fpsString = msgstream.str();
		m_fpsTimer = 0;
	}
	addText(m_fpsString, -1.0f, 1.0f - 32.0f / m_fbHeight, 1.0f);
	float messageBot = 0.8f;
	for (auto& p : m_messages) {
		TimedMessage& m = p.second;
		addText(m.txt, -0.5f, messageBot, 1.0f);
		messageBot -= 16 * (2.0 / m_fbHeight);
	}

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
	float charWidthPix = 11.0f;
	float charWidthU = charWidthPix / 512.0f;
	float charTopV = 4.0f / 256.0f;
	float charBotV = 20.0f / 256.0f;
	float charHeightPix = 16.0f;

	for (char c : txt) {
		// 1st triangle
		m_textData.push_back(left);
		m_textData.push_back(bot);
		m_textData.push_back((c & 0xf) / 16.0f - 0.5f / 512.0f);
		m_textData.push_back(1.0f - (c >> 4) / 8.0f - charBotV);

		m_textData.push_back(left + charWidthPix * pixelSizeX * scale);
		m_textData.push_back(bot);
		m_textData.push_back((c & 0xf) / 16.0f + charWidthU - 0.5f / 512.0f);
		m_textData.push_back(1.0f - (c >> 4) / 8.0f - charBotV);

		m_textData.push_back(left);
		m_textData.push_back(bot + charHeightPix * pixelSizeY * scale);
		m_textData.push_back((c & 0xf) / 16.0f - 0.5f / 512.0f);
		m_textData.push_back(1.0f - (c >> 4) / 8.0f - charTopV);

		// second triangle
		m_textData.push_back(left);
		m_textData.push_back(bot + charHeightPix * pixelSizeY * scale);
		m_textData.push_back((c & 0xf) / 16.0f - 0.5f / 512.0f);
		m_textData.push_back(1.0f - (c >> 4) / 8.0f - charTopV);

		m_textData.push_back(left + charWidthPix * pixelSizeX * scale);
		m_textData.push_back(bot);
		m_textData.push_back((c & 0xf) / 16.0f + charWidthU - 0.5f / 512.0f);
		m_textData.push_back(1.0f - (c >> 4) / 8.0f - charBotV);

		m_textData.push_back(left + charWidthPix * pixelSizeX * scale);
		m_textData.push_back(bot + charHeightPix * pixelSizeY * scale);
		m_textData.push_back((c & 0xf) / 16.0f + charWidthU - 0.5f / 512.0f);
		m_textData.push_back(1.0f - (c >> 4) / 8.0f - charTopV);

		left += charWidthPix * pixelSizeX * scale;
	}

	if (m_textData.size() > m_textDataVboSize) {
		throw GraphicsException("Text overflowwing vbo");
	}
}