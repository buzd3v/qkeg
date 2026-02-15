#include "Level/GameScene.h"
#include "Level/Level.h"

#include "Vulkan/GPUImage.h"

#include "Camera/Camera.h"
#include "CameraManager.h"
#include "DynamicCameraHandler.h"
#include "TrackingCameraHandler.h"

#include "ECS/ComponentLoader.h"
#include "ECS/EntityLoader.h"
#include "ECS/EntityUtil.h"
#include "Game.h"
class EntryScene : public GameScene
{
  public:
    void init(GPUDevice &device, std::string name, glm::ivec2 renderSize);
    void update(float dt) override;
    void handleInput(float dt) override;
    void updateGameLogic(float dt) override;
    void draw() override;

    void initDrawObjects();

    // entity management;
    void initEntityLoader();
    void registerComponent(ComponentLoader &comp);
    void postInitEnttCallback(entt::handle handle);
    void loadLevel(std::filesystem::path &path);
    void initAnimationEntity(entt::handle handle);

  private:
    Level          level;
    ImageId        gameImage{qTypes::NULL_IMAGE_ID};
    CameraManager  cameraManager;
    std::string    dynamicCameraTag{"dynamic"};
    std::string    trackingCameraTag{"tracking"};
    Camera         camera;
    entt::registry registry;
    entt::handle   playerHandle;
    EntityLoader  *enttLoader;

    glm::ivec2 windowSize;
};