#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace qConstant
{
inline constexpr glm::vec3 AXES_UP{0.f, 1.f, 0.f};
inline constexpr glm::vec3 AXES_DOWN{0.f, -1.f, 0.f};
inline constexpr glm::vec3 AXES_LEFT{1.f, 0.f, 0.f};
inline constexpr glm::vec3 AXES_RIGHT{-1.f, 0.f, 0.f};
inline constexpr glm::vec3 AXES_FRONT{0.f, 0.f, 1.f};
inline constexpr glm::vec3 AXES_BACK{0.f, 0.f, -1.f};
inline constexpr glm::mat4 IDENTITY_MAT{1.f};

} // namespace qConstant
// global axes
/*
        ^ +Y
        |
        |
        |
<----(^._.^)----->
-X     /         +X
      /
     v  +Z
*/

class Transform
{
  public:
    Transform() : position(0.0f), rotation(glm::quat()), scale(1.0f) {}
    Transform(const glm::vec3 &pos, const glm::quat &rot, const glm::vec3 &scl)
        : position(pos), rotation(rot), scale(scl)
    {
    }

    Transform(const glm::mat4 &model);

    // Transform &operator==(Transform &other);  // copy constructor
    // Transform &operator==(Transform &&other); // move constructor
    bool      operator==(const Transform &other) const;
    Transform operator*(const Transform &other) const;
    Transform inverse();

    // Getters and Setters
    glm::vec3 getPosition() const { return position; }
    glm::quat getRotation() const { return rotation; }
    glm::vec3 getScale() const { return scale; }

    void setPosition(const glm::vec3 &pos);
    void setRotation(const glm::quat &rot);
    void setScale(const glm::vec3 &scl);

    // Combine transformations into a single matrix
    glm::mat4 getTransformMatrix() const;

    // Helper functions to manipulate the transform
    void translate(const glm::vec3 &offset);
    void rotate(const glm::vec3 &axis, float angle);
    void scaleBy(const glm::vec3 &factor);
    bool isIdentity() const;

    glm::vec3 up();
    glm::vec3 right();
    glm::vec3 front();

  private:
    glm::vec3 position;
    glm::vec3 scale;
    glm::quat rotation;

    mutable glm::mat4 transformMat;
    mutable bool      isInit{false};
};