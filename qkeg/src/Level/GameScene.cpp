#include "Level/GameScene.h"

#include "GPUDevice.h"

#include "Mesh/MaterialPool.h"
#include "Mesh/MeshPool.h"
#include "Renderer/AnimationPool.h"
#include "Renderer/ScenePool.h"
#include "Vulkan/ImagePool.h"

void GameScene::init(GPUDevice &device, std::string name, glm::vec2 renderSize)
{
    this->device    = &device;
    scenePool       = ScenePool::GetInstance();
    materialPool    = MaterialPool::GetInstance();
    meshPool        = MeshPool::GetInstance();
    animationPool   = AnimationPool::GetInstance();
    imagePool       = ImagePool::GetInstance();
    inputManager    = InputManager::GetInstance();
    renderer        = GameRenderer::GetInstance();
    this->sceneName = name;
}