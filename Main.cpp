#include <cassert>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>

#include "Util.h"
#include "ShaderProgram.h"
#include "Scene.h"
#include "UI.h"


int main(int ArgCount, char** Args)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "Unable to initialize SDL" << std::endl;
        return 1;
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
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetSwapInterval(1);
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
        u64 prevTimestamp = SDL_GetPerformanceCounter();
        u64 prevFPSTimestamp = 0;
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(loggingCallback, nullptr);

        Scene scene;
        UI ui(&scene);

        bool running = true;
        std::string fpsMsg = "FPS: ????";
        while (running)
        {
            u64 currTimestamp = SDL_GetPerformanceCounter();
            float frameTime = (currTimestamp - prevTimestamp) / float(SDL_GetPerformanceFrequency());
            float fps = 1.0f / frameTime;
            prevTimestamp = currTimestamp;
            // only update the fps counter every second so it doesn't flicker
            if (currTimestamp - prevFPSTimestamp > SDL_GetPerformanceFrequency()) {
                prevFPSTimestamp = currTimestamp;
                std::ostringstream msgstream;
                msgstream << std::fixed << std::setprecision(1) << "FPS: " << fps;
                fpsMsg = msgstream.str();
            }
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
                        scene.m_binoMode = !scene.m_binoMode;
                        break;
                    case SDLK_f:
                        scene.getFlashlight().toggle();
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
                    scene.m_cam.m_yaw -= dx / float(window_w) * glm::radians(180.0f);
                    scene.m_cam.m_pitch -= dy / float(window_h) * glm::radians(90.0f);
                }
            }

            {
                int numkeys;
                const Uint8* keystate = SDL_GetKeyboardState(&numkeys);
                assert(numkeys > SDL_SCANCODE_W);
                glm::mat3 dirs(scene.m_cam.getVInv());
                glm::vec3 forward = -dirs[2];
                glm::vec3 right = dirs[0];
                glm::vec3 up = dirs[1];
                if (keystate[SDL_SCANCODE_W]) {
                    scene.m_cam.m_pos += 0.03f * forward;
                }
                if (keystate[SDL_SCANCODE_S]) {
                    scene.m_cam.m_pos -= 0.03f * forward;
                }
                if (keystate[SDL_SCANCODE_A]) {
                    scene.m_cam.m_pos -= 0.03f * right;
                }
                if (keystate[SDL_SCANCODE_D]) {
                    scene.m_cam.m_pos += 0.03f * right;
                }
                if (keystate[SDL_SCANCODE_E]) {
                    scene.m_cam.m_pos += 0.03f * up;
                }
                if (keystate[SDL_SCANCODE_Q]) {
                    scene.m_cam.m_pos -= 0.03f * up;
                }
                if (keystate[SDL_SCANCODE_RIGHT]) {
                    scene.getFlashlight().changeAngle(-0.01f, 0.0f);
                }
                if (keystate[SDL_SCANCODE_LEFT]) {
                    scene.getFlashlight().changeAngle(0.01f, 0.0f);
                }
                if (keystate[SDL_SCANCODE_UP]) {
                    scene.getFlashlight().changeAngle(0.0f, 0.01f);
                }
                if (keystate[SDL_SCANCODE_DOWN]) {
                    scene.getFlashlight().changeAngle(0.0f, -0.01f);
                }
            }


            SDL_GL_GetDrawableSize(window, &framebuffer_w, &framebuffer_h);
            glViewport(0, 0, framebuffer_w, framebuffer_h);
            float aspectRatio = float(framebuffer_w) / float(framebuffer_h);
            scene.m_cam.m_aspect = aspectRatio;
            scene.m_defaultFboW = framebuffer_w;
            scene.m_defaultFboH = framebuffer_h;
            ui.setFbSize(framebuffer_w, framebuffer_h);
            
            ui.addText(fpsMsg, -1.0f, 1.0f - 32.0f / framebuffer_h, 1.0f);
            
            scene.render();
            ui.draw();

            SDL_GL_SwapWindow(window);
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    SDL_Quit();

    return 0;
}