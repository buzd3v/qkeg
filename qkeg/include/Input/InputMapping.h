#pragma once

#include "glfw/glfw3.h"
#include <unordered_map>

using GLFW_Keycode = int;

template <typename T>
class StringMap
{
  public:
    void addMapping(const T &value, const std::string &str)
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

StringMap<GLFW_Keycode> initGLFWKeyMapping()
{
    StringMap<GLFW_Keycode> map;

    // number
    map.addMapping(GLFW_KEY_A, "A");
    map.addMapping(GLFW_KEY_B, "B");
    map.addMapping(GLFW_KEY_C, "C");
    map.addMapping(GLFW_KEY_D, "D");
    map.addMapping(GLFW_KEY_E, "E");
    map.addMapping(GLFW_KEY_F, "F");
    map.addMapping(GLFW_KEY_G, "G");
    map.addMapping(GLFW_KEY_H, "H");
    map.addMapping(GLFW_KEY_I, "I");
    map.addMapping(GLFW_KEY_J, "J");
    map.addMapping(GLFW_KEY_K, "K");
    map.addMapping(GLFW_KEY_L, "L");
    map.addMapping(GLFW_KEY_M, "M");
    map.addMapping(GLFW_KEY_N, "N");
    map.addMapping(GLFW_KEY_O, "O");
    map.addMapping(GLFW_KEY_P, "P");
    map.addMapping(GLFW_KEY_Q, "Q");
    map.addMapping(GLFW_KEY_R, "R");
    map.addMapping(GLFW_KEY_S, "S");
    map.addMapping(GLFW_KEY_T, "T");
    map.addMapping(GLFW_KEY_U, "U");
    map.addMapping(GLFW_KEY_V, "V");
    map.addMapping(GLFW_KEY_W, "W");
    map.addMapping(GLFW_KEY_X, "X");
    map.addMapping(GLFW_KEY_Y, "Y");
    map.addMapping(GLFW_KEY_Z, "Z");

    // Number mappings
    map.addMapping(GLFW_KEY_0, "0");
    map.addMapping(GLFW_KEY_1, "1");
    map.addMapping(GLFW_KEY_2, "2");
    map.addMapping(GLFW_KEY_3, "3");
    map.addMapping(GLFW_KEY_4, "4");
    map.addMapping(GLFW_KEY_5, "5");
    map.addMapping(GLFW_KEY_6, "6");
    map.addMapping(GLFW_KEY_7, "7");
    map.addMapping(GLFW_KEY_8, "8");
    map.addMapping(GLFW_KEY_9, "9");

    // Special keys
    map.addMapping(GLFW_KEY_SPACE, "Space");
    map.addMapping(GLFW_KEY_ENTER, "Enter");
    map.addMapping(GLFW_KEY_ESCAPE, "Escape");
    map.addMapping(GLFW_KEY_TAB, "Tab");
    map.addMapping(GLFW_KEY_BACKSPACE, "Backspace");
    map.addMapping(GLFW_KEY_LEFT_SHIFT, "Left Shift");
    map.addMapping(GLFW_KEY_RIGHT_SHIFT, "Right Shift");
    map.addMapping(GLFW_KEY_LEFT_CONTROL, "Left Control");
    map.addMapping(GLFW_KEY_RIGHT_CONTROL, "Right Control");
    map.addMapping(GLFW_KEY_LEFT_ALT, "Left Alt");
    map.addMapping(GLFW_KEY_RIGHT_ALT, "Right Alt");

    // Arrow keys
    map.addMapping(GLFW_KEY_UP, "Up Arrow");
    map.addMapping(GLFW_KEY_DOWN, "Down Arrow");
    map.addMapping(GLFW_KEY_LEFT, "Left Arrow");
    map.addMapping(GLFW_KEY_RIGHT, "Right Arrow");

    // Function keys
    map.addMapping(GLFW_KEY_F1, "F1");
    map.addMapping(GLFW_KEY_F2, "F2");
    map.addMapping(GLFW_KEY_F3, "F3");
    map.addMapping(GLFW_KEY_F4, "F4");
    map.addMapping(GLFW_KEY_F5, "F5");
    map.addMapping(GLFW_KEY_F6, "F6");
    map.addMapping(GLFW_KEY_F7, "F7");
    map.addMapping(GLFW_KEY_F8, "F8");
    map.addMapping(GLFW_KEY_F9, "F9");
    map.addMapping(GLFW_KEY_F10, "F10");
    map.addMapping(GLFW_KEY_F11, "F11");
    map.addMapping(GLFW_KEY_F12, "F12");

    // Other common keys
    map.addMapping(GLFW_KEY_CAPS_LOCK, "Caps Lock");
    map.addMapping(GLFW_KEY_NUM_LOCK, "Num Lock");
    map.addMapping(GLFW_KEY_PRINT_SCREEN, "Print Screen");
    map.addMapping(GLFW_KEY_PAUSE, "Pause");

    return map;
}

static StringMap<GLFW_Keycode> keyboardMap = initGLFWKeyMapping();