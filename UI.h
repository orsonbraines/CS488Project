#pragma once

#include <GL/glew.h>
#include <vector>
#include <map>
#include <string>

#include "ShaderProgram.h"
#include "Scene.h"
#include "Texture.h"
#include "Sound.h"

struct TimedMessage {
	std::string txt;
	float timeRemaining;
};

class UI {
public:
	UI(Scene *scene, AudioDevice &audioDevice);
	~UI();
	void draw();
	void setFbSize(int fbWidth, int fbHeight);
	// scale is a multiple of the size in the texture, scale = 1 is 10*16 pixels
	void addText(const std::string &txt, float left, float bot, float scale);
	void tick(float dt);
	void setFps(float fps) { m_fps = fps; }
	void addMessage(float duration, const std::string &key, const std::string& s);
	void setShowCrosshiars(bool show) { m_showCrosshairs = show; }
	void toggleShowHint() { m_showHint = !m_showHint; }
	bool getShowHint() { return m_showHint; }
	void changeHint(bool dir);
	void pickTarget();
private:
	Texture m_texFont;
	ShaderProgram m_prog, m_textProg;
	Scene* m_scene;
	GLuint m_vao, m_vbo;
	GLuint m_textVbo, m_textVao;
	float m_aspectRatio;
	int m_fbWidth, m_fbHeight;
	std::vector<float> m_textData;
	const int m_textDataVboSize; // size in floats

	const float m_clockSize = 0.15f;
	const int m_clockSubdivisions;
	int m_clockTickOffset;

	float m_fps;
	float m_fpsTimer;
	std::string m_fpsString;
	std::map<std::string, TimedMessage> m_messages;

	bool m_showCrosshairs;
	int m_crosshairsOffset;
	int m_checkboxOffset;

	std::vector<uint> m_requiredIds;
	std::vector<bool> m_foundObjects;
	bool m_showHint;
	uint m_hintIdx;
	std::vector<std::string> m_hints;

	AudioDevice& m_audioDevice;
	Sound m_failSound, m_passSound;
	float m_timeSinceLastSound;
};