#include "Camera/Camera.h"

void Camera::initCamera(ProjectionType type, float horiFov, float nearPlane, float farPlane, float aspectRatio,
                        float xScale, float yScale)

{
    this->type = type;
    switch (type)
    {
    case ProjectionType::Perspective: {
        // finding vertical fov
        // ref https://calculator.academy/vertical-fov-calculator/
        verticalFov = 2.f * glm::atan(1.f / aspectRatio * glm::tan(horiFov / 2.f)); //kien.nx fixing vFov calculation

        this->horizontalFov = horiFov;
        this->verticalFov   = verticalFov;
        this->nearPlane     = nearPlane;
        this->farPlane      = farPlane;

        if (useInverseDepth)
        {
            projection = glm::perspective(verticalFov, aspectRatio, farPlane, nearPlane);
        }
        else
        {
            projection = glm::perspective(verticalFov, aspectRatio, nearPlane, farPlane);
        }

        if (reverseYAxis)
        {
            projection[1][1] *= -1;
        }
        isInit = true;
        break;
    }
    case ProjectionType::Orthorgraphic: {
        this->farPlane    = farPlane;
        this->nearPlane   = nearPlane;
        this->aspectRatio = scaleX / scaleY;
        this->scaleX      = xScale;
        this->scaleY      = yScale;

        if (useInverseDepth)
        {
            projection = glm::ortho(-scaleX, scaleX, -scaleY, scaleY, farPlane, nearPlane);
        }
        else
        {
            projection = glm::ortho(-scaleX, scaleX, -scaleY, scaleY, nearPlane, farPlane);
        }
        if (reverseYAxis)
        {
            projection[1][1] *= -1;
        }
        isInit = true;
        break;
    }
    case ProjectionType::Orthorgraphic2D: {
        // to match window's coord space
        reverseYAxis = true;

        this->farPlane    = farPlane;
        this->nearPlane   = nearPlane;
        this->scaleX      = xScale;
        this->scaleY      = yScale;
        this->aspectRatio = xScale / yScale;

        switch (defPos)
        {
        case DefaultPosition::LeftCorner: {
            projection = glm::ortho(0.f, xScale, 0.f, yScale, nearPlane, farPlane);
            break;
        }
        case DefaultPosition::Center: {
            projection = glm::ortho(-xScale / 2.f, xScale / 2.f, -yScale / 2.f, yScale / 2.f, nearPlane, farPlane);
            break;
        }
        default:
            break;
        }

        viewport = {xScale, yScale};
        isInit   = true;

        break;
    }
    default:
        break;
    }
}

void Camera::setUseInverseDepth(bool b)
{
    useInverseDepth = b;
    reinit();
}

void Camera::setUseReverseYaxis(bool b)
{
    this->reverseYAxis = b;
    reinit();
}

void Camera::reinit()
{
    initCamera(type, horizontalFov, nearPlane, farPlane, aspectRatio, scaleX, scaleY);
}

glm::mat4 Camera::getViewMatrix()
{
    assert(isInit && "The camera hasn't been init!");

    if (this->type == ProjectionType::Orthorgraphic2D)
    {
        return glm::translate(glm::mat4{1.f}, -transform.getPosition());
    }
    const auto up     = transform.up();
    const auto lookat = getPosititon() + transform.front();
    return glm::lookAt(getPosititon(), lookat, up);
}

glm::mat4 Camera::getViewProjectionMatrix()
{
    assert(isInit && "The camera hasn't been init!");
    return projection * getViewMatrix();
}

void Camera::setRotation(float yaw, float pitch)
{
    auto yawDeg   = glm::angleAxis(yaw, qConstant::AXES_UP);
    auto pitchDeg = glm::angleAxis(pitch, qConstant::AXES_RIGHT);
    setRotation(yawDeg * pitchDeg);
}
