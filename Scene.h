#pragma once

#include <GL/glew.h>

#include "Camera.h"
#include "Texture.h"
#include "Camera.h"
#include "Model.h"
#include "GridMesh.h"
#include "Cylinder.h"
#include "SmokeSystem.h"
#include "Sun.h"
#include "Flashlight.h"

class Scene {
public:
	Scene();
	~Scene();
	void render();
	Flashlight& getFlashlight() { return m_flashlight;  }

	Camera m_cam;
	bool m_binoMode;
	uint m_defaultFboW, m_defaultFboH;
private:
	void renderObjects(const glm::mat4& P, const glm::mat4& V, bool isShadow, float alpha);
	void renderSmoke(const glm::mat4& P, const glm::mat4& V);
	void renderGround(const glm::mat4& P, const glm::mat4& V, float alpha);
	void renderWater(const glm::mat4& P, const glm::mat4& V);
	void setCommonUniforms(const ShaderProgram &p, const glm::mat4& P, const glm::mat4& V, const glm::mat4& M);
	void setShadowPVM(const glm::mat4& P, const glm::mat4& V, const glm::mat4& M);
	glm::mat4 getReflectionMatrix() const; // reflection about xz plane at given y

	Sun m_sun;
	Flashlight m_flashlight;

    ShaderProgram m_textureKdProg, 
		m_constantKdProg,
		m_hmapProg, 
		m_bumpmapProg, 
		m_alphatextureProg, 
		m_shadowProg,
		m_waterProg;

    Texture m_tex123456, m_texHeightmap, m_texBmapHeightfield, m_texBino, m_texSunShadowMap, m_texFlShadowMap;

	Model m_tree, m_cube;
	ModelInstance m_tree1, m_cube1;
	GridMesh m_gridMesh;
	Cylinder m_cylinder;
	CylinderInstance m_cyl1;
	SmokeSystem m_smoke;

	GLuint m_alphatVao, m_alphatVbo;

	GLuint m_sunShadowMapFbo, m_flShadowMapFbo;
	const uint m_sunShadowTextureSize, m_flShadowTextureSize;
	const float m_reflectionPlane;
};