#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "raytracer/Shape.h"
#include "raytracer/Material.h"
#include <memory>

class Shape;
class Material;

class Scene {

public:
    std::vector<std::shared_ptr<Shape>> Shapes;
    std::vector<std::shared_ptr<Material>> Materials;
};