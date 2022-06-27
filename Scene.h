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

class Scene {
public:
	Scene();
	~Scene();
	void render();

	Camera m_cam;
	bool m_binoMode;
	uint m_defaultFboW, m_defaultFboH;
private:
	void renderObjects(const glm::mat4& P, const glm::mat4& V, bool isShadow);

	Sun m_sun;

    ShaderProgram m_textureKdProg, m_constantKdProg, m_hmapProg, m_bumpmapProg, m_alphatextureProg, m_shadowProg;

    Texture m_tex123456, m_texHeightmap, m_texBmapHeightfield, m_texBino, m_texShadowMap;

	Model m_tree, m_cube;
	ModelInstance m_tree1, m_cube1;
	GridMesh m_gridMesh;
	Cylinder m_cylinder;
	CylinderInstance m_cyl1;
	SmokeSystem m_smoke;

	GLuint m_alphatVao, m_alphatVbo;

	GLuint m_shadowMapFbo;
	const uint m_shadowTextureSize;
};