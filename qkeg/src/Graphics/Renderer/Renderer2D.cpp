#include "Renderer/Renderer2D.h"
#include "Camera/Camera.h"
void Renderer2D::collectDataBegin()
{
    drawProps.clear();
}

void Renderer2D::collectDataEnd() {}

void Renderer2D::addSprite(GPUDevice &device, Sprite &sprite, glm::mat4 transform)
{
    assert(sprite.texId != qTypes::NULL_IMAGE_ID);

    auto size = glm::abs(sprite.uv2 - sprite.uv1) * sprite.texSize;
    transform = glm::scale(transform, glm::vec3{size, 1.f});
    transform = glm::translate(transform, glm::vec3{-sprite.pivot, 0.f});

    drawProps.emplace_back(SpriteDrawProps{
        .transform = transform,
        .tlUV      = sprite.uv1,
        .brUV      = sprite.uv2,
        .color     = sprite.color,
        .textureId = sprite.texId,
    });
}

void Renderer2D::addSprite(GPUDevice &device, Sprite &sprite, glm::vec2 pos, float rot, glm::vec2 scale)
{
    Transform spriteTrans;
    spriteTrans.setPosition({pos, 0.f});
    spriteTrans.setRotation(glm::angleAxis(rot, glm::vec3{0.f, 0.f, 1.f}));
    spriteTrans.setScale({scale, 1.f});
    addSprite(device, sprite, std::move(spriteTrans.getTransformMatrix()));
}

void Renderer2D::draw(VkCommandBuffer cmd, GPUDevice &device, GPUImage &drawImage, glm::mat4 viewProj)
{
    auto drawExtent = drawImage.getExtent2D();
    auto drawSize   = glm::vec2{drawExtent.width, drawExtent.height};

    pipeline.draw(cmd, device, drawImage, viewProj, drawProps);
}

void Renderer2D::draw(VkCommandBuffer cmd, GPUDevice &device, GPUImage &drawImage)
{
    Camera camera;
    camera.initCamera(
        Camera::ProjectionType::Orthorgraphic2D, 0.f, 0.f, 1.f, 1.f, drawImage.getSize2D().x, drawImage.getSize2D().y);

    draw(cmd, device, drawImage, camera.getViewProjectionMatrix());
}

void Renderer2D::init(GPUDevice &device, VkFormat format)
{
    drawProps.reserve(qConstant::MAX_SPRITES);
    pipeline.initialize(device, format, qConstant::MAX_SPRITES);
    isInit = true;
}
