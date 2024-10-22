#include "Input/ButtonState.h"

ButtonState::ButtonState() : pressed(false), wasPressed(false) {}

bool ButtonState::isPressed() const
{
    return pressed;
}

bool ButtonState::isHeld() const
{
    return pressed && wasPressed;
}

bool ButtonState::wasJustPressed() const
{
    return !wasPressed && pressed;
}

bool ButtonState::wasJustReleased() const
{
    return !pressed && wasPressed;
}

void ButtonState::reset()
{
    pressed    = false;
    wasPressed = false;
}

void ButtonState::onNewFrame()
{
    wasPressed = pressed;
}
