#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

#include "ActionName.h"

class ActionBinding
{
  public:
    void loadActions(const std::filesystem::path &path);
    void initAxisState(const std::string &axisTagStr);
    void initActionState(const std::string &actionTagStr);

    // get action state
    bool isPressed(ActionName tag) const;
    bool wasJustPressed(ActionName tag) const;
    bool wasJustRelease(ActionName tag) const;
    bool isHeld(ActionName tag) const;

    float getTimePressed(ActionName tag) const;
    float getAxisvalue(ActionName tag) const;

    ActionName getActionName(const std::string &tagStr) const;

    void setActionKeyRepeatable(const std::string &tagStr, float startDelay, float keyRepeatPeriod);

  public:
    // functions below are used by InputManager to update actions and axes state
    void setActionPressed(ActionName tag);
    void updateAxisState(ActionName tag, float value);
    // functions used by InputManager to manage state of ActionBinding
    void onNewFrame();
    void update(float dt);
    void resetState();

  private:
    bool isActionMapped(ActionName tag) const;
    bool isAxisMapped(ActionName tag) const;

    struct ActionState
    {
        float timePressed{0.f};
        bool  isPressed{false};
        bool  wasPressed{false};
    };

    struct AxisState
    {
        float value{0.f};
        float prevValue{0.f};
    };

    struct KeyRepeatedState
    {
        bool  repeat{false};
        float statDelay{0.f};
        bool  delayPressed{false};
        float repeatPeriod{0.f};
        float pressedValue{0.f};
    };

    std::unordered_map<std::string, ActionName> actionHashes;

    std::unordered_map<ActionName, AxisState>        axisStates;
    std::unordered_map<ActionName, ActionState>      actionStates;
    std::unordered_map<ActionName, KeyRepeatedState> keyRepeatedStates;
};