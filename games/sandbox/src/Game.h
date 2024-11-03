#include "Application.h"
#include "Graphics/GPUDevice.h"
#include "Graphics/Vulkan/GPUImage.h"

class Game : public Application
{
  public:
    Game();

    virtual void loadAppSetting() override;

    virtual void customInit() override;
    virtual void customUpdate(float dt) override;
    virtual void customDraw() override;
    virtual void customCleanup() override;

  private:
};
