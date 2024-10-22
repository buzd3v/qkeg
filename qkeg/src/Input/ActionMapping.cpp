#include "Input/ActionMapping.h"
#include "JsonParser/JsonFile.h"

void ActionMapping::loadActions(const std::filesystem::path &path)
{
    JsonFile file(path);

    if (!file.isGood())
    {
        throw std::runtime_error(fmt::format("Failed to load file from {}", path.string()));
    }

    const auto rootNode = file.getRootNode();

    for (const auto &action : rootNode.getNode("actions").asVectorOf<std::string>())
    {
        initActionState(action);
    }
    for (const auto &axis : rootNode.getNode("axes").asVectorOf<std::string>())
    {
        initActionState(axis);
    }
}

void ActionMapping::initAxisState(const std::string &axisTagStr)
{
    auto id                  = static_cast<ActionTagHash>(actionHashes.size());
    actionHashes[axisTagStr] = id;
    (void)axisStates[id];
}

void ActionMapping::initActionState(const std::string &actionTagStr)
{
    auto id                    = static_cast<ActionTagHash>(actionHashes.size());
    actionHashes[actionTagStr] = id;
    (void)actionStates[id];
}

bool ActionMapping::isPressed(ActionTagHash tag) const
{
    return actionStates.at(tag).isPressed;
}

bool ActionMapping::wasJustPressed(ActionTagHash tag) const
{
    if (!isActionMapped(tag))
        return false;
    auto &state = actionStates.at(tag);

    bool justPressed = state.isPressed && (!state.wasPressed);
    if (justPressed)
        return true;

    if (const auto &keyRep = keyRepeatedStates.find(tag); keyRep != keyRepeatedStates.end())
        return keyRep->second.repeat;

    return false;
}

bool ActionMapping::wasJustRelease(ActionTagHash tag) const
{
    if (!isActionMapped(tag))
        return false;
    auto &state = actionStates.at(tag);

    bool justRelease = (!state.isPressed) && state.wasPressed;
    return justRelease;
}

bool ActionMapping::isHeld(ActionTagHash tag) const
{
    if (!isActionMapped(tag))
        return false;
    auto &state = actionStates.at(tag);

    bool justHold = state.isPressed && state.wasPressed;
    return justHold;
}

float ActionMapping::getTimePressed(ActionTagHash tag) const
{
    if (!isActionMapped(tag))
        return false;
    auto &state = actionStates.at(tag);

    return state.timePressed;
}

float ActionMapping::getAxisvalue(ActionTagHash tag) const
{
    auto state = axisStates.find(tag);

    if (state != axisStates.end())
    {
        return state->second.value;
    }
    fmt::print("Axis not found: {}", tag);
    return 0.f;
}

void ActionMapping::setActionKeyRepeatable(const std::string &tagStr, float startDelay, float keyRepeatPeriod) {}

bool ActionMapping::isAxisMapped(ActionTagHash tag) const
{
    return (tag != ACTION_NONE_HASH) && (axisStates.find(tag) != axisStates.end());
}

bool ActionMapping::isActionMapped(ActionTagHash tag) const
{
    return (tag != ACTION_NONE_HASH) && (actionStates.find(tag) != actionStates.end());
}
ActionTagHash ActionMapping::getActionTagHash(const std::string &tagStr) const
{
    const auto &it = actionHashes.find(tagStr);
    if (it == actionHashes.end())
    {
        throw std::runtime_error(fmt::format("Cannot found this tag {} ", tagStr));
    }
    return it->second;
}

void ActionMapping::setActionPressed(ActionTagHash tag)
{
    if (!isActionMapped(tag))
    {
        return;
    }
    auto &state     = actionStates.at(tag);
    state.isPressed = true;
}

void ActionMapping::updateAxisState(ActionTagHash tag, float value)
{
    if (!isAxisMapped(tag))
    {
        return;
    }
    axisStates.at(tag).value += value;
}

void ActionMapping::onNewFrame()
{
    for (auto &[tag, actionState] : actionStates)
    {
        actionState.wasPressed = actionState.isPressed;
        actionState.isPressed  = false;
    }
    for (auto &[tag, axisState] : axisStates)
    {
        axisState.prevValue = axisState.value;
        axisState.value     = 0.f;
    }
}

void ActionMapping::update(float dt)
{
    for (auto &[tag, state] : actionStates)
    {
        if (state.isPressed)
        {
            state.timePressed += dt;
        }
        else
        {
            state.timePressed = 0;
        }
    }
    for (auto &[tag, axisState] : axisStates)
    {
        axisState.prevValue = axisState.value;
        axisState.value     = std::clamp(axisState.value, -1.f, 1.f);
    }
}

void ActionMapping::resetState()
{
    for (auto &[tag, state] : actionStates)
    {
        state.isPressed   = false;
        state.wasPressed  = false;
        state.timePressed = 0.f;
    }

    for (auto &[tag, state] : axisStates)
    {
        state.value     = 0.f;
        state.prevValue = 0.f;
    }
}
