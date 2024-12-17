#pragma once
#include "MaterialProps.h"

struct GPUMaterial
{
    qColor::LinearColor baseColor{1.f, 1.f, 1.f, 1.f};
    float               metalicFactor{0.f};
    float               roughnessFactor{0.f};
    float               emissiveFactor{0.f};

    ImageId diffuseTexture{qTypes::NULL_IMAGE_ID};
    ImageId normalMapTexture{qTypes::NULL_IMAGE_ID};
    ImageId metallicRoughnessTexture{qTypes::NULL_IMAGE_ID};
    ImageId emissiveTexture{qTypes::NULL_IMAGE_ID};

    std::string name{""};
};