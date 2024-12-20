#pragma once

#include <array>
#include <unordered_map>

#include "ActionName.h"
#include "ButtonState.h"
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
using GLFW_MouseBtn = int;

class ActionBinding;
class JsonNode;

class MouseState
{
  public:
    void loadBinding(const JsonNode &Node, ActionBinding &actionBinding);

    void onNewFrame();
    void handleEvent(GLFW_MouseBtn btn, int action);

    void addActionBinding(GLFW_MouseBtn key, ActionName tag);

    bool wasJustPressed(GLFW_MouseBtn key) const;
    bool wasJustReleased(GLFW_MouseBtn key) const;
    bool isHeld(GLFW_MouseBtn key) const;

    void resetInput();

  private:
    void mouseMouseCallback(GLFWwindow *window, double xPos, double yPos);
    void mouseButtonCallBack(GLFWwindow *window, int button, int actions, int mods);

    glm::vec2 currentPos;
    glm::vec2 prevPos;

    // bindings
    std::unordered_map<GLFW_MouseBtn, ActionName> keyActionBindings;
    std::array<ButtonState, 16>                   mouseBtnStates;
};
