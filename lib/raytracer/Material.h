#pragma once

#include <glm/glm.hpp>
#include <vector>

class Material {

public:
    glm::vec3 Albedo{1.0f};
    float Roughness = 1.0f;
    float Metallic = 0.0f;
    float EmmissionPower = 0.0f;
    glm::vec3 EmissionColor{0.0f};

    glm::vec3 GetEmission() const {return EmmissionPower*EmissionColor;}
    bool RenderUiSettings(int index);
};