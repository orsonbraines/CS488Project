#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>


#include "Scene.h"

Scene::Scene() :
    m_textureKdProg("shaders/textureKd.vs", "shaders/textureKd.fs"),
    m_constantKdProg("shaders/constantKd.vs", "shaders/constantKd.fs"),
    m_hmapProg("shaders/hmap.vs", "shaders/constantKd.fs"),
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
	m_smoke(5000),
    m_binoMode(false),
    m_defaultFboW(0),
    m_defaultFboH(0),
    m_shadowTextureSize(2048)
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
	m_cube1.transform(glm::translate(glm::vec3(-0.5f, 0.5f, 0.5f)));
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
	m_cam.pos = glm::vec3(0, 0, 8);
	m_cam.yaw = glm::radians(180.0f);
	m_cam.farZ = 100.0f;

    // Allocate the shadow map
    glTextureStorage2D(m_texShadowMap.getId(), 1, GL_DEPTH_COMPONENT32, m_shadowTextureSize, m_shadowTextureSize);
    m_texShadowMap.setWrapS(GL_CLAMP_TO_BORDER);
    m_texShadowMap.setWrapT(GL_CLAMP_TO_BORDER);
    m_texShadowMap.setBorderColour(1.0f, 1.0f, 1.0f, 1.0f);
    glGenFramebuffers(1, &m_shadowMapFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texShadowMap.getId(), 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Scene::~Scene() {
	glDeleteVertexArrays(1, &m_alphatVao);
	glDeleteBuffers(1, &m_alphatVbo);
    glDeleteFramebuffers(1, &m_shadowMapFbo);
}

void Scene::render() {
    m_sun.setTheta(m_sun.getTheta() + 0.0001f);

    glDisable(GL_SCISSOR_TEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFbo);
    glViewport(0, 0, m_shadowTextureSize, m_shadowTextureSize);
    glClear(GL_DEPTH_BUFFER_BIT);
    renderObjects(m_sun.getP(), m_sun.getV(m_cam.pos), true);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_defaultFboW, m_defaultFboH);
    if (m_binoMode) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        m_cam.fovy = glm::radians(18.0f);
    }
    else {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        m_cam.fovy = glm::radians(50.0f);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    if (m_binoMode) {
        glEnable(GL_SCISSOR_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        GLint scissorX = 0;
        GLint scissorY = int((-0.5f * m_cam.aspect / 2.0f + 0.5f) * m_defaultFboH) + 2;
        GLint scissorW = m_defaultFboW;
        GLint scissorH = int(0.5f * m_cam.aspect * m_defaultFboH) - 4;
        glScissor(scissorX, scissorY, scissorW, scissorH);
    }

    renderObjects(m_cam.getP(), m_cam.getV(), false);

    m_smoke.setPV(m_cam.getP() * m_cam.getV());
    m_smoke.tick();
    glDepthMask(GL_FALSE); // smoke should not overwrite the depth buffer
    m_smoke.draw();
    glDepthMask(GL_TRUE);

    if (m_binoMode) {
        glDisable(GL_DEPTH_TEST);
        m_alphatextureProg.use();
        glBindVertexArray(m_alphatVao);
        glActiveTexture(GL_TEXTURE0);
        m_texBino.bind();
        glUniform1i(m_alphatextureProg["sampler"], 0);
        glm::mat3 binoM(1.0f);
        binoM[0][0] = 1.0f;
        binoM[1][1] = 0.5f * m_cam.aspect;
        glUniformMatrix3fv(m_alphatextureProg["M"], 1, GL_FALSE, glm::value_ptr(binoM));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }
}

void Scene::renderObjects(const glm::mat4& P, const glm::mat4& V, bool isShadow) {
    glm::vec3 sunDir = m_sun.getLightDir();
    glm::mat4 PV = P * V;
    glm::mat4 sunPV = m_sun.getP() * m_sun.getV(m_cam.pos);

    if (isShadow) {
        m_shadowProg.use();
        glUniformMatrix4fv(m_shadowProg["PVM"], 1, GL_FALSE, glm::value_ptr(PV * m_cube1.getM()));
        m_cube1.setUniformLocations(-1, -1, -1);  
    }
    else {
        m_textureKdProg.use();
        glActiveTexture(GL_TEXTURE0);
        m_tex123456.bind();
        glUniform1i(m_textureKdProg["sampler"], 0);
        glm::mat4 pvm = PV * m_cube1.getM();
        glm::mat3 normMat = glm::mat3(glm::transpose(glm::inverse(m_cube1.getM())));
        glUniformMatrix4fv(m_textureKdProg["PVM"], 1, false, glm::value_ptr(pvm));
        glUniformMatrix4fv(m_textureKdProg["M"], 1, false, glm::value_ptr(m_cube1.getM()));
        glUniformMatrix3fv(m_textureKdProg["normalMatrix"], 1, false, glm::value_ptr(normMat));
        glUniform3fv(m_textureKdProg["lightDir"], 1, glm::value_ptr(sunDir));
        glUniform3f(m_textureKdProg["ambientColour"], 0.5f, 0.5f, 0.5f);
        glUniform3f(m_textureKdProg["lightColour"], 0.5f, 0.0f, 0.0f);
        glUniform3fv(m_textureKdProg["vs_eye"], 1, glm::value_ptr(m_cam.pos));
        m_cube1.setUniformLocations(-1, m_textureKdProg["Ks"], m_textureKdProg["Ns"]);
    }
    m_cube1.draw();

    if (isShadow) {
        glUniformMatrix4fv(m_shadowProg["PVM"], 1, GL_FALSE, glm::value_ptr(PV * m_tree1.getM()));
        m_tree1.setUniformLocations(-1, -1, -1);
    }
    else {
        m_constantKdProg.use();
        glm::mat4 pvm = PV * m_tree1.getM();
        glm::mat3 normMat = glm::mat3(glm::transpose(glm::inverse(m_tree1.getM())));
        glUniformMatrix4fv(m_constantKdProg["camPVM"], 1, false, glm::value_ptr(pvm));
        glUniformMatrix4fv(m_constantKdProg["sunPVM"], 1, false, glm::value_ptr(sunPV * m_tree1.getM()));
        glActiveTexture(GL_TEXTURE0);
        m_texShadowMap.bind();
        glUniform1i(m_constantKdProg["shadow"], 0);
        glUniformMatrix4fv(m_constantKdProg["M"], 1, false, glm::value_ptr(m_tree1.getM()));
        glUniformMatrix3fv(m_constantKdProg["normalMatrix"], 1, false, glm::value_ptr(normMat));
        glUniform3fv(m_constantKdProg["lightDir"], 1, glm::value_ptr(sunDir));
        glUniform3f(m_constantKdProg["ambientColour"], 0.5f, 0.5f, 0.5f);
        glUniform3f(m_constantKdProg["lightColour"], 1.0f, 1.0f, 1.0f);
        glUniform3fv(m_constantKdProg["vs_eye"], 1, glm::value_ptr(m_cam.pos));
        m_tree1.setUniformLocations(m_constantKdProg["Kd"], m_constantKdProg["Ks"], m_constantKdProg["Ns"]);
    }
    m_tree1.draw();

    if (isShadow) {
        glUniformMatrix4fv(m_shadowProg["PVM"], 1, GL_FALSE, glm::value_ptr(PV * m_cyl1.getM()));
    }
    else {
        m_bumpmapProg.use();
        glm::mat4 pvm = PV * m_cyl1.getM();
        glm::mat3 normMat = glm::mat3(glm::transpose(glm::inverse(m_cyl1.getM())));
        glUniformMatrix4fv(m_bumpmapProg["PVM"], 1, false, glm::value_ptr(pvm));
        glUniformMatrix4fv(m_bumpmapProg["M"], 1, false, glm::value_ptr(m_cyl1.getM()));
        glUniformMatrix3fv(m_bumpmapProg["normalMatrix"], 1, false, glm::value_ptr(normMat));
        glUniform3f(m_bumpmapProg["Ks"], 0, 0, 0);
        glUniform1f(m_bumpmapProg["Ns"], 0);
        glUniform3f(m_bumpmapProg["Kd"], 0.329826f, 0.060918f, 0.008916f);
        glUniform3fv(m_bumpmapProg["lightDir"], 1, glm::value_ptr(sunDir));
        glUniform3f(m_bumpmapProg["ambientColour"], 0.5f, 0.5f, 0.5f);
        glUniform3f(m_bumpmapProg["lightColour"], 1.0f, 1.0f, 1.0f);
        glUniform3fv(m_bumpmapProg["vs_eye"], 1, glm::value_ptr(m_cam.pos));
        glActiveTexture(GL_TEXTURE1);
        m_texBmapHeightfield.bind();
        glUniform1i(m_bumpmapProg["heightfield"], 1);
    }
    m_cyl1.draw();
    if (!isShadow) {
        glm::mat4 meshM = glm::rotate(glm::radians(-90.0f), glm::vec3(1, 0, 0));
        meshM = glm::scale(glm::vec3(40.0f, 4.0f, 40.0f)) * meshM;
        meshM = glm::translate(glm::vec3(-20.0f, -2.5f, 20.f)) * meshM;
        glm::mat4 pvm = PV * meshM;
        // can't use shadow program here because pos isn't in attrib 0
        m_hmapProg.use();

        glActiveTexture(GL_TEXTURE1);
        m_texHeightmap.bind();
        glUniform1i(m_hmapProg["sampler"], 1);
        glm::mat3 normMat = glm::mat3(glm::transpose(glm::inverse(meshM)));
        glUniformMatrix4fv(m_hmapProg["camPVM"], 1, false, glm::value_ptr(pvm));
        glUniformMatrix4fv(m_hmapProg["sunPVM"], 1, false, glm::value_ptr(sunPV * meshM));
        glUniformMatrix4fv(m_hmapProg["M"], 1, false, glm::value_ptr(meshM));
        glUniformMatrix3fv(m_hmapProg["normalMatrix"], 1, false, glm::value_ptr(normMat));
        glUniform3fv(m_hmapProg["lightDir"], 1, glm::value_ptr(sunDir));
        glUniform3f(m_hmapProg["ambientColour"], 0.5f, 0.5f, 0.5f);
        glUniform3f(m_hmapProg["lightColour"], 1.0f, 1.0f, 1.0f);
        glUniform3fv(m_hmapProg["vs_eye"], 1, glm::value_ptr(m_cam.pos));
        glUniform3f(m_hmapProg["Ks"], 0, 0, 0);
        glUniform1f(m_hmapProg["Ns"], 0);
        glUniform3f(m_hmapProg["Kd"], 0.0f, 0.5f, 0.0f);
        if (!isShadow) {
            glActiveTexture(GL_TEXTURE0);
            m_texShadowMap.bind();
            glUniform1i(m_hmapProg["shadow"], 0);
        }
        m_gridMesh.draw();
    }
}