#include "Input/InputManager.h"
#include "JsonParser/JsonFile.h"

void InputManager::init(GLFWwindow *window)
{
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);
}

void InputManager::loadMapping(const std::filesystem::path &inputActionsPath,
                               const std::filesystem::path &inputMappingPath)
{
    actionMapping.loadActions(inputActionsPath);

    JsonFile file(inputMappingPath);
    if (!file.isGood())
    {
        throw(std::runtime_error(fmt::format("Failed to open file: {}", inputMappingPath.string())));
    }
    const auto rootNode = file.getRootNode();

    keyboard.loadMapping(rootNode, actionMapping);
}

void InputManager::onNewFrame()
{
    actionMapping.onNewFrame();
    keyboard.onNewFrame();
}

void InputManager::handleEvent() {}

void InputManager::update(float dt)
{
    keyboard.update(dt, actionMapping);

    actionMapping.update(dt);
}

void InputManager::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    InputManager *instance = static_cast<InputManager *>(glfwGetWindowUserPointer(window));
    if (instance)
    {
        instance->keyboard.handleEvent(key, action);
    }
}

void InputManager::resetInput()
{
    actionMapping.resetState();
    keyboard.resetInput();
}

void InputManager::cleanUp() {}
