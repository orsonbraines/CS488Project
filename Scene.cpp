#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

#include "Scene.h"
#include "Util.h"

Scene::Scene() :
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
	m_tree("models/simpletree.obj"),
	m_tree1(&m_tree),
	m_cube("models/texturedCube.obj"),
	m_cube1(&m_cube),
	m_cylinder(),
	m_cyl1(&m_cylinder),
	m_gridMesh(128,128),
	m_smoke(15000),
    m_binoMode(false),
    m_defaultFboW(0),
    m_defaultFboH(0),
    m_sunShadowTextureSize(2048),
    m_flShadowTextureSize(1024),
    m_sceneWidth(640),
    m_sceneHeight(360),
    m_reflectionPlane(1.0f)
{
	// load textures
	m_tex123456.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	m_tex123456.loadDDS("textures/123456.dds");
    m_texHeightmap.loadBMP("textures/noise.bmp");
	m_texBmapHeightfield.setWrapS(GL_MIRRORED_REPEAT);
	m_texBmapHeightfield.setWrapT(GL_MIRRORED_REPEAT);
	m_texBmapHeightfield.loadBMP("textures/heightfield.bmp");
	m_texBino.loadBMP("textures/binoculars.bmp");

	// Set the model instance transforms
	m_tree1.transform(glm::scale(glm::vec3(0.5f, 1.0f, 0.5f)));
	m_tree1.transform(glm::translate(glm::vec3(2.0f, 0.0f, -2.0f)));
	m_cube1.transform(glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)));
	m_cube1.transform(glm::translate(glm::vec3(4.0f, 0.0f, 0.0f)));
	m_cyl1.transform(glm::rotate(glm::radians(-90.0f), glm::vec3(1, 0, 0)));
	m_cyl1.transform(glm::scale(glm::vec3(0.3f, 4.0f, 0.3f)));
	m_cyl1.transform(glm::translate(glm::vec3(3.0f, 0.0f, 1.0f)));

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
	m_cam.m_pos = glm::vec3(0, m_reflectionPlane + 0.5f, 8);
	//m_cam.yaw = glm::radians(180.0f);
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

void Scene::render() {
    m_sun.tick(0.004f);

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
    renderObjects(m_sun.getP(), m_sun.getV(m_cam.m_pos), true, 1.0f);

    glBindFramebuffer(GL_FRAMEBUFFER, m_flShadowMapFbo);
    glViewport(0, 0, m_flShadowTextureSize, m_flShadowTextureSize);
    glClear(GL_DEPTH_BUFFER_BIT);
    renderObjects(m_flashlight.getP(), m_flashlight.getV(), true, 1.0f);

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
    glEnable(GL_DEPTH_TEST);

    renderObjects(m_cam.getP(), m_cam.getV(), false, 1.0f);
    renderGround(m_cam.getP(), m_cam.getV(), 1.0f);

    // draw the water and mark with stencil
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0, 0xff);
    glStencilOp(GL_KEEP, GL_KEEP,GL_INCR);
    renderWater(m_cam.getP(), m_cam.getV());
    glDisable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);
    m_skybox.draw(m_cam.getP() * glm::mat4(glm::mat3(m_cam.getV())));
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);

    m_smoke.tick();
    glDepthMask(GL_FALSE); // smoke should not overwrite the depth buffer
    renderSmoke(m_cam.getP(), m_cam.getV());
    glDepthMask(GL_TRUE);

    // draw the objects reflected about the water plane
    glBindFramebuffer(GL_FRAMEBUFFER, m_reflectedFbo);
    glViewport(0,0,m_sceneWidth,m_sceneHeight);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_BLEND);
    glEnable(GL_CLIP_DISTANCE0);
    glCullFace(GL_FRONT); // the faces will reverse direction when reflected
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    renderObjects(m_cam.getP(), m_cam.getV() * getReflectionMatrix(), false, 0.3f);
    renderGround(m_cam.getP(), m_cam.getV() * getReflectionMatrix(), 0.3f);
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
    glUniform1f(m_blurProg["focusDistance"], 3.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glDisable(GL_SCISSOR_TEST);
}

