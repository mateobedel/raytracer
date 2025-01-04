#pragma once

#include <glm/glm.hpp>

class Shape;

struct Ray {
    glm::vec3 Origin;
    glm::vec3 Direction;
};

struct HitPayLoad {
    float HitDistance;
    glm::vec3 WorldPosition;
    glm::vec3 WorldNormal;

    Shape* HitShape;
};