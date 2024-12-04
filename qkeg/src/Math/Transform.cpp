#include "Math/Transform.h"

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
