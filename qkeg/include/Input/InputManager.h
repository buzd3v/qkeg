#pragma once

#include "ActionBinding.h"
#include "KeyboardState.h"
#include <GLFW/glfw3.h>
#include <filesystem>

#include "singleton_atomic.hpp"

class InputManager : public SingletonAtomic<InputManager>
{
  public:
    void init(GLFWwindow *window);

    void loadInputBinding(const std::filesystem::path &inputActionsPath, const std::filesystem::path &inputMappingPath);

    void onNewFrame();
    void handleEvent();
    void update(float dt);

    void cleanUp();
    void resetInput();

    const ActionBinding &getActionBinding() const { return actionBinding; }
    ActionBinding       &getActionBinding() { return actionBinding; }
    KeyboardState       &getKeyboard() { return keyboard; }
    const KeyboardState &getKeyboard() const { return keyboard; }

  private:
    static void   keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    ActionBinding actionBinding;
    KeyboardState keyboard;
};
