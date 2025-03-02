#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "raytracer/Shape.h"
#include "raytracer/Material.h"
#include "raytracer/Scene.h"
#include <memory>


class Sphere : public Shape {

public:

    Sphere();
    Sphere(glm::vec3 p, int i, float r);

    glm::vec3 GetAABBMin() const override;
    glm::vec3 GetAABBMax() const override;

    virtual bool intersect(const Ray& ray, float& intersectT) const;
    virtual void ClosestHit(const Ray& ray, HitPayLoad& payload);

    bool RenderUiSettings(int index, Scene& scene) override;

public:
    float Radius = .5f;
};