#include "Input/KeyboardState.h"

#include "Input/ActionBinding.h"
#include "Input/InputBinding.h"
#include "JsonParser/JsonNode.h"

void KeyboardState::keyCallback(GLFWwindow *window, GLFW_Keycode key, int scanCode, int action, int mods)
{
    handleEvent(key, action);
}

void KeyboardState::handleEvent(GLFW_Keycode key, int action)
{
    auto press = (action == GLFW_PRESS);
    if (key >= GLFW_KEY_LAST || key < 0)
    {
        return;
    }
    keyStates[key].pressed = press;
}

void KeyboardState::loadBinding(const JsonNode &Node, ActionBinding &actionBinding)
{
    auto actionNode = Node.getNode("keyboard").getNode("actions");
    auto keyvalMap  = actionNode.getKeyValueMap();
    for (auto &[tagStr, keysNode] : keyvalMap)
    {
        const auto tag = actionBinding.getActionName(tagStr);
        for (auto &key : keysNode.asVectorOf<std::string>())
        {
            GLFW_Keycode keycode = keyboardMap.at(key);
            addActionBinding(keycode, tag);
        }
    }
    auto axisButtonNode = Node.getNode("keyboard").getNode("axisButton");
    for (auto &mappingNode : axisButtonNode.getVector())
    {
        std::string keyStr;
        mappingNode.get("key", keyStr);

        std::string tagStr;
        mappingNode.get("tag", tagStr);

        float scale{};
        mappingNode.get("scale", scale);

        const auto keycode = keyboardMap.at(keyStr);
        const auto tag     = actionBinding.getActionName(tagStr);

        addAxisBinding(keycode, tag, scale);
    }
}

void KeyboardState::onNewFrame()
{
    for (auto &keyState : keyStates)
    {
        keyState.onNewFrame();
    }
}

void KeyboardState::update(float /*dt*/, ActionBinding &actionBinding)
{
    for (auto &[tag, keys] : keyActionBindings)
    {
        for (const auto &key : keys)
        {
            if (keyStates[key].pressed)
            {
                actionBinding.setActionPressed(tag);
            }
        }
    }

    for (auto &[key, bindings] : axisButtonBindings)
    {
        if (keyStates[key].pressed)
        {
            for (const auto &binding : bindings)
            {
                actionBinding.updateAxisState(binding.tag, binding.scale);
            }
        }
    }
}

void KeyboardState::addActionBinding(GLFW_Keycode key, ActionName tag)
{
    keyActionBindings[tag].push_back(key);
}

void KeyboardState::addAxisBinding(GLFW_Keycode key, ActionName tag, float scale)
{
    axisButtonBindings[key].push_back(ButtonAxisBinding{tag, scale});
}

bool KeyboardState::wasJustPressed(GLFW_Keycode key) const
{
    return keyStates[key].wasJustPressed();
}

bool KeyboardState::wasJustReleased(GLFW_Keycode key) const
{
    return keyStates[key].wasJustReleased();
}

bool KeyboardState::isPressed(GLFW_Keycode key) const
{
    return keyStates[key].isPressed();
}

bool KeyboardState::isHeld(GLFW_Keycode key) const
{
    return keyStates[key].isHeld();
}

void KeyboardState::resetInput()
{
    for (auto &key : keyStates)
    {
        key.reset();
    }
}
