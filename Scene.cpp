#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

#include "Scene.h"
#include "Util.h"
#include "Collision.h"

Scene::Scene(AudioDevice &audioDevice) :
    m_audioDevice(audioDevice),
    m_collideSound("sounds/collide.wav"),
    m_timeSinceLastSound(10.0f),
    m_reflectionEnabled(true),
    m_shadowEnabled(true),
    m_bumpmapEnabled(true),
    m_flashlight(m_cam),
    m_textureKdProg("shaders/textureKd.vs", "shaders/textureKd.fs"),
    m_constantKdProg("shaders/constantKd.vs", "shaders/constantKd.fs"),
    m_hmapProg("shaders/hmap.vs", "shaders/constantKd.fs"),
    m_waterProg("shaders/water.vs", "shaders/constantKd.fs"),
    m_bumpmapProg("shaders/bumpmap.vs", "shaders/bumpmap.fs"),
    m_alphatextureProg("shaders/2dTexture.vs", "shaders/2dAlphaTexture.fs"),
    m_colourTextureProg("shaders/2dTexture.vs", "shaders/2dColourTexture.fs"),
    m_blurProg("shaders/2dTexture.vs", "shaders/dofBlur.fs"),
    m_gaussianProg("shaders/2dTexture.vs", "shaders/gaussianBlur.fs"),
    m_shadowProg("shaders/shadow.vs", "shaders/shadow.fs"),
    m_pickProg("shaders/shadow.vs", "shaders/pick.fs"),
	m_rock("models/rock.obj"),
	m_rubiksCube("models/rubikscube.obj"),
    m_goldRing("models/goldring.obj"),
    m_house("models/house.obj"),
	m_cylinder(),
    m_leaves(2.0f, 1.0f),
    m_tombstone(),
    m_tombstoneInst(&m_tombstone),
	m_gridMesh(128,128),
	m_smoke(15000, glm::vec3(15.0f, 9.0f, 17.0f)),
    m_binoMode(false),
    m_defaultFboW(0),
    m_defaultFboH(0),
    m_sunShadowTextureSize(2048),
    m_flShadowTextureSize(1024),
    m_sceneWidth(960),
    m_sceneHeight(540),
    m_reflectionPlane(1.0f),
    m_binoFocusDist(3.0f),
    m_skybox(),
    m_daySkybox(&m_skybox, "textures/skybox"),
    m_nightSkybox(&m_skybox, "textures/night")
{
	// load textures
	m_texRubiksCube.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    m_texRubiksCube.loadDDS("textures/cube.dds");
    m_texHeightmap.loadBMP("textures/noise.bmp");
	m_texBmapHeightfield.setWrapS(GL_MIRRORED_REPEAT);
	m_texBmapHeightfield.setWrapT(GL_MIRRORED_REPEAT);
    m_texBmapHeightfield.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	m_texBmapHeightfield.loadBMP("textures/treetexture.bmp", true);
    m_texBmapRip.setWrapS(GL_MIRRORED_REPEAT);
    m_texBmapRip.setWrapT(GL_MIRRORED_REPEAT);
    m_texBmapRip.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    m_texBmapRip.loadBMP("textures/rip.bmp", true);
	m_texBino.loadBMP("textures/binoculars.bmp");

	// Set the model instance transforms
    for (int i = 0; i < 3; ++i) {
        ModelInstance rockInst(&m_rock);
        rockInst.transform(glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)));
        if (i == 0) {
            rockInst.transform(glm::translate(glm::vec3(2.0f, 0.8f, -2.0f)));
        }
        else if (i == 1) {
            rockInst.transform(glm::translate(glm::vec3(14.0f, 2.5f, 1.0f)));
        }
        else {
            rockInst.transform(glm::translate(glm::vec3(0.0f, 3.0f, -15.0f)));
        }
        m_kdModelInsts.push_back(rockInst);
    }
    ModelInstance rubiksCubeInst(&m_rubiksCube);
    rubiksCubeInst.transform(glm::scale(glm::vec3(0.1f, 0.1f, 0.1f)));
    rubiksCubeInst.transform(glm::translate(glm::vec3(-1.0f, 2.45f, -1.0f)));
    m_texturedModelInsts.push_back(rubiksCubeInst);
    ModelInstance goldRingInst(&m_goldRing);
    goldRingInst.transform(glm::scale(glm::vec3(0.15f, 0.15f, 0.15f)));
    goldRingInst.transform(glm::rotate(glm::radians(-20.0f), glm::vec3(0, 0, 1)));
    goldRingInst.transform(glm::rotate(glm::radians(5.0f), glm::vec3(1, 0, 0)));
    goldRingInst.transform(glm::translate(glm::vec3(2.5f, 0.62f, 1.5f)));
    m_kdModelInsts.push_back(goldRingInst);
    ModelInstance houseInst(&m_house);
    houseInst.transform(glm::scale(glm::vec3(3.5f, 4.0f, 3.5f)));
    houseInst.transform(glm::translate(glm::vec3(15.0f, 1.32f, 15.0f)));
    m_kdModelInsts.push_back(houseInst);
    m_tombstoneInst.transform(glm::scale(glm::vec3(1.0f, 1.0f, 0.25f)));
    m_tombstoneInst.transform(glm::rotate(glm::radians(-90.0f), glm::vec3(0, 1, 0)));
    m_tombstoneInst.transform(glm::translate(glm::vec3(3.0f, 2.8f, 13.0f)));

    m_gridMeshM = glm::rotate(glm::radians(-90.0f), glm::vec3(1, 0, 0));
    float scaleY = 4.0f;
    m_gridMeshM = glm::scale(glm::vec3(40.0f, scaleY, 40.0f)) * m_gridMeshM;
    m_gridMeshM = glm::translate(glm::vec3(-20.0f, 0.0f, 20.f)) * m_gridMeshM;

    for (int i = 0; i < 3; ++i) {
        TreeInstance tree;
        if (i == 0) {
            tree = TreeInstance(this, &m_cylinder, &m_leaves, glm::vec3(-5, 1, 3));
        }
        else if (i == 1) {
            tree = TreeInstance(this, &m_cylinder, &m_leaves, glm::vec3(-7, 3.2f, -6));
        }
        else {
            tree = TreeInstance(this, &m_cylinder, &m_leaves, glm::vec3(7.2f, 1.8f, -7.0f));
        }
        m_trees.push_back(tree);
    }

	// I don't have a mesh/geometry class for this alpha texture yet
	// alphat mesh
	float alphatVboData[16] = {
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &m_alphatVao);
	glBindVertexArray(m_alphatVao);
	glGenBuffers(1, &m_alphatVbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_alphatVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(alphatVboData), (void*)alphatVboData, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// init camera pos
	m_cam.m_pos = glm::vec3(0, 5, 8);
	m_cam.m_farZ = 100.0f;

    // Allocate the sun's shadow map
    glTextureStorage2D(m_texSunShadowMap.getId(), 1, GL_DEPTH_COMPONENT32, m_sunShadowTextureSize, m_sunShadowTextureSize);
    m_texSunShadowMap.setWrapS(GL_CLAMP_TO_BORDER);
    m_texSunShadowMap.setWrapT(GL_CLAMP_TO_BORDER);
    m_texSunShadowMap.setBorderColour(1.0f, 1.0f, 1.0f, 1.0f);
    glGenFramebuffers(1, &m_sunShadowMapFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_sunShadowMapFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texSunShadowMap.getId(), 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);


    // Allocate the sun's shadow map
    glTextureStorage2D(m_texFlShadowMap.getId(), 1, GL_DEPTH_COMPONENT32, m_flShadowTextureSize, m_flShadowTextureSize);
    m_texFlShadowMap.setWrapS(GL_CLAMP_TO_BORDER);
    m_texFlShadowMap.setWrapT(GL_CLAMP_TO_BORDER);
    m_texFlShadowMap.setBorderColour(1.0f, 1.0f, 1.0f, 1.0f);
    glGenFramebuffers(1, &m_flShadowMapFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_flShadowMapFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texFlShadowMap.getId(), 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glTextureStorage2D(m_texReflectedDepth.getId(), 1, GL_DEPTH_COMPONENT32, m_sceneWidth, m_sceneHeight);
    glTextureStorage2D(m_texReflectedScene.getId(), 1, GL_RGBA8, m_sceneWidth, m_sceneHeight);
    glGenFramebuffers(1, &m_reflectedFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_reflectedFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texReflectedDepth.getId(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texReflectedScene.getId(), 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glTextureStorage2D(m_texSceneDepth.getId(), 1, GL_DEPTH24_STENCIL8, m_sceneWidth, m_sceneHeight);
    glTextureStorage2D(m_texScene.getId(), 1, GL_RGBA8, m_sceneWidth, m_sceneHeight);
    glGenFramebuffers(1, &m_sceneFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_sceneFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_texSceneDepth.getId(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texScene.getId(), 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    DBG(std::cerr << "Line 136" << std::endl);


    glGenFramebuffers(2, m_blurFbos);
    for (int i = 0; i < 2; ++i) {
        glTextureStorage2D(m_texBlurs[i].getId(), 1, GL_RGBA8, m_sceneWidth / 2, m_sceneHeight / 2);
        glBindFramebuffer(GL_FRAMEBUFFER, m_blurFbos[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texBlurs[i].getId(), 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Scene::~Scene() {
	glDeleteVertexArrays(1, &m_alphatVao);
	glDeleteBuffers(1, &m_alphatVbo);
    glDeleteFramebuffers(1, &m_sunShadowMapFbo);
    glDeleteFramebuffers(1, &m_flShadowMapFbo);
    glDeleteFramebuffers(1, &m_reflectedFbo);
    glDeleteFramebuffers(1, &m_sceneFbo);
    glDeleteFramebuffers(2, m_blurFbos);
}

void Scene::tick(float dt) {
    m_sun.tick(dt);
    m_smoke.tick(dt);
    m_timeSinceLastSound += dt;
}

uint Scene::pickTarget() {
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_CLIP_DISTANCE0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_BLEND);

    if (m_binoMode) {
        m_cam.m_fovy = glm::radians(18.0f);
    }
    else {
        m_cam.m_fovy = glm::radians(50.0f);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    renderObjects(m_cam.getP(), m_cam.getV(), RenderType::Pick, 1.0f);
    glReadBuffer(GL_BACK);
    uint id = 0;
    glReadPixels(m_defaultFboW / 2, m_defaultFboH / 2, 1, 1, GL_RED, GL_UNSIGNED_BYTE, &id);

    DBG(std::cerr << "picked " << id << std::endl);
    return id;
}

void Scene::render() {
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_CLIP_DISTANCE0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindFramebuffer(GL_FRAMEBUFFER, m_sunShadowMapFbo);
    glViewport(0, 0, m_sunShadowTextureSize, m_sunShadowTextureSize);
    glClear(GL_DEPTH_BUFFER_BIT);
    if (m_shadowEnabled) {
        renderObjects(m_sun.getP(), m_sun.getV(m_cam.m_pos), RenderType::Shadow, 1.0f);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_flShadowMapFbo);
    glViewport(0, 0, m_flShadowTextureSize, m_flShadowTextureSize);
    glClear(GL_DEPTH_BUFFER_BIT);
    if (m_shadowEnabled) {
        renderObjects(m_flashlight.getP(), m_flashlight.getV(), RenderType::Shadow, 1.0f);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_sceneFbo);
    glViewport(0, 0, m_sceneWidth, m_sceneHeight);
    if (m_binoMode) {
        m_cam.m_fovy = glm::radians(18.0f);
    }
    else {
        m_cam.m_fovy = glm::radians(50.0f);
    }
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    renderObjects(m_cam.getP(), m_cam.getV(), RenderType::Normal, 1.0f);
    renderGround(m_cam.getP(), m_cam.getV(), 1.0f);
    renderLeaves(m_cam.getP(), m_cam.getV(), 1.0f);

    // draw the water and mark with stencil
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0, 0xff);
    glStencilOp(GL_KEEP, GL_KEEP,GL_INCR);
    renderWater(m_cam.getP(), m_cam.getV());
    glDisable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);
    if (m_sun.isNight()) {
        m_nightSkybox.draw(m_cam.getP() * glm::mat4(glm::mat3(m_cam.getV())), 1.0f);
    }
    else {
        m_daySkybox.draw(m_cam.getP() * glm::mat4(glm::mat3(m_cam.getV())), m_sun.getBrightness());
    }
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);

    glDepthMask(GL_FALSE); // smoke should not overwrite the depth buffer
    renderSmoke(m_cam.getP(), m_cam.getV());
    glDepthMask(GL_TRUE);

    if (m_reflectionEnabled) {
        // draw the objects reflected about the water plane
        glBindFramebuffer(GL_FRAMEBUFFER, m_reflectedFbo);
        glViewport(0, 0, m_sceneWidth, m_sceneHeight);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_BLEND);
        glEnable(GL_CLIP_DISTANCE0);
        glCullFace(GL_FRONT); // the faces will reverse direction when reflected
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        renderObjects(m_cam.getP(), m_cam.getV() * getReflectionMatrix(), RenderType::Normal, 0.3f);
        renderGround(m_cam.getP(), m_cam.getV() * getReflectionMatrix(), 0.3f);
        renderLeaves(m_cam.getP(), m_cam.getV() * getReflectionMatrix(), 0.3f);
        glDisable(GL_CLIP_DISTANCE0);
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // blend the reflections into the water
        glBindFramebuffer(GL_FRAMEBUFFER, m_sceneFbo);
        glViewport(0, 0, m_sceneWidth, m_sceneHeight);
        glEnable(GL_STENCIL_TEST);
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glStencilFunc(GL_LEQUAL, 1, 0xff);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        m_colourTextureProg.use();
        glBindVertexArray(m_alphatVao);
        glActiveTexture(GL_TEXTURE0);
        m_texReflectedScene.bind();
        glUniform1i(m_colourTextureProg["sampler"], 0);
        glm::mat3 I(1.0f);
        glUniformMatrix3fv(m_colourTextureProg["M"], 1, GL_FALSE, glm::value_ptr(I));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    if (m_binoMode) {
        //apply the blur
        blur(m_sceneFbo, m_texSceneDepth, m_texScene, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_defaultFboW, m_defaultFboH);
        glEnable(GL_BLEND);
        m_alphatextureProg.use();
        glBindVertexArray(m_alphatVao);
        glActiveTexture(GL_TEXTURE0);
        m_texBino.bind();
        glUniform1i(m_alphatextureProg["sampler"], 0);
        glm::mat3 binoM(1.0f);
        binoM[0][0] = 1.0f;
        binoM[1][1] = 0.5f * m_cam.m_aspect;
        glUniformMatrix3fv(m_alphatextureProg["M"], 1, GL_FALSE, glm::value_ptr(binoM));
        glUniform3f(m_alphatextureProg["colour"], 0, 0, 0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }
    else {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_sceneFbo);
        glBlitFramebuffer(0, 0, m_sceneWidth, m_sceneHeight, 0, 0, m_defaultFboW, m_defaultFboH, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    }
}

void Scene::blur(GLuint srcFbo, const Texture& srcDepthBuffer, const Texture& srcColourBuffer, GLuint dstFbo) {
    glm::mat3 I(1.0f);

    // Downsample the scene
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_blurFbos[0]);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFbo);
    glBlitFramebuffer(0, 0, m_sceneWidth, m_sceneHeight, 0, 0, m_sceneWidth / 2, m_sceneHeight / 2, GL_COLOR_BUFFER_BIT, GL_LINEAR);


    // Blur horizontal
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    m_gaussianProg.use();
    glBindFramebuffer(GL_FRAMEBUFFER, m_blurFbos[1]);
    glViewport(0, 0, m_sceneWidth / 2, m_sceneHeight / 2);
    glBindVertexArray(m_alphatVao);
    glActiveTexture(GL_TEXTURE0);
    m_texBlurs[0].bind();
    glUniform1i(m_gaussianProg["colourIn"], 0);
    glUniform1i(m_gaussianProg["isHorizontal"], 1);
    glUniformMatrix3fv(m_gaussianProg["M"], 1, GL_FALSE, glm::value_ptr(I));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    // Blur vertical
    glBindFramebuffer(GL_FRAMEBUFFER, m_blurFbos[0]);
    glActiveTexture(GL_TEXTURE0);
    m_texBlurs[1].bind();
    glUniform1i(m_gaussianProg["isHorizontal"], 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER, dstFbo);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, m_defaultFboW, m_defaultFboH);
    if (m_cam.m_aspect < 2.0f) {
        glEnable(GL_SCISSOR_TEST);
        GLint scissorX = 0;
        GLint scissorY = int((-0.5f * m_cam.m_aspect / 2.0f + 0.5f) * m_defaultFboH) + 2;
        GLint scissorW = m_defaultFboW;
        GLint scissorH = int(0.5f * m_cam.m_aspect * m_defaultFboH) - 4;
        glScissor(scissorX, scissorY, scissorW, scissorH);
    }
    m_blurProg.use();
    glBindVertexArray(m_alphatVao);
    glActiveTexture(GL_TEXTURE0);
    m_texBlurs[0].bind();
    glUniform1i(m_blurProg["blurryColourIn"], 0);
    glActiveTexture(GL_TEXTURE1);
    srcColourBuffer.bind();
    glUniform1i(m_blurProg["sharpColourIn"], 1);
    glActiveTexture(GL_TEXTURE2);
    srcDepthBuffer.bind();
    glUniform1i(m_blurProg["depthIn"], 2);
    glUniformMatrix3fv(m_blurProg["M"], 1, GL_FALSE, glm::value_ptr(I));
    glUniform1f(m_blurProg["nearClip"], m_cam.m_nearZ);
    glUniform1f(m_blurProg["farClip"], m_cam.m_farZ);
    glUniform1f(m_blurProg["kBlurry"], 2.0f);
    glUniform1f(m_blurProg["focusDistance"], m_binoFocusDist);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glDisable(GL_SCISSOR_TEST);
}

void Scene::renderObjects(const glm::mat4& P, const glm::mat4& V, RenderType renderType, float alpha) {
    if (renderType == RenderType::Shadow) {
        m_shadowProg.use();
        for (ModelInstance& inst : m_texturedModelInsts) {
            setPVM(m_shadowProg, P, V, inst.getM());
            inst.setUniformLocations(-1, -1, -1);
            inst.draw();
        }
    }
    else if (renderType == RenderType::Pick) {
        m_pickProg.use();
        for (ModelInstance& inst : m_texturedModelInsts) {
            inst.setUniformLocations(-1, -1, -1);
            setPVM(m_pickProg, P, V, inst.getM());
            glUniform1i(m_pickProg["id"], inst.getId());
            inst.draw();
        }
    }
    else {
        m_textureKdProg.use();
        glActiveTexture(GL_TEXTURE0);
        m_texRubiksCube.bind();
        glUniform1i(m_textureKdProg["sampler"], 0);
        glActiveTexture(GL_TEXTURE1);
        m_texSunShadowMap.bind();
        glUniform1i(m_textureKdProg["sunShadow"], 1);
        glActiveTexture(GL_TEXTURE2);
        m_texFlShadowMap.bind();
        glUniform1i(m_textureKdProg["flShadow"], 2);
        glUniform1f(m_textureKdProg["u_alpha"], alpha);
        setCommonUniforms(m_textureKdProg);
        for (ModelInstance& inst : m_texturedModelInsts) {
            setMatrixUniforms(m_textureKdProg, P, V, inst.getM());
            inst.setUniformLocations(-1, m_textureKdProg["Ks"], m_textureKdProg["Ns"]);
            inst.draw();
        }
    }

    if (renderType == RenderType::Shadow) {
        for (ModelInstance& inst : m_kdModelInsts) {
            setPVM(m_shadowProg, P, V, inst.getM());
            inst.setUniformLocations(-1, -1, -1);
            inst.draw();
        }
    }
    else if (renderType == RenderType::Pick) {
        for (ModelInstance& inst : m_kdModelInsts) {
            setPVM(m_pickProg, P, V, inst.getM());
            glUniform1i(m_pickProg["id"], inst.getId());
            inst.setUniformLocations(-1, -1, -1);
            inst.draw();
        }
    }
    else {
        m_constantKdProg.use();
        glActiveTexture(GL_TEXTURE0);
        m_texSunShadowMap.bind();
        glUniform1i(m_constantKdProg["sunShadow"], 0);
        glActiveTexture(GL_TEXTURE1);
        m_texFlShadowMap.bind();
        glUniform1i(m_constantKdProg["flShadow"], 1);
        glUniform1f(m_constantKdProg["u_alpha"], alpha);

        setCommonUniforms(m_constantKdProg);
        for (ModelInstance& inst : m_kdModelInsts) {
            setMatrixUniforms(m_constantKdProg, P, V, inst.getM());
            inst.setUniformLocations(m_constantKdProg["Kd"], m_constantKdProg["Ks"], m_constantKdProg["Ns"]);
            inst.draw();
        }
    }

    if (renderType == RenderType::Shadow) {
        for (TreeInstance& tree : m_trees) {
            tree.setUniformLocations(-1, -1, -1);
            tree.draw(m_shadowProg, P, V, true);
        }
        m_tombstoneInst.setUniformLocations(-1, -1, -1);
        setPVM(m_shadowProg, P, V, m_tombstoneInst.getM());
        m_tombstoneInst.draw();
    }
    else if (renderType == RenderType::Pick) {
        glUniform1i(m_pickProg["id"], m_cylinder.getId());
        for (TreeInstance& tree : m_trees) {
            tree.setUniformLocations(-1, -1, -1);
            tree.draw(m_pickProg, P, V, true);
        }
        glUniform1i(m_pickProg["id"], m_tombstone.getId());
        m_tombstoneInst.setUniformLocations(-1, -1, -1);
        setPVM(m_pickProg, P, V, m_tombstoneInst.getM());
        m_tombstoneInst.draw();
    }
    else {
        m_bumpmapProg.use();
        glUniform1f(m_bumpmapProg["u_alpha"], alpha);
        glUniform1f(m_bumpmapProg["bumpScale"], m_bumpmapEnabled ? 1.6f : 0.0f);
        setCommonUniforms(m_bumpmapProg);
        glActiveTexture(GL_TEXTURE0);
        m_texSunShadowMap.bind();
        glUniform1i(m_bumpmapProg["sunShadow"], 0);
        glActiveTexture(GL_TEXTURE1);
        m_texFlShadowMap.bind();
        glUniform1i(m_bumpmapProg["flShadow"], 1);
        glActiveTexture(GL_TEXTURE2);
        m_texBmapHeightfield.bind();
        glUniform1i(m_bumpmapProg["heightfield"], 2);
        for (TreeInstance& tree : m_trees) {
            tree.setUniformLocations(m_bumpmapProg["Kd"], m_bumpmapProg["Ks"], m_bumpmapProg["Ns"]);
            tree.draw(m_bumpmapProg, P, V, false);
        }
        m_texBmapRip.bind();
        m_tombstoneInst.setUniformLocations(m_bumpmapProg["Kd"], m_bumpmapProg["Ks"], m_bumpmapProg["Ns"]);
        setMatrixUniforms(m_bumpmapProg, P, V, m_tombstoneInst.getM());
        m_tombstoneInst.draw();
    }
}

void Scene::renderSmoke(const glm::mat4& P, const glm::mat4& V) {
    m_smoke.setPV(m_cam.getP() * m_cam.getV());
    m_smoke.setVinv(m_cam.getVInv());
    m_smoke.draw();
}

void Scene::renderGround(const glm::mat4& P, const glm::mat4& V, float alpha) {
    m_hmapProg.use();
    glActiveTexture(GL_TEXTURE0);
    m_texSunShadowMap.bind();
    glUniform1i(m_hmapProg["sunShadow"], 0);
    glActiveTexture(GL_TEXTURE1);
    m_texFlShadowMap.bind();
    glUniform1i(m_hmapProg["flShadow"], 1);
    glActiveTexture(GL_TEXTURE2);
    m_texHeightmap.bind();
    glUniform1i(m_hmapProg["sampler"], 2);
    glUniform1f(m_hmapProg["u_alpha"], alpha);
    setCommonUniforms(m_hmapProg);
    setMatrixUniforms(m_hmapProg, P, V, m_gridMeshM);
    glUniform3f(m_hmapProg["Ks"], 0, 0, 0);
    glUniform1f(m_hmapProg["Ns"], 0);
    glUniform3f(m_hmapProg["Kd"], 0.1f, 0.4f, 0.1f);
    m_gridMesh.draw();
}

void Scene::renderWater(const glm::mat4& P, const glm::mat4& V) {
    float scaleY = 4.0f;
    m_waterProg.use();
    glActiveTexture(GL_TEXTURE0);
    m_texSunShadowMap.bind();
    glUniform1i(m_waterProg["sunShadow"], 0);
    glActiveTexture(GL_TEXTURE1);
    m_texFlShadowMap.bind();
    glUniform1i(m_waterProg["flShadow"], 1);
    glActiveTexture(GL_TEXTURE2);
    m_texHeightmap.bind();
    glUniform1i(m_waterProg["sampler"], 2);

    setCommonUniforms(m_waterProg);
    setMatrixUniforms(m_waterProg, P, V, m_gridMeshM);
    glUniform3f(m_waterProg["Ks"], 1.0f, 1.0f, 1.0f);
    glUniform1f(m_waterProg["Ns"], 50.0f);
    glUniform3f(m_waterProg["Kd"], 0.1f, 0.1f, 0.9f);

    glUniform1f(m_waterProg["plane"], m_reflectionPlane / scaleY);
    glUniform1f(m_waterProg["maxAlphaDepth"], 0.25f);
    m_gridMesh.draw();

}

void Scene::renderLeaves(const glm::mat4& P, const glm::mat4& V, float alpha) {
    m_textureKdProg.use();
    glActiveTexture(GL_TEXTURE1);
    m_texSunShadowMap.bind();
    glUniform1i(m_textureKdProg["sunShadow"], 1);
    glActiveTexture(GL_TEXTURE2);
    m_texFlShadowMap.bind();
    glUniform1i(m_textureKdProg["flShadow"], 2);
    glUniform1f(m_textureKdProg["u_alpha"], alpha);
    setCommonUniforms(m_textureKdProg);
    m_leaves.setUniformLocations(m_textureKdProg["Ks"], m_textureKdProg["Ns"]);
    for (TreeInstance& tree : m_trees) {
        tree.drawLeaves(m_textureKdProg, P, V);
    }
}

void Scene::setPVM(const ShaderProgram& p, const glm::mat4& P, const glm::mat4& V, const glm::mat4& M) {
    glUniformMatrix4fv(p["PVM"], 1, GL_FALSE, glm::value_ptr(P * V * M));
}

void Scene::setCommonUniforms(const ShaderProgram& p) {
    glm::vec3 sunDir = m_sun.getLightDir();
    glUniform3fv(p["sunlightDir"], 1, glm::value_ptr(sunDir));
    glUniform3fv(p["ambientColour"], 1, glm::value_ptr(m_sun.getAmbientColour()));
    glUniform3fv(p["sunlightColour"], 1, glm::value_ptr(m_sun.getColour()));
    glUniform3fv(p["u_eye"], 1, glm::value_ptr(m_cam.m_pos));
    glUniform3fv(p["flPos"], 1, glm::value_ptr(m_flashlight.getPos()));
    glUniform3fv(p["flDir"], 1, glm::value_ptr(m_flashlight.getDir()));
    glUniform3fv(p["flColour"], 1, glm::value_ptr(m_flashlight.getColour()));
    glUniform1f(p["flSoftCutoff"], m_flashlight.getSoftCutoff());
    glUniform1f(p["flHardCutoff"], m_flashlight.getHardCutoff());
    glUniform1f(p["plane"], m_reflectionPlane);
}

void Scene::setMatrixUniforms(const ShaderProgram& p, const glm::mat4& P, const glm::mat4& V, const glm::mat4& M) {
    glm::mat4 PV = P * V;
    glm::mat4 sunPV = m_sun.getP() * m_sun.getV(m_cam.m_pos);
    glm::mat4 flPV = m_flashlight.getP() * m_flashlight.getV();
    glm::mat3 normMat = glm::mat3(glm::transpose(glm::inverse(M)));
    glm::mat4 pvm = PV * M;
    glUniformMatrix4fv(p["camPVM"], 1, false, glm::value_ptr(pvm));
    glUniformMatrix4fv(p["sunPVM"], 1, false, glm::value_ptr(sunPV * M));
    glUniformMatrix4fv(p["flPVM"], 1, false, glm::value_ptr(flPV * M));
    glUniformMatrix4fv(p["M"], 1, false, glm::value_ptr(M));
    glUniformMatrix3fv(p["normalMatrix"], 1, false, glm::value_ptr(normMat));
}

glm::mat4 Scene::getReflectionMatrix() const {
    glm::mat4 reflect = glm::mat4(glm::vec4(1,0,0,0), glm::vec4(0, -1, 0, 0), glm::vec4(0, 0, 1, 0), glm::vec4(0, 0, 0, 1));
    return glm::translate(glm::vec3(0, m_reflectionPlane, 0)) * reflect * glm::translate(glm::vec3(0, -m_reflectionPlane, 0));
}

void Scene::changeFocusDistance(float delta) {
    const float minFocusDistance = 0.1f;
    const float maxFocusDistance = 50.0f;
    m_binoFocusDist += delta;
    if (m_binoFocusDist > maxFocusDistance) {
        m_binoFocusDist = maxFocusDistance;
    }
    if (m_binoFocusDist < minFocusDistance) {
        m_binoFocusDist = minFocusDistance;
    }
}

bool Scene::detectCollision() const {
    Sphere camSphere(0.2f, m_cam.m_pos);

    for (const ModelInstance& m : m_texturedModelInsts) {
        if (intersects(m.getAABB(), camSphere)) {
            DBG(std::cerr << "collision detected with id " << m.getId() << std::endl);
            return true;
        }
    }

    for (const ModelInstance& m : m_kdModelInsts) {
        if (intersects(m.getAABB(), camSphere)) {
            DBG(std::cerr << "collision detected with id " << m.getId() << std::endl);
            return true;
        }
    }

    for (const TreeInstance& t : m_trees) {
        if (t.collides(camSphere)) {
            DBG(std::cerr << "collision detected with tree" << std::endl);
            return true;
        }
    }

    if (intersects(m_tombstoneInst.getAABB(), camSphere)) {
        DBG(std::cerr << "collision detected with tombstone" << std::endl);
        return true;
    }

    // collisions with the ground aren't very good without a better AABB
    /*if (intersects(m_gridMesh.getAABB().transform(m_gridMeshM), camSphere)) {
        DBG(std::cerr << "collision detected with ground" << std::endl);
        return true;
    }*/

    AABB aeeaBoundaries[4] = { AABB(glm::vec3(20, -100, -100), glm::vec3(21, 100, 100)),
                              AABB(glm::vec3(-21, -100, -100), glm::vec3(-20, 100, 100)),
                              AABB(glm::vec3(-100, -100, -21), glm::vec3(100, 100, -20)),
                              AABB(glm::vec3(-100, -100, 20), glm::vec3(100, 100, 21)), };
    for (int i = 0; i < 4; ++i) {
        if (intersects(aeeaBoundaries[i], camSphere)) {
            return true;
        }
    }

    return false;
}

void Scene::moveCamera(glm::vec3 delta) {
    m_cam.m_pos += delta;
    bool collision = detectCollision();
    if (collision) {
        m_cam.m_pos -= delta;
        if (m_timeSinceLastSound > 0.5f) {
            m_audioDevice.playSound(m_collideSound);
            m_timeSinceLastSound = 0.0f;
        }
    }
}