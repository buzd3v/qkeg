#include "Game.h"
#include "glm/glm.hpp"
void Game::handleInput(float deltaTime) {}

void Game::handlePlayerInput(float dt)
{
    const auto       &actionBind        = inputManager.getActionBinding();
    static const auto horizonalWalkAxis = actionBind.getActionName("MoveX");
    static const auto verticalWalkAxis  = actionBind.getActionName("MoveY");

    const auto moveStickState =
        glm::vec2(actionBind.getAxisvalue(horizonalWalkAxis), actionBind.getAxisvalue(verticalWalkAxis));
}
