#pragma once

#include "Math/Transform.h"

class Camera
{
  public:
    enum class Position
    {
        Center,
        LeftCorner,
    };
    enum class ProjectionType
    {
        Orthorgraphic,
        Orthorgraphic2D,
        Perspective,
    };

  public:
  private:
    Transform      transform;
    glm::mat4      projection;
    ProjectionType type;

    float near{1.f};
    float far{100.f};
    float aspectRatio{16.f / 9};
    float fovX{glm::radians(90.f)};
    float fovY{glm::radians(60.f)};

    // zooming ( only for orthor camera)
    float     scaleX = 1.f;
    float     scaleY = 1.f;
    glm::vec2 viewport;

    bool isInit{false};
    bool useInverseDepth{false}; // inverse depth for near->far = 1->0
    bool reverseYAxis{false}; // since vulkan use positive Y axis point up, use this to make y axis point down to match
                              // other app's coordinate space
};