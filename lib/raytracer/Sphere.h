#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "raytracer/Shape.h"
#include "raytracer/Material.h"
#include "raytracer/Scene.h"
#include <memory>


class Sphere : public Shape {

public:

    Sphere() {};
    Sphere(glm::vec3 p, int i, float r) : Shape(p,i), Radius(r) {};

    virtual bool intersect(const Ray& ray, float& intersectT) const;
    virtual HitPayLoad ClosestHit(const Ray& ray, float hitDistance);

    bool RenderUiSettings(int index, Scene& scene) override;

public:
    float Radius = .5f;
};