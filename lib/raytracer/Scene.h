#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "raytracer/Shape.h"
#include "raytracer/Material.h"

class Shape;
class Material;

class Scene {

public:
    std::vector<Shape*> Shapes;
    std::vector<Material> Materials;

    ~Scene() {
        for (Shape* shape : Shapes) delete shape; 
    }
};