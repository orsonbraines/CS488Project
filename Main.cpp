#include <cassert>
#include <iostream>
#include <vector>
#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <glm/ext.hpp>

#include "Util.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Camera.h"

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

        GLint samplerLocation = glGetUniformLocation(program.getId(), "sampler");
        GLint PVMLocation = glGetUniformLocation(program.getId(), "PVM");

        float verts[20]{
            // x,y,z,s,t
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
        };
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), (void*)verts, GL_STATIC_DRAW);

        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        Texture texture("textures/ht.dds");

        Camera cam;
        cam.pos = glm::vec3(0,0,3);
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
                    float dx = e.motion.xrel;
                    float dy = e.motion.yrel;
                    cam.yaw -= dx / float(window_w) * glm::radians(180.0f);
                    cam.pitch += dy / float(window_h) * glm::radians(90.0f);
                }
            }

            {
                int numkeys;
                const Uint8* keystate = SDL_GetKeyboardState(&numkeys);
                assert(numkeys > SDL_SCANCODE_W && numkeys > SDL_SCANCODE_A && numkeys > SDL_SCANCODE_S && numkeys > SDL_SCANCODE_D);
                glm::vec3 forward = cam.getViewDir();
                glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0,1,0)));
                if (keystate[SDL_SCANCODE_W]) {
                    cam.pos += 0.1f * forward;
                }
                if (keystate[SDL_SCANCODE_S]) {
                    cam.pos -= 0.1f * forward;
                }
                if (keystate[SDL_SCANCODE_A]) {
                    cam.pos -= 0.01f * right;
                }
                if (keystate[SDL_SCANCODE_D]) {
                    cam.pos += 0.01f * right;
                }
            }


            SDL_GL_GetDrawableSize(window, &framebuffer_w, &framebuffer_h);
            glViewport(0, 0, framebuffer_w, framebuffer_h);
            glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            program.use();
            glBindVertexArray(vao);
            glActiveTexture(GL_TEXTURE0);
            texture.bind();
            glUniform1i(samplerLocation, 0);
            
            glm::mat4 pvm = cam.getP() * cam.getV();
            //std::cout << pvm << std::endl;
            //pvm = glm::mat4(1.0);

            glUniformMatrix4fv(PVMLocation, 1, false, value_ptr(pvm));
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            //PVM[0] = 0.5f; PVM[5] = 0.5f; PVM[12] = -0.5f; PVM[13] = -0.5f;
            //glUniformMatrix4fv(PVMLocation, 1, false, PVM);
            //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            SDL_GL_SwapWindow(window);
        }

        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }


    return 0;
}