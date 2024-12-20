#pragma once

#include <array>
#include <unordered_map>

#include "ActionName.h"
#include "ButtonState.h"
#include <glfw/glfw3.h>

using GLFW_Keycode = int;

class ActionBinding;
class JsonNode;

class KeyboardState
{
  public:
    void loadBinding(const JsonNode &Node, ActionBinding &actionBinding);

    void onNewFrame();
    void handleEvent(GLFW_Keycode keycode, int action);
    void update(float dt, ActionBinding &actionBinding);

    void addActionBinding(GLFW_Keycode key, ActionName tag);
    void addAxisBinding(GLFW_Keycode key, ActionName tag, float scale);

    bool wasJustPressed(GLFW_Keycode key) const;
    bool wasJustReleased(GLFW_Keycode key) const;
    bool isPressed(GLFW_Keycode key) const;
    bool isHeld(GLFW_Keycode key) const;

    void resetInput();

  private:
    void keyCallback(GLFWwindow *window, GLFW_Keycode key, int scanCode, int action, int mods);

    struct ButtonAxisBinding
    {
        ActionName tag;
        float      scale;
    };

    // state
    std::array<ButtonState, GLFW_KEY_LAST + 1> keyStates;

    // bindings
    std::unordered_map<ActionName, std::vector<GLFW_Keycode>>        keyActionBindings;
    std::unordered_map<GLFW_Keycode, std::vector<ButtonAxisBinding>> axisButtonBindings;
};
