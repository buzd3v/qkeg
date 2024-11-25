#include "Game.h"
#include "JsonParser/JsonFile.h"
#include <utility>
Game::Game() : Application() {}

void Game::loadAppSetting()
{
    const std::filesystem::path settingsPath{"assets/settings/app_settings.json"};
    JsonFile                    file(settingsPath);

    if (!file.isGood())
    {
        throw(std::runtime_error(fmt::format("Cannot open file: {}", settingsPath.string())));
        return;
    }
    const JsonNode   rootNode = file.getRootNode();
    std::vector<int> a;
    params.renderSize = {800, 600};
    rootNode.getIfExists("renderResolution", params.renderSize);
    rootNode.getIfExists("appName", params.appName);
}

void Game::customInit()
{
    auto pool       = ImagePool::GetInstance();
    auto createInfo = GPUImageCreateInfo{
        .format     = VK_FORMAT_R16G16B16A16_SFLOAT,
        .usageFlags = qConstant::defaultDrawImageUsage,
        .extent =
            {
                .width  = (uint32_t)params.renderSize.x,
                .height = (uint32_t)params.renderSize.y,
                .depth  = 1,
            },
    };
    gameImage = pool->createImage(createInfo);
    // inputManager.loadMapping("assets/settings/input_actions.json", "assets/settings/input/_mapping.json");
    gradPipeline.init(gpuDevice);
}

void Game::customUpdate(float dt) {}

void Game::customDraw()
{
    std::filesystem::path path("assets/img.png");

    auto cmd       = gpuDevice.beginFrame();
    auto drawImage = gpuDevice.queryImage(gameImage);

    VkUtil::transitionImage(cmd, drawImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    gradPipeline.draw(cmd, gpuDevice, drawImage);

    GPUDevice::FrameProperties props = {
        .copyImageToSwapchain = true,
        .linearSampler        = true,
    };
    gpuDevice.endFrame(cmd, drawImage, props);
}

void Game::customCleanup()
{
    gradPipeline.cleanUp(gpuDevice);
}
