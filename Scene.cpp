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
    m_alphatextureProg("shaders/alphatexture.vs", "shaders/alphatexture.fs"),
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
    m_reflectionPlane(2.5f)
{
	// load textures
	m_tex123456.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	m_tex123456.loadDDS("textures/123456.dds");
	m_texHeightmap.loadBMP("textures/heightmap.bmp");
	m_texBmapHeightfield.setWrapS(GL_MIRRORED_REPEAT);
	m_texBmapHeightfield.setWrapT(GL_MIRRORED_REPEAT);
	m_texBmapHeightfield.loadBMP("textures/heightfield.bmp");
	m_texBino.loadBMP("textures/binoculars.bmp");

	// Set the model instance transforms
	m_tree1.transform(glm::scale(glm::vec3(0.5f, 1.0f, 0.5f)));
	m_tree1.transform(glm::translate(glm::vec3(2.0f, 0.0f, -2.0f)));
	m_cube1.transform(glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)));
	m_cube1.transform(glm::translate(glm::vec3(4.0f, 1.0f, 0.0f)));
	m_cyl1.transform(glm::rotate(glm::radians(-90.0f), glm::vec3(1, 0, 0)));
	m_cyl1.transform(glm::scale(glm::vec3(0.3f, 4.0f, 0.3f)));
	m_cyl1.transform(glm::translate(glm::vec3(3.0f, -0.2f, 1.0f)));

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
	m_cam.m_pos = glm::vec3(0, 0, 8);
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
}

Scene::~Scene() {
	glDeleteVertexArrays(1, &m_alphatVao);
	glDeleteBuffers(1, &m_alphatVbo);
    glDeleteFramebuffers(1, &m_sunShadowMapFbo);
    glDeleteFramebuffers(1, &m_flShadowMapFbo);
}

void Scene::render() {
    //m_sun.tick(0.004f);

    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_STENCIL_TEST);
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

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_defaultFboW, m_defaultFboH);
    if (m_binoMode) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        m_cam.m_fovy = glm::radians(18.0f);
    }
    else {
        //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        m_cam.m_fovy = glm::radians(50.0f);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    if (m_binoMode) {
        glEnable(GL_SCISSOR_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        GLint scissorX = 0;
        GLint scissorY = int((-0.5f * m_cam.m_aspect / 2.0f + 0.5f) * m_defaultFboH) + 2;
        GLint scissorW = m_defaultFboW;
        GLint scissorH = int(0.5f * m_cam.m_aspect * m_defaultFboH) - 4;
        glScissor(scissorX, scissorY, scissorW, scissorH);
    }

    renderObjects(m_cam.getP(), m_cam.getV(), false, 1.0f);
    renderGround(m_cam.getP(), m_cam.getV());

    // draw the water and mark with stencil
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0, 0xff);
    glStencilOp(GL_KEEP, GL_KEEP,GL_INCR);
    renderWater(m_cam.getP(), m_cam.getV());
    glDisable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    m_smoke.tick();
    glDepthMask(GL_FALSE); // smoke should not overwrite the depth buffer
    renderSmoke(m_cam.getP(), m_cam.getV());
    glDepthMask(GL_TRUE);

    // draw the reflected objects in the water
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_LEQUAL, 1, 0xff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glCullFace(GL_FRONT); // the faces will reverse direction when reflected
    glClear(GL_DEPTH_BUFFER_BIT);
    renderObjects(m_cam.getP(), m_cam.getV() * getReflectionMatrix(), false, 0.5f);

    if (m_binoMode) {
        glDisable(GL_DEPTH_TEST);
        m_alphatextureProg.use();
        glBindVertexArray(m_alphatVao);
        glActiveTexture(GL_TEXTURE0);
        m_texBino.bind();
        glUniform1i(m_alphatextureProg["sampler"], 0);
        glm::mat3 binoM(1.0f);
        binoM[0][0] = 1.0f;
        binoM[1][1] = 0.5f * m_cam.m_aspect;
        glUniformMatrix3fv(m_alphatextureProg["M"], 1, GL_FALSE, glm::value_ptr(binoM));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }
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

void Scene::renderGround(const glm::mat4& P, const glm::mat4& V) {
    glm::mat4 meshM = glm::rotate(glm::radians(-90.0f), glm::vec3(1, 0, 0));
    float scaleY = 4.0f;
    meshM = glm::scale(glm::vec3(40.0f, scaleY, 40.0f)) * meshM;
    meshM = glm::translate(glm::vec3(-20.0f, -2.5f, 20.f)) * meshM;
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
    meshM = glm::translate(glm::vec3(-20.0f, -2.5f, 20.f)) * meshM;
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
    glUniform1f(m_waterProg["maxAlphaDepth"], 0.05f);
    m_gridMesh.draw();

}

void Scene::setShadowPVM(const glm::mat4& P, const glm::mat4& V, const glm::mat4& M) {
    glUniformMatrix4fv(m_shadowProg["PVM"], 1, GL_FALSE, glm::value_ptr(P * V * M));
}

void Scene::setCommonUniforms(const ShaderProgram& p, const glm::mat4& P, const glm::mat4& V, const glm::mat4& M) {
    glm::vec3 sunDir = m_sun.getLightDir();
    glm::mat4 PV = P * V;// *getReflectionMatrix();
    glm::mat4 sunPV = m_sun.getP() * m_sun.getV(m_cam.m_pos);// *getReflectionMatrix();
    glm::mat4 flPV = m_flashlight.getP() * m_flashlight.getV();// *getReflectionMatrix();
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
}

glm::mat4 Scene::getReflectionMatrix() const {
    glm::mat4 reflect = glm::mat4(glm::vec4(1,0,0,0), glm::vec4(0, -1, 0, 0), glm::vec4(0, 0, 1, 0), glm::vec4(0, 0, 0, 1));
    return glm::translate(glm::vec3(0,m_reflectionPlane,0)) * reflect * glm::translate(glm::vec3(0, m_reflectionPlane, 0));
}
