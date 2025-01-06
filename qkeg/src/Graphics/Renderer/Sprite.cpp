#include "Renderer/Sprite.h"
#include "Vulkan/ImagePool.h"

Sprite::Sprite(ImageId id)
{

    setTexture(id);
}

void Sprite::setTexture(ImageId id)
{
    auto pool    = ImagePool::GetInstance();
    texId        = id;
    auto texture = pool->getImage(texId);

    texSize = glm::vec2{texture.getExtent2D().width, texture.getExtent2D().height};
}

void Sprite::setTextureBound(const Rect<int> texRect)
{
    uv1 = glm::vec2{texRect.top, texRect.left} / texSize;
    uv2 = glm::vec2{texRect.top + texRect.width, texRect.left + texRect.height} / texSize;
}

void Sprite::setPivot(glm::vec2 pos)
{
    pivot = glm::vec2{pos} / texSize;
}