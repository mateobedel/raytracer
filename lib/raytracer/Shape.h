#pragma once

#include <glm/glm.hpp>
#include "raytracer/Ray.h"
#include "raytracer/Material.h"

struct HitPayLoad;
struct Scene;


class Shape {
    public:

        Shape() {};
        Shape(glm::vec3 pos, int i) : Position(pos), MaterialIndex(i) {};

        virtual bool intersect(const Ray& ray, float& intersectT) const = 0;
        virtual HitPayLoad ClosestHit(const Ray& ray, float hitDistance) = 0;
        HitPayLoad Miss(const Ray& ray) { 
            HitPayLoad hit;
            hit.HitDistance = -1.0f;
            return hit;
        }

        virtual bool RenderUiSettings(int index, Scene& scene) = 0;

        bool RenderUiMaterial(Scene& scene);


    public:
        glm::vec3 Position{.0f, .0f, .0f};
        int MaterialIndex = 0;
};