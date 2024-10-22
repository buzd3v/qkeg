#pragma once

#include <filesystem>
#include <memory>
#include <string>

#include <GLFW/glfw3.h>
#include <fmt/format.h>
#include <glm/glm.hpp>

#include "Graphics/GPUDevice.h"
#include "Input/InputManager.h"

class Application
{
  public:
    struct Params
    {
        glm::ivec2 windowSize{};
        glm::ivec2 renderSize{};

        std::string appName{"Window"};
        std::string windowTitle;
    };

  public:
    void init(const Params &ps);
    void run();
    void cleanup();

    virtual void loadAppSetting()       = 0;
    virtual void customInit()           = 0;
    virtual void customUpdate(float dt) = 0;
    virtual void customDraw()           = 0;
    virtual void customCleanup()        = 0;

  public:
    bool isRunning{false};
    bool gamePaused{false};

    bool prodMode{false};
    bool isDevEnvironment{false};

    bool  frameLimit{true};
    float frameTime{0.f};
    float avgFPS{0.f};

    GLFWwindow *window;

  protected:
    InputManager inputManager;
    GPUDevice    gpuDevice;
    Params       params{};
};
