#include <cassert>
#include <iostream>
#include <vector>
#include <sstream>
#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>

#include "Util.h"
#include "ShaderProgram.h"
#include "Scene.h"
#include "UI.h"
#include "Sound.h"

void addFocusDistanceMessage(UI &ui, float focusDistance) {
    std::ostringstream msg;
    msg << "focus distance: " << focusDistance << "m";
    ui.addMessage(2.0f, "focusDistance", msg.str());
}

void addTimeRateMessage(UI& ui, float rate) {
    std::ostringstream msg;
    msg << "time multiplier: " << rate << "m";
    ui.addMessage(2.0f, "timeRate", msg.str());
}


int main(int argc, char** argv)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0) {
        std::cerr << "Unable to initialize SDL" << std::endl;
        return 1;
    }
    int framebufferW = 960;
    int framebufferH = 540;
    int windowW = 960;
    int windowH = 540;
    unsigned WindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetSwapInterval(1);
    SDL_Window* window = SDL_CreateWindow("Orson's Scavenger Hunt - CS488 Project", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowW, windowH, WindowFlags);
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

        AudioDevice audioDevice;
        Scene scene(audioDevice);
        UI ui(&scene, audioDevice);

        bool running = true;
        std::string fpsMsg = "FPS: ????";
        while (running)
        {
            u64 currTimestamp = SDL_GetPerformanceCounter();
            float frameTime = (currTimestamp - prevTimestamp) / float(SDL_GetPerformanceFrequency());
            float fps = 1.0f / frameTime;
            prevTimestamp = currTimestamp;
            ui.setFps(fps);
            SDL_GetWindowSize(window, &windowW, &windowH);
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_KEYDOWN) {
                    switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                    case SDLK_b:
                        scene.toggleBinoMode();
                        break;
                    case SDLK_f:
                        scene.getFlashlight().toggle();
                        break;
                    case SDLK_g:
                        ui.toggleShowHint();
                        break;
                    case SDLK_u:
                        scene.getSun().multiplyRate(0.5f);
                        addTimeRateMessage(ui, scene.getSun().getRate());
                        break;
                    case SDLK_p:
                        scene.getSun().multiplyRate(2.0f);
                        addTimeRateMessage(ui, scene.getSun().getRate());
                        break;
                    case SDLK_i:
                        scene.getSun().changeTime(-1.0f);
                        addTimeRateMessage(ui, scene.getSun().getRate());
                        break;
                    case SDLK_o:
                        scene.getSun().changeTime(1.0f);
                        addTimeRateMessage(ui, scene.getSun().getRate());
                        break;
                    case SDLK_h:
                        scene.changeFocusDistance(-1.0f);
                        addFocusDistanceMessage(ui, scene.getFocusDistance());
                        break;
                    case SDLK_j:
                        scene.changeFocusDistance(-0.1f);
                        addFocusDistanceMessage(ui, scene.getFocusDistance());
                        break;
                    case SDLK_k:
                        scene.changeFocusDistance(0.1f);
                        addFocusDistanceMessage(ui, scene.getFocusDistance());
                        break;
                    case SDLK_l:
                        scene.changeFocusDistance(1.0f);
                        addFocusDistanceMessage(ui, scene.getFocusDistance());
                        break;
                    case SDLK_LEFT:
                        if (ui.getShowHint()) {
                            ui.changeHint(false);
                        }
                        break;
                    case SDLK_RIGHT:
                        if (ui.getShowHint()) {
                            ui.changeHint(true);
                        }
                        break;
                    case SDLK_1:
                        scene.toggleReflection();
                        break;
                    case SDLK_2:
                        scene.toggleShadow();
                        break;
                    case SDLK_3:
                        scene.toggleBumpmap();
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
                    scene.getCamera().m_yaw -= dx / float(windowW) * glm::radians(180.0f);
                    scene.getCamera().m_pitch -= dy / float(windowH) * glm::radians(90.0f);
                }
                else if (e.type == SDL_MOUSEBUTTONUP) {
                    if (e.button.button == SDL_BUTTON_LEFT) {
                        ui.pickTarget();
                        ui.setShowCrosshiars(false);
                    }
                }
                else if (e.type == SDL_MOUSEBUTTONDOWN) {
                    if (e.button.button == SDL_BUTTON_LEFT) {
                        ui.setShowCrosshiars(true);
                    }
                }
            }

            glm::vec3 camDelta(0.0);

            {
                int numkeys;
                const Uint8* keystate = SDL_GetKeyboardState(&numkeys);
                assert(numkeys > SDL_SCANCODE_W);
                glm::mat3 dirs(scene.getCamera().getVInv());
                glm::vec3 forward = -dirs[2];
                glm::vec3 right = dirs[0];
                glm::vec3 up = dirs[1];
                if (keystate[SDL_SCANCODE_W]) {
                    camDelta += 4.5f * frameTime * forward;
                }
                if (keystate[SDL_SCANCODE_S]) {
                    camDelta -= 4.5f * frameTime * forward;
                }
                if (keystate[SDL_SCANCODE_A]) {
                    camDelta -= 4.5f * frameTime * right;
                }
                if (keystate[SDL_SCANCODE_D]) {
                    camDelta += 4.5f * frameTime * right;
                }
                if (keystate[SDL_SCANCODE_E]) {
                    camDelta += 4.5f * frameTime * up;
                }
                if (keystate[SDL_SCANCODE_Q]) {
                    camDelta -= 4.5f * frameTime * up;
                }
                if (!ui.getShowHint()) {
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
            }
            scene.moveCamera(camDelta);

            SDL_GL_GetDrawableSize(window, &framebufferW, &framebufferH);
            glViewport(0, 0, framebufferW, framebufferH);
            float aspectRatio = float(framebufferW) / float(framebufferH);
            scene.getCamera().m_aspect = aspectRatio;
            scene.setFramebufferSize(framebufferW, framebufferH);
            scene.tick(frameTime);
            ui.tick(frameTime);
            ui.setFbSize(framebufferW, framebufferH);

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
