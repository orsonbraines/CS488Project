#pragma once

#include <GL/glew.h>

#include "ShaderProgram.h"
#include "Scene.h"

class UI {
public:
	UI(Scene *scene);
	~UI();
	void draw();
	void setFbSize(int fbWidth, int fbHeight);
private:
	ShaderProgram m_prog;
	Scene* m_scene;
	GLuint m_vao, m_vbo;
	float m_aspectRatio;
	int m_fbWidth, m_fbHeight;

	const float m_clockSize = 0.15f;
	const int m_clockSubdivisions;
	int m_clockTickOffset;
};