#pragma once

#include "raytracer/Ray.h"

class Scene;

class Shape {
    public:

        Shape() {};
        Shape(glm::vec3 pos, int i) : Position(pos), MaterialIndex(i) {};

        virtual float intersect(const Ray& ray) const = 0;
        virtual bool RenderUiSettings(int index, Scene& scene) = 0;

    public:
        glm::vec3 Position{.0f, .0f, .0f};
        int MaterialIndex = 0;
};