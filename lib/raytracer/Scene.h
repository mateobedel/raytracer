#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "raytracer/Shape.h"
#include "raytracer/Material.h"
#include "BVHTree.h"

class Shape;
class Material;

class Scene {

public:
    std::vector<Shape*> Shapes;
    std::vector<Material> Materials; //TODO : pointeur
    BVHTree bvh;



    ~Scene() {
        for (Shape* shape : Shapes) delete shape; 
    }
};