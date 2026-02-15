#pragma once
#include <string>
class GPUDevice;

#include "Input/InputManager.h"
#include "Renderer/AnimationPool.h"
#include "Renderer/GameRenderer.h"
#include "Renderer/ScenePool.h"
#include "Sound/SoundPool.h"

class GameScene
{
  public:
    virtual void init(GPUDevice &device, std::string name, glm::ivec2 renderSize);
    virtual void update(float dt) {}

    virtual void draw() {}
    virtual void handleInput(float dt) {}
    virtual void updateGameLogic(float dt) {}

    virtual void        setSceneName(std::string name) { sceneName = name; }
    virtual std::string getSceneName() const { return this->sceneName; }

  protected:
    GPUDevice     *device;
    ScenePool     *scenePool;
    ImagePool     *imagePool;
    AnimationPool *animationPool;
    MeshPool      *meshPool;
    MaterialPool  *materialPool;
    InputManager  *inputManager;
    GameRenderer  *renderer;
    SoundPool     *soundPool;

    std::string sceneName;
};