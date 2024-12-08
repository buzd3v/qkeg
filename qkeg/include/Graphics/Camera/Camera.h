#pragma once

#include "Math/Transform.h"

class Camera
{
  public:
    enum class DefaultPosition
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

    // init
  public:
    void    initCamera(float horiFov, float nearPlane, float farPlane, float aspectRatio = 1.f, float xScale = 1.f,
                       float yScale = 1.f);
    Camera &setProjectionType(ProjectionType type)
    {
        this->type = type;
        return *this;
    }
    Camera &setOriginPosition(DefaultPosition pos)
    {
        this->defPos = pos;
        return *this;
    }
    // getter and setter
  public:
    const Transform &getTransform() { return transform; }
    const glm::vec3  getPosititon() { return transform.getPosition(); }
    const glm::vec2  getPosititon2D() { return glm::vec2(transform.getPosition()); }
    const glm::mat4  getProjection() { return projection; }
    const glm::quat  getRotation() { return transform.getRotation(); }

    void setPosition(const glm::vec3 pos) { transform.setPosition(pos); }
    void setPosition(const glm::vec2 pos) { transform.setPosition(glm::vec3(pos, 0.f)); }
    void setRotation(const glm::quat rot) { transform.setRotation(rot); }
    void setProjection(const glm::mat4 projection) { this->projection = projection; }

    void setUseInverseDepth(bool b);
    void useReverseYaxis(bool b);

    bool isUseInverseDepth() { return useInverseDepth; }
    bool isUseReverseYAxis() { return reverseYAxis; }

    ProjectionType getProjectionType() { return type; }

    float getNearPlane() const { return nearPlane; }
    float getFarPlane() const { return farPlane; }
    float getAspectRatio() const { return aspectRatio; }
    float getHorizontalFov() const { return horizontalFov; }
    float getVerticalFov() const { return verticalFov; }

    void reinit();

    // helper to calculate mvp matrix component
  public:
    glm::mat4 getViewMatrix();
    glm::mat4 getViewProjectionMatrix();
    void      setRotation(float yaw, float pitch);

  private:
    Transform       transform;
    glm::mat4       projection;
    ProjectionType  type;
    DefaultPosition defPos;

    float nearPlane{1.f};
    float farPlane{100.f};
    float aspectRatio{16.f / 9};
    float horizontalFov{glm::radians(90.f)};
    float verticalFov{glm::radians(60.f)};

    // zooming ( only for orthor camera)
    float     scaleX = 1.f;
    float     scaleY = 1.f;
    glm::vec2 viewport;

    bool isInit{false};
    bool useInverseDepth{false}; // inverse depth for near->far = 1->0
    bool reverseYAxis{false}; // since vulkan use positive Y axis point up, use this to make y axis point down to match
                              // other app's coordinate space
};