#include <cassert>
#include <iostream>
#include <vector>
#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>

#include "Util.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Camera.h"
#include "Model.h"
#include "GridMesh.h"
#include "Cylinder.h"
#include "SmokeSystem.h"
#include "Sun.h"

int main(int ArgCount, char** Args)
{
    if (!SDL_WasInit(SDL_INIT_EVERYTHING)) {
        SDL_Init(SDL_INIT_EVERYTHING);
    }
    int framebuffer_w = 640;
    int framebuffer_h = 360;
    int window_w = 640;
    int window_h = 360;
    unsigned WindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_Window* window = SDL_CreateWindow("SDL OpenGL Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_w, window_h, WindowFlags);
    assert(window);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    int err = SDL_SetRelativeMouseMode(SDL_TRUE);
    assert(err == 0);
    int glewErr = glewInit();
    if (glewErr != GLEW_OK) {
        std::cerr << glewGetErrorString(glewErr) << std::endl;
        return 1;
    }
    if (!glewIsSupported("GL_VERSION_4_6")) {
        std::cout << "opengl version 4.6 not supported" << std::endl;
        return 1;
    }
    if (!glewIsSupported("GL_EXT_texture_compression_s3tc")) {
        std::cout << "compressed textures not supported" << std::endl;
        return 1;
    }

    try {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(loggingCallback, nullptr);

        ShaderProgram program("shaders/vert.vs", "shaders/frag.fs");
        ShaderProgram program2("shaders/vert2.vs", "shaders/frag2.fs");
        ShaderProgram hmapProg("shaders/hmap.vs", "shaders/frag2.fs");
        ShaderProgram bumpmapProg("shaders/bumpmap.vs", "shaders/bumpmap.fs");
        ShaderProgram alphatextureProg("shaders/alphatexture.vs", "shaders/alphatexture.fs");


        //GLint prog1PVMLocation = glGetUniformLocation(program.getId(), "PVM");
        //GLint prog1MLocation = glGetUniformLocation(program.getId(), "M");
        //GLint prog1NormalMatrixLocation = glGetUniformLocation(program.getId(), "normalMatrix");
        //GLint prog1VsEyeLocation = glGetUniformLocation(program.getId(), "vs_eye");
        //GLint prog1SamplerLocation = glGetUniformLocation(program.getId(), "sampler");
        //GLint prog1LightDirLocation = glGetUniformLocation(program.getId(), "lightDir");
        //GLint prog1LightColourLocation = glGetUniformLocation(program.getId(), "lightColour");
        //GLint prog1AmbientColourLocation = glGetUniformLocation(program.getId(), "ambientColour");
        //GLint prog1KsLocation = glGetUniformLocation(program.getId(), "Ks");
        //GLint prog1NsLocation = glGetUniformLocation(program.getId(), "Ns");

        GLint prog2PVMLocation = glGetUniformLocation(program2.getId(), "PVM");
        GLint prog2MLocation = glGetUniformLocation(program2.getId(), "M");
        GLint prog2NormalMatrixLocation = glGetUniformLocation(program2.getId(), "normalMatrix");
        GLint prog2VsEyeLocation = glGetUniformLocation(program2.getId(), "vs_eye");
        GLint prog2LightDirLocation = glGetUniformLocation(program2.getId(), "lightDir");
        GLint prog2LightColourLocation = glGetUniformLocation(program2.getId(), "lightColour");
        GLint prog2AmbientColourLocation = glGetUniformLocation(program2.getId(), "ambientColour");
        GLint prog2KdLocation = glGetUniformLocation(program2.getId(), "Kd");
        GLint prog2KsLocation = glGetUniformLocation(program2.getId(), "Ks");
        GLint prog2NsLocation = glGetUniformLocation(program2.getId(), "Ns");

        GLint hmapSamplerLocation = glGetUniformLocation(hmapProg.getId(), "sampler");
        GLint hmapPVMLocation = glGetUniformLocation(hmapProg.getId(), "PVM");
        GLint hmapMLocation = glGetUniformLocation(hmapProg.getId(), "M");
        GLint hmapNormalMatrixLocation = glGetUniformLocation(hmapProg.getId(), "normalMatrix");
        GLint hmapVsEyeLocation = glGetUniformLocation(hmapProg.getId(), "vs_eye");
        GLint hmapLightDirLocation = glGetUniformLocation(hmapProg.getId(), "lightDir");
        GLint hmapLightColourLocation = glGetUniformLocation(hmapProg.getId(), "lightColour");
        GLint hmapAmbientColourLocation = glGetUniformLocation(hmapProg.getId(), "ambientColour");
        GLint hmapKdLocation = glGetUniformLocation(hmapProg.getId(), "Kd");
        GLint hmapKsLocation = glGetUniformLocation(hmapProg.getId(), "Ks");
        GLint hmapNsLocation = glGetUniformLocation(hmapProg.getId(), "Ns");

        GLint bumpmapProgPVMLocation = glGetUniformLocation(bumpmapProg.getId(), "PVM");
        GLint bumpmapProgMLocation = glGetUniformLocation(bumpmapProg.getId(), "M");
        GLint bumpmapProgNormalMatrixLocation = glGetUniformLocation(bumpmapProg.getId(), "normalMatrix");
        GLint bumpmapProgVsEyeLocation = glGetUniformLocation(bumpmapProg.getId(), "vs_eye");
        GLint bumpmapProgLightDirLocation = glGetUniformLocation(bumpmapProg.getId(), "lightDir");
        GLint bumpmapProgLightColourLocation = glGetUniformLocation(bumpmapProg.getId(), "lightColour");
        GLint bumpmapProgAmbientColourLocation = glGetUniformLocation(bumpmapProg.getId(), "ambientColour");
        GLint bumpmapProgKdLocation = glGetUniformLocation(bumpmapProg.getId(), "Kd");
        GLint bumpmapProgKsLocation = glGetUniformLocation(bumpmapProg.getId(), "Ks");
        GLint bumpmapProgNsLocation = glGetUniformLocation(bumpmapProg.getId(), "Ns");
        GLint bumpmapProgHeighfieldLocation = glGetUniformLocation(bumpmapProg.getId(), "heightfield");

        GLint alphatextureProgMLocation = glGetUniformLocation(alphatextureProg.getId(), "M");
        GLint alphatextureProgSamplerLocation = glGetUniformLocation(alphatextureProg.getId(), "sampler");
        
        Texture tex123456;
        tex123456.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
        tex123456.loadDDS("textures/123456.dds");
        Texture texHeightmap;
        texHeightmap.loadBMP("textures/heightmap.bmp");
        Texture bmapHeightfield;
        bmapHeightfield.setWrapS(GL_MIRRORED_REPEAT);
        bmapHeightfield.setWrapT(GL_MIRRORED_REPEAT);
        bmapHeightfield.loadBMP("textures/heightfield.bmp");
        Texture texBino;
        texBino.loadBMP("textures/binoculars.bmp");

        Model tree("models/simpletree.obj");
        ModelInstance tree1(&tree);
        tree1.transform(glm::scale(glm::vec3(0.5f, 1.0f, 0.5f)));
        tree1.transform(glm::translate(glm::vec3(2.0f, 0.0f, -2.0f)));
        Model cube("models/texturedCube.obj");
        ModelInstance cube1(&cube);
        cube1.transform(glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)));
        cube1.transform(glm::translate(glm::vec3(-0.5f, 0.5f, 0.5f)));
        GridMesh gridMesh(128,128);
        Cylinder cylinder;
        CylinderInstance cyl1(&cylinder);
        cyl1.transform(glm::rotate(glm::radians(-90.0f), glm::vec3(1,0,0)));
        cyl1.transform(glm::scale(glm::vec3(0.3f, 4.0f, 0.3f)));
        cyl1.transform(glm::translate(glm::vec3(3.0f, -0.2f, 1.0f)));
        SmokeSystem smoke(10000);

        // alphat mesh
        float alphatVboData[16] = {
            -1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f
        };
        GLuint alphatVao;
        glGenVertexArrays(1, &alphatVao);
        glBindVertexArray(alphatVao);
        GLuint alphatVbo;
        glGenBuffers(1, &alphatVbo);
        glBindBuffer(GL_ARRAY_BUFFER, alphatVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(alphatVboData), (void*)alphatVboData, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);


        Camera cam;
        cam.pos = glm::vec3(0,0,8);
        //cam.pos = glm::vec3(0, 5, 0);
        cam.yaw = glm::radians(180.0f);
        cam.farZ = 100.0f;

        Sun sun;

        bool running = true;
        bool binoMode = false;
        while (running)
        {
            SDL_GetWindowSize(window, &window_w, &window_h);
            SDL_Event e;
            while (SDL_PollEvent(&e))
            {
                if (e.type == SDL_KEYDOWN)
                {
                    switch (e.key.keysym.sym)
                    {
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                    case SDLK_b:
                        binoMode = !binoMode;
                        break;
                    default:
                        break;
                    }
                }
                else if (e.type == SDL_QUIT)
                {
                    running = false;
                }
                else if (e.type == SDL_MOUSEMOTION) {
                    float dx = float(e.motion.xrel);
                    float dy = float(e.motion.yrel);
                    cam.yaw -= dx / float(window_w) * glm::radians(180.0f);
                    cam.pitch += dy / float(window_h) * glm::radians(90.0f);
                }
            }

            {
                int numkeys;
                const Uint8* keystate = SDL_GetKeyboardState(&numkeys);
                assert(numkeys > SDL_SCANCODE_W);
                glm::vec3 forward = cam.getViewDir();
                glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0,1,0)));
                glm::vec3 up = glm::normalize(glm::cross(right, forward));
                if (keystate[SDL_SCANCODE_W]) {
                    cam.pos += 0.03f * forward;
                }
                if (keystate[SDL_SCANCODE_S]) {
                    cam.pos -= 0.03f * forward;
                }
                if (keystate[SDL_SCANCODE_A]) {
                    cam.pos -= 0.03f * right;
                }
                if (keystate[SDL_SCANCODE_D]) {
                    cam.pos += 0.03f * right;
                }
                if (keystate[SDL_SCANCODE_E]) {
                    cam.pos += 0.03f * up;
                }
                if (keystate[SDL_SCANCODE_Q]) {
                    cam.pos -= 0.03f * up;
                }
            }


            SDL_GL_GetDrawableSize(window, &framebuffer_w, &framebuffer_h);
            glViewport(0, 0, framebuffer_w, framebuffer_h);
            float aspectRatio = float(framebuffer_w) / float(framebuffer_h);
            cam.aspect = aspectRatio;
            glDisable(GL_SCISSOR_TEST);
            if (binoMode) {
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                cam.fovy = glm::radians(18.0f);
            }
            else {
                glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
                cam.fovy = glm::radians(50.0f);
            }
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            if (binoMode) {
                glEnable(GL_SCISSOR_TEST);
                glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                GLint scissorX = 0;
                GLint scissorY = int((-0.5f * aspectRatio / 2.0f + 0.5f) * framebuffer_h) + 2;
                GLint scissorW = framebuffer_w;
                GLint scissorH = int(0.5f * aspectRatio * framebuffer_h) - 4;
                glScissor(scissorX, scissorY, scissorW, scissorH);
            }

            glm::vec3 sunDir = sun.getLightDir();

            program.use();
            glActiveTexture(GL_TEXTURE0);
            tex123456.bind();
            glUniform1i(program["sampler"], 0);
            glm::mat4 pvm = cam.getP() * cam.getV() * cube1.getM();
            //glm::mat4 pvm = sun.getP() * sun.getV(cam.pos) * cube1.getM();
            glm::mat3 normMat = glm::mat3(glm::transpose(glm::inverse(cube1.getM())));
            glUniformMatrix4fv(program["PVM"], 1, false, glm::value_ptr(pvm));
            glUniformMatrix4fv(program["M"], 1, false, glm::value_ptr(cube1.getM()));
            glUniformMatrix3fv(program["normalMatrix"], 1, false, glm::value_ptr(normMat));
            glUniform3fv(program["lightDir"], 1, glm::value_ptr(sunDir));
            glUniform3f(program["ambientColour"], 0.5f, 0.5f, 0.5f);
            glUniform3f(program["lightColour"], 0.5f, 0.0f, 0.0f);
            glUniform3fv(program["vs_eye"], 1, glm::value_ptr(cam.pos));

            cube1.setUniformLocations(-1, program["Ks"], program["Ns"]);
            cube1.draw();
            
            program2.use();
            pvm = cam.getP() * cam.getV() * tree1.getM();
            //pvm = sun.getP() * sun.getV(cam.pos) * tree1.getM();
            normMat = glm::mat3(glm::transpose(glm::inverse(tree1.getM())));
            glUniformMatrix4fv(program2["PVM"], 1, false, glm::value_ptr(pvm));
            glUniformMatrix4fv(program2["M"], 1, false, glm::value_ptr(tree1.getM()));
            glUniformMatrix3fv(program2["normalMatrix"], 1, false, glm::value_ptr(normMat));
            glUniform3fv(program2["lightDir"], 1, glm::value_ptr(sunDir));
            glUniform3f(program2["ambientColour"], 0.5f, 0.5f, 0.5f);
            glUniform3f(program2["lightColour"], 1.0f, 1.0f, 1.0f);
            glUniform3fv(program2["vs_eye"], 1, glm::value_ptr(cam.pos));
            tree1.setUniformLocations(program2["Kd"], program2["Ks"], program2["Ns"]);
            tree1.draw();
            
            bumpmapProg.use();
            pvm = cam.getP() * cam.getV() * cyl1.getM();
            normMat = glm::mat3(glm::transpose(glm::inverse(cyl1.getM())));
            glUniformMatrix4fv(bumpmapProg["PVM"], 1, false, glm::value_ptr(pvm));
            glUniformMatrix4fv(bumpmapProg["M"], 1, false, glm::value_ptr(cyl1.getM()));
            glUniformMatrix3fv(bumpmapProg["normalMatrix"], 1, false, glm::value_ptr(normMat));
            glUniform3f(bumpmapProg["Ks"], 0, 0, 0);
            glUniform1f(bumpmapProg["Ns"], 0);
            glUniform3f(bumpmapProg["Kd"], 0.329826f, 0.060918f, 0.008916f);
            glUniform3fv(bumpmapProg["lightDir"], 1, glm::value_ptr(sunDir));
            glUniform3f(bumpmapProg["ambientColour"], 0.5f, 0.5f, 0.5f);
            glUniform3f(bumpmapProg["lightColour"], 1.0f, 1.0f, 1.0f);
            glUniform3fv(bumpmapProg["vs_eye"], 1, glm::value_ptr(cam.pos));
            glActiveTexture(GL_TEXTURE1);
            bmapHeightfield.bind();
            glUniform1i(bumpmapProgHeighfieldLocation, 1);
            cyl1.draw();


            hmapProg.use();
            glActiveTexture(GL_TEXTURE1);
            texHeightmap.bind();
            glUniform1i(hmapSamplerLocation, 1);
            glm::mat4 meshM = glm::rotate(glm::radians(-90.0f), glm::vec3(1,0,0));
            meshM = glm::scale(glm::vec3(40.0f, 4.0f, 40.0f)) * meshM;
            meshM = glm::translate(glm::vec3(-20.0f, -2.5f, 20.f)) * meshM;
            pvm = cam.getP() * cam.getV() * meshM;
            normMat = glm::mat3(glm::transpose(glm::inverse(meshM)));
            glUniformMatrix4fv(hmapProg["PVM"], 1, GL_FALSE, glm::value_ptr(pvm));
            glUniformMatrix4fv(hmapProg["M"], 1, false, glm::value_ptr(meshM));
            glUniformMatrix3fv(hmapProg["normalMatrix"], 1, false, glm::value_ptr(normMat));
            glUniform3fv(hmapProg["lightDir"], 1, glm::value_ptr(sunDir));
            glUniform3f(hmapProg["ambientColour"], 0.0f, 0.0f, 0.0f);
            glUniform3f(hmapProg["lightColour"], 1.0f, 1.0f, 1.0f);
            glUniform3fv(hmapProg["vs_eye"], 1, glm::value_ptr(cam.pos));
            glUniform3f(hmapProg["Ks"], 0, 0, 0);
            glUniform1f(hmapProg["Ns"], 0);
            glUniform3f(hmapProg["Kd"], 0.0f, 0.7f, 0.0f);
            gridMesh.draw();

            smoke.setPV(cam.getP()* cam.getV());
            smoke.tick();
            smoke.draw();

            if (binoMode) {
                glDisable(GL_DEPTH_TEST);
                alphatextureProg.use();
                glBindVertexArray(alphatVao);
                glActiveTexture(GL_TEXTURE0);
                texBino.bind();
                glUniform1i(alphatextureProg["sampler"], 0);
                glm::mat3 binoM(1.0f);
                binoM[0][0] = 1.0f;
                binoM[1][1] = 0.5f * aspectRatio;
                glUniformMatrix3fv(alphatextureProg["M"], 1, GL_FALSE, glm::value_ptr(binoM));
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                glBindVertexArray(0);
                glDisable(GL_BLEND);
            }   

            SDL_GL_SwapWindow(window);
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }


    return 0;
}