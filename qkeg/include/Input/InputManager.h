#pragma once

#include "ActionMapping.h"
#include "KeyboardState.h"
#include <GLFW/glfw3.h>
#include <filesystem>

class InputManager
{
  public:
    void init(GLFWwindow *window);

    void loadMapping(const std::filesystem::path &inputActionsPath, const std::filesystem::path &inputMappingPath);

    void onNewFrame();
    void handleEvent();
    void update(float dt);

    void cleanUp();
    void resetInput();

    const ActionMapping &getActionMapping() const { return actionMapping; }
    ActionMapping       &getActionMapping() { return actionMapping; }
    KeyboardState       &getKeyboard() { return keyboard; }
    const KeyboardState &getKeyboard() const { return keyboard; }

  private:
    static void   keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    ActionMapping actionMapping;
    KeyboardState keyboard;
};
