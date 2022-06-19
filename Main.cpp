#include <cassert>
#include <iostream>
#include <vector>
#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <glm/ext.hpp>

#include "Util.h"
#include "ShaderProgram.h"

int main(int ArgCount, char** Args)
{
    if (!SDL_WasInit(SDL_INIT_EVERYTHING)) {
        SDL_Init(SDL_INIT_EVERYTHING);
    }
    int w = 512;
    int h = 512;
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
    SDL_Window* window = SDL_CreateWindow("SDL OpenGL Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, WindowFlags);
    assert(window);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    int glewErr = glewInit();
    if (glewErr != GLEW_OK) {
        std::cerr << glewGetErrorString(glewErr) << std::endl;
        return 1;
    }
    if (glewIsSupported("GL_VERSION_4_6")) {
        std::cout << "opengl version 4.6 supported" << std::endl;
    }

    try {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(loggingCallback, nullptr);

        ShaderProgram program("shaders/vert.vs", "shaders/frag.fs");

        GLint uColourLocation = glGetUniformLocation(program.getId(), "u_colour");
        GLint PVMLocation = glGetUniformLocation(program.getId(), "PVM");

        float verts[9]{
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f, 0.5f, 0.0f
        };
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), (void*)verts, GL_STATIC_DRAW);

        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        bool running = true;
        while (running)
        {
            SDL_Event Event;
            while (SDL_PollEvent(&Event))
            {
                if (Event.type == SDL_KEYDOWN)
                {
                    switch (Event.key.keysym.sym)
                    {
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                    default:
                        break;
                    }
                }
                else if (Event.type == SDL_QUIT)
                {
                    running = false;
                }
            }
            SDL_GL_GetDrawableSize(window, &w, &h);
            glViewport(0, 0, w, h);
            glClearColor(0, 0, 0, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            program.use();
            glBindVertexArray(vao);
            glUniform3f(uColourLocation, 1.0f, 0.0f, 1.0f);
            float displacement = SDL_GetTicks() % 2000;
            displacement = displacement / 1000.0f - 1.0f;
            float PVM[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, displacement,0,0,1 };
            glUniformMatrix4fv(PVMLocation, 1, false, PVM);
            glDrawArrays(GL_TRIANGLES, 0, 3);

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