void Scene::renderObjects(const glm::mat4& P, const glm::mat4& V, bool isShadow, float alpha) {
    if (isShadow) {
        m_shadowProg.use();
        setShadowPVM(P, V, m_cube1.getM());
        m_cube1.setUniformLocations(-1, -1, -1);
    }
    else {
        m_textureKdProg.use();
        glActiveTexture(GL_TEXTURE0);
        m_tex123456.bind();
        glUniform1i(m_textureKdProg["sampler"], 0);
        glActiveTexture(GL_TEXTURE1);
        m_texSunShadowMap.bind();
        glUniform1i(m_textureKdProg["sunShadow"], 1);
        glActiveTexture(GL_TEXTURE2);
        m_texFlShadowMap.bind();
        glUniform1i(m_textureKdProg["flShadow"], 2);
        glUniform1f(m_textureKdProg["u_alpha"], alpha);
        setCommonUniforms(m_textureKdProg, P, V, m_cube1.getM());
        m_cube1.setUniformLocations(-1, m_textureKdProg["Ks"], m_textureKdProg["Ns"]);
    }
    m_cube1.draw();

    if (isShadow) {
        setShadowPVM(P, V, m_tree1.getM());
        m_tree1.setUniformLocations(-1, -1, -1);
    }
    else {
        m_constantKdProg.use();
        glActiveTexture(GL_TEXTURE0);
        m_texSunShadowMap.bind();
        glUniform1i(m_constantKdProg["sunShadow"], 0);
        glActiveTexture(GL_TEXTURE1);
        m_texFlShadowMap.bind();
        glUniform1i(m_constantKdProg["flShadow"], 1);
        setCommonUniforms(m_constantKdProg, P, V, m_tree1.getM());
        glUniform1f(m_constantKdProg["u_alpha"], alpha);
        m_tree1.setUniformLocations(m_constantKdProg["Kd"], m_constantKdProg["Ks"], m_constantKdProg["Ns"]);
    }
    m_tree1.draw();

    if (isShadow) {
        setShadowPVM(P, V, m_cyl1.getM());
    }
    else {
        m_bumpmapProg.use();
        glUniform3f(m_bumpmapProg["Ks"], 0, 0, 0);
        glUniform1f(m_bumpmapProg["Ns"], 0);
        glUniform3f(m_bumpmapProg["Kd"], 0.329826f, 0.060918f, 0.008916f);
        glUniform1f(m_bumpmapProg["u_alpha"], alpha);
        setCommonUniforms(m_bumpmapProg, P, V, m_cyl1.getM());
        glActiveTexture(GL_TEXTURE0);
        m_texSunShadowMap.bind();
        glUniform1i(m_bumpmapProg["sunShadow"], 0);
        glActiveTexture(GL_TEXTURE1);
        m_texFlShadowMap.bind();
        glUniform1i(m_bumpmapProg["flShadow"], 1);
        glActiveTexture(GL_TEXTURE2);
        m_texBmapHeightfield.bind();
        glUniform1i(m_bumpmapProg["heightfield"], 2);
    }
    m_cyl1.draw();
}

void Scene::renderSmoke(const glm::mat4& P, const glm::mat4& V) {
    m_smoke.setPV(m_cam.getP() * m_cam.getV());
    m_smoke.setVinv(m_cam.getVInv());
    m_smoke.draw();
}

void Scene::renderGround(const glm::mat4& P, const glm::mat4& V, float alpha) {
    glm::mat4 meshM = glm::rotate(glm::radians(-90.0f), glm::vec3(1, 0, 0));
    float scaleY = 4.0f;
    meshM = glm::scale(glm::vec3(40.0f, scaleY, 40.0f)) * meshM;
    meshM = glm::translate(glm::vec3(-20.0f, 0.0f, 20.f)) * meshM;
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
    setCommonUniforms(m_hmapProg, P, V, meshM);
    glUniform3f(m_hmapProg["Ks"], 0, 0, 0);
    glUniform1f(m_hmapProg["Ns"], 0);
    glUniform3f(m_hmapProg["Kd"], 0.1f, 0.4f, 0.1f);
    m_gridMesh.draw();
}

void Scene::renderWater(const glm::mat4& P, const glm::mat4& V) {
    glm::mat4 meshM = glm::rotate(glm::radians(-90.0f), glm::vec3(1, 0, 0));
    float scaleY = 4.0f;
    meshM = glm::scale(glm::vec3(40.0f, scaleY, 40.0f)) * meshM;
    meshM = glm::translate(glm::vec3(-20.0f, 0.0f, 20.f)) * meshM;
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

    setCommonUniforms(m_waterProg, P, V, meshM);
    glUniform3f(m_waterProg["Ks"], 1.0f, 1.0f, 1.0f);
    glUniform1f(m_waterProg["Ns"], 50.0f);
    glUniform3f(m_waterProg["Kd"], 0.1f, 0.1f, 0.9f);

    glUniform1f(m_waterProg["plane"], m_reflectionPlane / scaleY);
    glUniform1f(m_waterProg["maxAlphaDepth"], 0.25f);
    m_gridMesh.draw();

}

void Scene::setShadowPVM(const glm::mat4& P, const glm::mat4& V, const glm::mat4& M) {
    glUniformMatrix4fv(m_shadowProg["PVM"], 1, GL_FALSE, glm::value_ptr(P * V * M));
}

void Scene::setCommonUniforms(const ShaderProgram& p, const glm::mat4& P, const glm::mat4& V, const glm::mat4& M) {
    glm::vec3 sunDir = m_sun.getLightDir();
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

glm::mat4 Scene::getReflectionMatrix() const {
    glm::mat4 reflect = glm::mat4(glm::vec4(1,0,0,0), glm::vec4(0, -1, 0, 0), glm::vec4(0, 0, 1, 0), glm::vec4(0, 0, 0, 1));
    return glm::translate(glm::vec3(0, m_reflectionPlane, 0)) * reflect * glm::translate(glm::vec3(0, -m_reflectionPlane, 0));
}
