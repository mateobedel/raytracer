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
    Triangle() {
        Vertex V0 = {.Position = glm::vec3(-1,0,0)};
        Vertex V1 = {.Position = glm::vec3(1,0,0)};
        Vertex V2 = {.Position = glm::vec3(0,2,0)};

        V[0] = V0; V[1] = V1; V[2] = V2;
        onVertexChange();
    }


    Triangle(Vertex V0, Vertex V1, Vertex V2) {
        V[0] = V0; V[1] = V1; V[2] = V2;
        E[0] = V1.Position - V0.Position; E[1] = V2.Position - V0.Position; E[2] = V2.Position - V1.Position;
    
        Normal = glm::normalize(glm::cross(E[0], E[1])); 
        dPlane = - glm::dot(Normal, V[0].Position);
    }

    virtual bool intersect(const Ray& ray, float& intersectT) const;
    virtual HitPayLoad ClosestHit(const Ray& ray, float hitDistance);
    bool RenderUiSettings(int index, Scene& scene) override;

    void onVertexChange() {
        E[0] = V[1].Position - V[0].Position; E[1] = V[2].Position - V[0].Position; E[2] = V[2].Position - V[1].Position;
        Normal = glm::normalize(glm::cross(E[0], E[1])); 
        dPlane = - glm::dot(Normal, V[0].Position);
    }
    

public:
    Vertex V[3]; //Vertex
    glm::vec3 E[3]; //Edges

    glm::vec3 Normal;
    float dPlane; //d parameter of triangle's plane (ax+by+cz+d = 0)
};