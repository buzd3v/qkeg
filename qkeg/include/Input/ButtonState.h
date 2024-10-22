#ifndef __BUTTONSTATE_H__
#define __BUTTONSTATE_H__

struct ButtonState
{
    bool wasPressed{false};
    bool pressed{false};

    ButtonState();

    bool isPressed() const;
    bool isHeld() const;
    bool wasJustPressed() const;
    bool wasJustReleased() const;
    void reset();
    void onNewFrame();
};

#endif // __BUTTONSTATE_H__