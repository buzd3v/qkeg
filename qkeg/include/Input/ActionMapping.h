#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

#include "ActionTagHash.h"

class ActionMapping
{
  public:
    void loadActions(const std::filesystem::path &path);
    void initAxisState(const std::string &axisTagStr);
    void initActionState(const std::string &actionTagStr);

    // get action state
    bool isPressed(ActionTagHash tag) const;
    bool wasJustPressed(ActionTagHash tag) const;
    bool wasJustRelease(ActionTagHash tag) const;
    bool isHeld(ActionTagHash tag) const;

    float getTimePressed(ActionTagHash tag) const;
    float getAxisvalue(ActionTagHash tag) const;

    ActionTagHash getActionTagHash(const std::string &tagStr) const;

    void setActionKeyRepeatable(const std::string &tagStr, float startDelay, float keyRepeatPeriod);

  public:
    // functions below are used by InputManager to update actions and axes state
    void setActionPressed(ActionTagHash tag);
    void updateAxisState(ActionTagHash tag, float value);
    // functions used by InputManager to manage state of ActionMapping
    void onNewFrame();
    void update(float dt);
    void resetState();

  private:
    bool isActionMapped(ActionTagHash tag) const;
    bool isAxisMapped(ActionTagHash tag) const;

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

    std::unordered_map<std::string, ActionTagHash> actionHashes;

    std::unordered_map<ActionTagHash, AxisState>        axisStates;
    std::unordered_map<ActionTagHash, ActionState>      actionStates;
    std::unordered_map<ActionTagHash, KeyRepeatedState> keyRepeatedStates;
};