#pragma once

#include <array>
#include <unordered_map>

#include "ActionTagHash.h"
#include "ButtonState.h"
#include <glfw/glfw3.h>

using GLFW_Keycode = int;

class ActionMapping;
class JsonNode;

class KeyboardState
{
  public:
    void loadMapping(const JsonNode &Node, ActionMapping &actionMapping);

    void onNewFrame();
    void handleEvent(GLFW_Keycode keycode, int action);
    void update(float dt, ActionMapping &actionMapping);

    void addActionMapping(GLFW_Keycode key, ActionTagHash tag);
    void addAxisMapping(GLFW_Keycode key, ActionTagHash tag, float scale);

    bool wasJustPressed(GLFW_Keycode key) const;
    bool wasJustReleased(GLFW_Keycode key) const;
    bool isPressed(GLFW_Keycode key) const;
    bool isHeld(GLFW_Keycode key) const;

    void resetInput();

  private:
    void keyCallback(GLFWwindow *window, GLFW_Keycode key, int scanCode, int action, int mods);

    struct ButtonAxisBinding
    {
        ActionTagHash tag;
        float         scale;
    };

    // state
    std::array<ButtonState, GLFW_KEY_LAST + 1> keyStates;

    // bindings
    std::unordered_map<ActionTagHash, std::vector<GLFW_Keycode>>     keyActionBindings;
    std::unordered_map<GLFW_Keycode, std::vector<ButtonAxisBinding>> axisButtonBindings;
};
