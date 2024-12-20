#include "Input/InputManager.h"
#include "JsonParser/JsonFile.h"

void InputManager::init(GLFWwindow *window)
{
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);
}

void InputManager::loadInputBinding(const std::filesystem::path &actionPath, const std::filesystem::path &mappingPath)
{
    actionBinding.loadActions(actionPath);

    JsonFile file(mappingPath);
    if (!file.isGood())
    {
        throw(std::runtime_error(fmt::format("Failed to open file: {}", mappingPath.string())));
    }
    const auto rootNode = file.getRootNode();

    keyboard.loadBinding(rootNode, actionBinding);
}

void InputManager::onNewFrame()
{
    actionBinding.onNewFrame();
    keyboard.onNewFrame();
}

void InputManager::handleEvent() {}

void InputManager::update(float dt)
{
    keyboard.update(dt, actionBinding);

    actionBinding.update(dt);
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
    actionBinding.resetState();
    keyboard.resetInput();
}

void InputManager::cleanUp() {}
