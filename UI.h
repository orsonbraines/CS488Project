#pragma once

#include <GL/glew.h>
#include <vector>
#include <string>

#include "ShaderProgram.h"
#include "Scene.h"
#include "Texture.h"

class UI {
public:
	UI(Scene *scene);
	~UI();
	void draw();
	void setFbSize(int fbWidth, int fbHeight);
	// scale is a multiple of the size in the texture, scale = 1 is 10*16 pixels
	void addText(const std::string &txt, float left, float bot, float scale);
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
};