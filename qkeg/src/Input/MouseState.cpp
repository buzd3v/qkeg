#include "Input/MouseState.h"

#include "Input/ActionBinding.h"
#include "Input/InputBinding.h"
#include "JsonParser/JsonNode.h"

void MouseState::onNewFrame()
{
    prevPos = currentPos;
}

void MouseState::resetInput() {}

bool MouseState::wasJustReleased(GLFW_MouseBtn key) const
{
    return true;
}

bool MouseState::isHeld(GLFW_MouseBtn key) const
{
    return true;
}

bool MouseState::wasJustPressed(GLFW_MouseBtn key) const
{
    return true;
}

void MouseState::addActionBinding(GLFW_MouseBtn key, ActionName tag) {}

void MouseState::handleEvent(GLFW_MouseBtn btn, int action)
{
    switch (btn)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
        break;
    }
}

void MouseState::loadBinding(const JsonNode &Node, ActionBinding &actionBinding) {}

void MouseState::mouseMouseCallback(GLFWwindow *window, double xPos, double yPos) {}

void MouseState::mouseButtonCallBack(GLFWwindow *window, int button, int actions, int mods) {}