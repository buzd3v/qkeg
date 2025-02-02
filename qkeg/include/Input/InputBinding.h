#pragma once

#include "glfw/glfw3.h"
#include <unordered_map>

using GLFW_Keycode = int;

template <typename T>
class StringMap
{
  public:
    void addBinding(const T &value, const std::string &str)
    {
        strValueMap[value] = str;
        valueStrMap[str]   = value;
    }

    const std::string &at(const T &value) const { return strValueMap.at(value); }
    const T           &at(const std::string &str) const { return valueStrMap.at(str); }

  private:
    std::unordered_map<T, std::string> strValueMap{};
    std::unordered_map<std::string, T> valueStrMap{};
};

StringMap<GLFW_Keycode> initGLFWKeyBinding()
{
    StringMap<GLFW_Keycode> map;

    // number
    map.addBinding(GLFW_KEY_A, "A");
    map.addBinding(GLFW_KEY_B, "B");
    map.addBinding(GLFW_KEY_C, "C");
    map.addBinding(GLFW_KEY_D, "D");
    map.addBinding(GLFW_KEY_E, "E");
    map.addBinding(GLFW_KEY_F, "F");
    map.addBinding(GLFW_KEY_G, "G");
    map.addBinding(GLFW_KEY_H, "H");
    map.addBinding(GLFW_KEY_I, "I");
    map.addBinding(GLFW_KEY_J, "J");
    map.addBinding(GLFW_KEY_K, "K");
    map.addBinding(GLFW_KEY_L, "L");
    map.addBinding(GLFW_KEY_M, "M");
    map.addBinding(GLFW_KEY_N, "N");
    map.addBinding(GLFW_KEY_O, "O");
    map.addBinding(GLFW_KEY_P, "P");
    map.addBinding(GLFW_KEY_Q, "Q");
    map.addBinding(GLFW_KEY_R, "R");
    map.addBinding(GLFW_KEY_S, "S");
    map.addBinding(GLFW_KEY_T, "T");
    map.addBinding(GLFW_KEY_U, "U");
    map.addBinding(GLFW_KEY_V, "V");
    map.addBinding(GLFW_KEY_W, "W");
    map.addBinding(GLFW_KEY_X, "X");
    map.addBinding(GLFW_KEY_Y, "Y");
    map.addBinding(GLFW_KEY_Z, "Z");

    // Number mappings
    map.addBinding(GLFW_KEY_0, "0");
    map.addBinding(GLFW_KEY_1, "1");
    map.addBinding(GLFW_KEY_2, "2");
    map.addBinding(GLFW_KEY_3, "3");
    map.addBinding(GLFW_KEY_4, "4");
    map.addBinding(GLFW_KEY_5, "5");
    map.addBinding(GLFW_KEY_6, "6");
    map.addBinding(GLFW_KEY_7, "7");
    map.addBinding(GLFW_KEY_8, "8");
    map.addBinding(GLFW_KEY_9, "9");

    // Special keys
    map.addBinding(GLFW_KEY_SPACE, "Space");
    map.addBinding(GLFW_KEY_ENTER, "Enter");
    map.addBinding(GLFW_KEY_ESCAPE, "Escape");
    map.addBinding(GLFW_KEY_TAB, "Tab");
    map.addBinding(GLFW_KEY_BACKSPACE, "Backspace");
    map.addBinding(GLFW_KEY_LEFT_SHIFT, "Left Shift");
    map.addBinding(GLFW_KEY_RIGHT_SHIFT, "Right Shift");
    map.addBinding(GLFW_KEY_LEFT_CONTROL, "Left Control");
    map.addBinding(GLFW_KEY_RIGHT_CONTROL, "Right Control");
    map.addBinding(GLFW_KEY_LEFT_ALT, "Left Alt");
    map.addBinding(GLFW_KEY_RIGHT_ALT, "Right Alt");

    // Arrow keys
    map.addBinding(GLFW_KEY_UP, "Up");
    map.addBinding(GLFW_KEY_DOWN, "Down");
    map.addBinding(GLFW_KEY_LEFT, "Left");
    map.addBinding(GLFW_KEY_RIGHT, "Right");

    // Function keys
    map.addBinding(GLFW_KEY_F1, "F1");
    map.addBinding(GLFW_KEY_F2, "F2");
    map.addBinding(GLFW_KEY_F3, "F3");
    map.addBinding(GLFW_KEY_F4, "F4");
    map.addBinding(GLFW_KEY_F5, "F5");
    map.addBinding(GLFW_KEY_F6, "F6");
    map.addBinding(GLFW_KEY_F7, "F7");
    map.addBinding(GLFW_KEY_F8, "F8");
    map.addBinding(GLFW_KEY_F9, "F9");
    map.addBinding(GLFW_KEY_F10, "F10");
    map.addBinding(GLFW_KEY_F11, "F11");
    map.addBinding(GLFW_KEY_F12, "F12");

    // Other common keys
    map.addBinding(GLFW_KEY_CAPS_LOCK, "Caps Lock");
    map.addBinding(GLFW_KEY_NUM_LOCK, "Num Lock");
    map.addBinding(GLFW_KEY_PRINT_SCREEN, "Print Screen");
    map.addBinding(GLFW_KEY_PAUSE, "Pause");

    return map;
}

static StringMap<GLFW_Keycode> keyboardMap = initGLFWKeyBinding();