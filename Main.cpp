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

int main(int ArgCount, char** Args)
{
    if (!SDL_WasInit(SDL_INIT_EVERYTHING)) {
        SDL_Init(SDL_INIT_EVERYTHING);
    }
    int framebuffer_w = 512;
    int framebuffer_h = 512;
    int window_w = 512;
    int window_h = 512;
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


        GLint prog1PVMLocation = glGetUniformLocation(program.getId(), "PVM");
        GLint prog1MLocation = glGetUniformLocation(program.getId(), "M");
        GLint prog1NormalMatrixLocation = glGetUniformLocation(program.getId(), "normalMatrix");
        GLint prog1VsEyeLocation = glGetUniformLocation(program.getId(), "vs_eye");
        GLint prog1SamplerLocation = glGetUniformLocation(program.getId(), "sampler");
        GLint prog1LightDirLocation = glGetUniformLocation(program.getId(), "lightDir");
        GLint prog1LightColourLocation = glGetUniformLocation(program.getId(), "lightColour");
        GLint prog1AmbientColourLocation = glGetUniformLocation(program.getId(), "ambientColour");
        GLint prog1KsLocation = glGetUniformLocation(program.getId(), "Ks");
        GLint prog1NsLocation = glGetUniformLocation(program.getId(), "Ns");

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

        /*uniform sampler2D sampler;
uniform mat4 PVM;
uniform mat4 M;

uniform mat3 normalMatrix;
uniform vec3 vs_eye;
uniform vec3 lightDir;
uniform vec3 lightColour;
uniform vec3 ambientColour;

uniform vec3 Kd;
uniform vec3 Ks;
uniform float Ns;
*/
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

        Texture texture;
        texture.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
        texture.loadDDS("textures/123456.dds");
        Texture texture2;
        texture2.loadBMP("textures/heightmap.bmp");
        Texture bmapHeightfield;
        bmapHeightfield.setWrapS(GL_MIRRORED_REPEAT);
        bmapHeightfield.setWrapT(GL_MIRRORED_REPEAT);
        bmapHeightfield.loadBMP("textures/heightfield.bmp");

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
        cyl1.transform(glm::translate(glm::vec3(3.0f, 0.0f, 1.0f)));

        Camera cam;
        cam.pos = glm::vec3(0,0,8);
        cam.yaw = glm::radians(180.0f);
        cam.farZ = 100.0f;

        bool running = true;
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
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            program.use();
            glActiveTexture(GL_TEXTURE0);
            texture.bind();
            glUniform1i(prog1SamplerLocation, 0);

            glm::mat4 pvm = cam.getP() * cam.getV() * cube1.getM();
            //glm::mat3 m3(1.0f);
            glm::mat3 normMat = glm::mat3(glm::transpose(glm::inverse(cube1.getM())));
            glUniformMatrix4fv(prog1PVMLocation, 1, false, glm::value_ptr(pvm));
            glUniformMatrix4fv(prog1MLocation, 1, false, glm::value_ptr(cube1.getM()));
            glUniformMatrix3fv(prog1NormalMatrixLocation, 1, false, glm::value_ptr(normMat));
            glUniform3f(prog1LightDirLocation, 1, 0, 0);
            glUniform3f(prog1AmbientColourLocation, 0.5f, 0.5f, 0.5f);
            glUniform3f(prog1LightColourLocation, 0.5f, 0.0f, 0.0f);
            glUniform3fv(prog1VsEyeLocation, 1, glm::value_ptr(cam.pos));

            cube1.setUniformLocations(-1, prog1KsLocation, prog1NsLocation);
            cube1.draw();
            
            program2.use();
            pvm = cam.getP() * cam.getV() * tree1.getM();
            normMat = glm::mat3(glm::transpose(glm::inverse(tree1.getM())));
            glUniformMatrix4fv(prog2PVMLocation, 1, false, glm::value_ptr(pvm));
            glUniformMatrix4fv(prog2MLocation, 1, false, glm::value_ptr(tree1.getM()));
            glUniformMatrix3fv(prog2NormalMatrixLocation, 1, false, glm::value_ptr(normMat));
            glUniform3f(prog2LightDirLocation, 1, 0, 0);
            glUniform3f(prog2AmbientColourLocation, 0.5f, 0.5f, 0.5f);
            glUniform3f(prog2LightColourLocation, 1.0f, 1.0f, 1.0f);
            glUniform3fv(prog2VsEyeLocation, 1, glm::value_ptr(cam.pos));
            tree1.setUniformLocations(prog2KdLocation, prog2KsLocation,prog2NsLocation );
            tree1.draw();

            bumpmapProg.use();
            pvm = cam.getP() * cam.getV() * cyl1.getM();
            normMat = glm::mat3(glm::transpose(glm::inverse(cyl1.getM())));
            glUniformMatrix4fv(bumpmapProgPVMLocation, 1, false, glm::value_ptr(pvm));
            glUniformMatrix4fv(bumpmapProgMLocation, 1, false, glm::value_ptr(cyl1.getM()));
            glUniformMatrix3fv(bumpmapProgNormalMatrixLocation, 1, false, glm::value_ptr(normMat));
            glUniform3f(bumpmapProgKsLocation, 0, 0, 0);
            glUniform1f(bumpmapProgNsLocation, 0);
            glUniform3f(bumpmapProgKdLocation, 0.329826f, 0.060918f, 0.008916f);
            glUniform3f(bumpmapProgLightDirLocation, 1, 0, 0);
            glUniform3f(bumpmapProgAmbientColourLocation, 0.5f, 0.5f, 0.5f);
            glUniform3f(bumpmapProgLightColourLocation, 1.0f, 1.0f, 1.0f);
            glUniform3fv(bumpmapProgVsEyeLocation, 1, glm::value_ptr(cam.pos));
            glActiveTexture(GL_TEXTURE1);
            bmapHeightfield.bind();
            glUniform1i(bumpmapProgHeighfieldLocation, 1);
            cyl1.draw();


            hmapProg.use();
            glActiveTexture(GL_TEXTURE1);
            texture2.bind();
            glUniform1i(hmapSamplerLocation, 1);
            glm::mat4 meshM = glm::rotate(glm::radians(-90.0f), glm::vec3(1,0,0));
            meshM = glm::scale(glm::vec3(10.0f, 1.0f, 10.0f)) * meshM;
            pvm = cam.getP() * cam.getV() * meshM;
            normMat = glm::mat3(glm::transpose(glm::inverse(meshM)));
            glUniformMatrix4fv(hmapPVMLocation, 1, GL_FALSE, glm::value_ptr(pvm));
            glUniformMatrix4fv(hmapMLocation, 1, false, glm::value_ptr(meshM));
            glUniformMatrix3fv(hmapNormalMatrixLocation, 1, false, glm::value_ptr(normMat));
            glUniform3f(hmapLightDirLocation, 0.9f, 1.0, 0);
            glUniform3f(hmapAmbientColourLocation, 0.0f, 0.0f, 0.0f);
            glUniform3f(hmapLightColourLocation, 1.0f, 1.0f, 1.0f);
            glUniform3fv(hmapVsEyeLocation, 1, glm::value_ptr(cam.pos));
            glUniform3f(hmapKsLocation, 0, 0, 0);
            glUniform1f(hmapNsLocation, 0);
            glUniform3f(hmapKdLocation, 0.0f, 0.7f, 0.0f);
            gridMesh.draw();

            SDL_GL_SwapWindow(window);
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }


    return 0;
}