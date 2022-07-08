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
#include "Skybox.h"

enum class RenderType {
	Normal,
	Shadow,
	Pick
};

class Scene {
public:
	Scene();
	~Scene();
	void render();
	Flashlight& getFlashlight() { return m_flashlight; }
	float getDaytime() { return m_sun.getTime(); }
	Camera& getCamera() { return m_cam; }
	Sun& getSun() { return m_sun; }
	void toggleBinoMode() { m_binoMode = !m_binoMode; }
	void changeFocusDistance(float delta);
	float getFocusDistance() { return m_binoFocusDist; }
	void setFramebufferSize(uint fboW, uint fboH) {
		m_defaultFboW = fboW;
		m_defaultFboH = fboH;
	}
	void tick(float dt);
	void pickTarget();
private:
	void renderObjects(const glm::mat4& P, const glm::mat4& V, RenderType renderType, float alpha);
	void renderSmoke(const glm::mat4& P, const glm::mat4& V);
	void renderGround(const glm::mat4& P, const glm::mat4& V, float alpha);
	void renderWater(const glm::mat4& P, const glm::mat4& V);
	void blur(GLuint srcFbo, const Texture &srcDepthBuffer, const Texture &srcColourBuffer, GLuint dstFbo); // srcFbo and dst Fbo must be distinct
	void setCommonUniforms(const ShaderProgram &p, const glm::mat4& P, const glm::mat4& V, const glm::mat4& M);
	void setShadowPVM(const glm::mat4& P, const glm::mat4& V, const glm::mat4& M);
	glm::mat4 getReflectionMatrix() const; // reflection about xz plane at given y

	bool m_binoMode;
	uint m_defaultFboW, m_defaultFboH;
	Camera m_cam;
	Sun m_sun;
	Flashlight m_flashlight;

    ShaderProgram m_textureKdProg, 
		m_constantKdProg,
		m_hmapProg, 
		m_bumpmapProg, 
		m_alphatextureProg,
		m_colourTextureProg,
		m_blurProg,
		m_gaussianProg,
		m_shadowProg,
		m_waterProg,
		m_pickProg;

	Texture m_texRubiksCube,
		m_texHeightmap,
		m_texBmapHeightfield,
		m_texBino,
		m_texSunShadowMap,
		m_texFlShadowMap,
		m_texReflectedScene,
		m_texReflectedDepth,
		m_texScene,
		m_texSceneDepth;
	Texture m_texBlurs[2];

	Model m_tree, m_rubiksCube, m_goldRing;
	ModelInstance m_tree1, m_rubiksCubeInst, m_goldRingInst;
	GridMesh m_gridMesh;
	Cylinder m_cylinder;
	CylinderInstance m_cyl1;
	SmokeSystem m_smoke;
	Skybox m_skybox;

	GLuint m_alphatVao, m_alphatVbo;

	GLuint m_sunShadowMapFbo, m_flShadowMapFbo, m_reflectedFbo, m_sceneFbo;
	GLuint m_blurFbos[2];
	const uint m_sunShadowTextureSize, m_flShadowTextureSize;
	uint m_sceneWidth, m_sceneHeight;
	const float m_reflectionPlane;
	float m_binoFocusDist;
};