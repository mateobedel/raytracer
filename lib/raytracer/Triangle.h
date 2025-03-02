#pragma once

#include <glm/glm.hpp>
#include "raytracer/Shape.h"
#include <cstdio>

struct Vertex {
public:
    glm::vec3 Position;
    glm::vec3 Normal;
};

class Triangle : public Shape {

public:
    Triangle();
    Triangle(Vertex V0, Vertex V1, Vertex V2);

    glm::vec3 GetAABBMin() const override;
    glm::vec3 GetAABBMax() const override;
    
    virtual bool intersect(const Ray& ray, float& intersectT) const;
    virtual void ClosestHit(const Ray& ray, HitPayLoad& payload);
    bool RenderUiSettings(int index, Scene& scene) override;
    void onVertexChange();
    

public:
    Vertex V[3]; //Vertex
    glm::vec3 E[3]; //Edges

    glm::vec3 Normal;
    float dPlane; //d parameter of triangle's plane (ax+by+cz+d = 0)
};