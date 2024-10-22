#include "Input/KeyboardState.h"

#include "JsonParser/JsonNode.h"
#include "Input/ActionMapping.h"
#include "Input/InputMapping.h"

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

void KeyboardState::loadMapping(const JsonNode &Node, ActionMapping &actionMapping)
{
    for (auto &[tagStr, keysNode] : Node.getNode("action").getKeyValueMap())
    {
        const auto tag = actionMapping.getActionTagHash(tagStr);
        for (auto &key : keysNode.asVectorOf<std::string>())
        {
            GLFW_Keycode keycode = keyboardMap.at(key);
            addActionMapping(keycode, tag);
        }
    }

    for (auto &mappingNode : Node.getNode("axisButton").getVector())
    {
        std::string keyStr;
        mappingNode.get("key", keyStr);

        std::string tagStr;
        mappingNode.get("tag", tagStr);

        float scale{};
        mappingNode.get("scale", scale);

        const auto keycode = keyboardMap.at(keyStr);
        const auto tag     = actionMapping.getActionTagHash(tagStr);

        addAxisMapping(keycode, tag, scale);
    }
}

void KeyboardState::onNewFrame()
{
    for (auto &keyState : keyStates)
    {
        keyState.onNewFrame();
    }
}

void KeyboardState::update(float /*dt*/, ActionMapping &actionMapping)
{
    for (auto &[tag, keys] : keyActionBindings)
    {
        for (const auto &key : keys)
        {
            if (keyStates[key].pressed)
            {
                actionMapping.setActionPressed(tag);
            }
        }
    }

    for (auto &[key, bindings] : axisButtonBindings)
    {
        if (keyStates[key].pressed)
        {
            for (const auto &binding : bindings)
            {
                actionMapping.updateAxisState(binding.tag, binding.scale);
            }
        }
    }
}

void KeyboardState::addActionMapping(GLFW_Keycode key, ActionTagHash tag)
{
    keyActionBindings[tag].push_back(key);
}

void KeyboardState::addAxisMapping(GLFW_Keycode key, ActionTagHash tag, float scale)
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
