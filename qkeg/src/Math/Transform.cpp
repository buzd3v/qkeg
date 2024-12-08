#include "Math/Transform.h"

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/matrix_decompose.hpp>

Transform::Transform(const glm::mat4 &model)
{
    glm::vec3 scaleVec;
    glm::quat rotationQuat;
    glm::vec3 translationVec;
    glm::vec3 skewVec;
    glm::vec4 perspectiveVec;
    glm::decompose(model, scaleVec, rotationQuat, translationVec, skewVec, perspectiveVec);

    position = translationVec;
    scale    = scaleVec;
    rotation = rotationQuat;
}

void Transform::translate(const glm::vec3 &offset)
{
    position += offset;
    isInit = false;
}
void Transform::rotate(const glm::vec3 &axis, float angle)
{
    rotation = glm::rotate(rotation, glm::radians(angle), axis);
    isInit   = false;
}
void Transform::scaleBy(const glm::vec3 &factor)
{
    scale *= factor;
    isInit = false;
}

bool Transform::operator==(const Transform &other) const
{
    return position == other.getPosition() && scale == other.getScale() && rotation == other.getRotation();
}

Transform Transform::operator*(const Transform &other) const
{
    if (isIdentity())
        return other;
    if (other.isIdentity())
        return *this;
    return Transform(getTransformMatrix() * other.getTransformMatrix());
}

void Transform::setPosition(const glm::vec3 &pos)
{
    position = pos;
    isInit   = false;
}

void Transform::setRotation(const glm::quat &rot)
{
    this->rotation = glm::normalize(rot);
    isInit         = false;
}

void Transform::setScale(const glm::vec3 &scl)
{
    scale  = scl;
    isInit = false;
}

glm::mat4 Transform::getTransformMatrix() const
{
    if (isInit)
    {
        return transformMat;
    }

    transformMat = glm::translate(qConstant::IDENTITY_MAT, position);
    if (rotation != glm::identity<glm::quat>())
    {
        transformMat *= glm::mat4_cast(rotation);
    }
    transformMat = glm::scale(transformMat, scale);
    isInit       = true;
    return transformMat;
}

Transform Transform::inverse()
{
    if (isIdentity())
        return Transform(glm::mat4{1.f});
    return Transform(glm::inverse(getTransformMatrix()));
}

bool Transform::isIdentity() const
{
    return getTransformMatrix() == qConstant::IDENTITY_MAT;
}

glm::vec3 Transform::up()
{
    return rotation * qConstant::AXES_UP;
}
glm::vec3 Transform::right()
{
    return rotation * qConstant::AXES_FRONT;
}
glm::vec3 Transform::front()
{
    return rotation * qConstant::AXES_RIGHT;
}