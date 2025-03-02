#pragma once

#include <glm/glm.hpp>
#include "raytracer/Ray.h"
#include "raytracer/Material.h"

struct HitPayLoad;
struct Scene;


class Shape {
    public:

        Shape();
        Shape(glm::vec3 pos, int i);

        virtual bool intersect(const Ray& ray, float& intersectT) const = 0;

        virtual void ClosestHit(const Ray& ray, HitPayLoad& payload) = 0;
        static void Miss(const Ray& ray, HitPayLoad& payload);

        virtual bool RenderUiSettings(int index, Scene& scene) = 0;
        bool RenderUiMaterial(Scene& scene);
        bool RenderDeleteButton(int index, Scene& scene);

        virtual glm::vec3 GetAABBMin() const = 0;
        virtual glm::vec3 GetAABBMax() const = 0;


    public:
        glm::vec3 Position{.0f, .0f, .0f};
        int MaterialIndex = 0;
};