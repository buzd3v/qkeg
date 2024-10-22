#include "Application.h"

#include <chrono>
#include <iostream>

void glfwErrorCallback(int error, const char *description)
{
    std::cerr << "GLFW Error: " << description << std::endl;
}

void Application::init(const Params &ps)
{
    params = ps;

    // load params for window init
    loadAppSetting();

    if (params.windowSize == glm::ivec2{})
    {
        assert(params.renderSize != glm::ivec2{});
        params.windowSize = params.renderSize;
    }

    if (params.renderSize == glm::ivec2{})
    {
        assert(params.windowSize != glm::ivec2{});
        params.renderSize = params.windowSize;
    }

    if (params.windowTitle.empty())
    {
        params.windowTitle = params.appName;
    }

    // Initialize GLFW
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(params.windowSize.x, params.windowSize.y, params.appName.c_str(), nullptr, nullptr);

    if (!window)
    {
        std::cerr << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return;
    }

    gpuDevice.init(window, params.appName.c_str(), Version(1, 1, 1));
    inputManager.init(window);
}
void Application::run()
{
    const float FPS = 60.f;
    const float dt  = 1 / FPS;

    auto  prevTime    = std::chrono::high_resolution_clock::now();
    float accumulator = dt; // so that we get at least 1 update before render

    isRunning = true;
    while (isRunning && !glfwWindowShouldClose(window))
    {
        const auto newTime = std::chrono::high_resolution_clock::now();
        frameTime          = std::chrono::duration<float>(newTime - prevTime).count();

        if (frameTime > 0.07f && frameTime < 5.f)
        { // if >=5.f - debugging?
            printf("frame drop, time: %.4f\n", frameTime);
        }

        accumulator += frameTime;
        prevTime = newTime;

        // moving average
        float newFPS = 1.f / frameTime;
        if (newFPS == std::numeric_limits<float>::infinity())
        {
            // can happen when frameTime == 0
            newFPS = 0;
        }
        avgFPS = std::lerp(avgFPS, newFPS, 0.1f);

        if (accumulator > 10 * dt)
        {
            // game stopped for debug
            accumulator = dt;
        }
        // while (accumulator >= dt)
        // {
        //     {
        //         // manage glfw event polling
        //         glfwPollEvents();
        //     }
        // }
        {
            inputManager.onNewFrame(); // reset input state on new frame
            glfwPollEvents();          // manage glfw window polling
        }
    }
}

void Application::cleanup() {}
