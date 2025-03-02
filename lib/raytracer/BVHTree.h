#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <cstdio>

#include "Shape.h"

struct BVHNode {
    glm::vec3 aabbMin, aabbMax;
    int LeftNodeInd, firstShapeInd, nbShape;
    bool isLeaf() const { return nbShape > 0; }
};


class BVHTree {

public:
    std::vector<BVHNode> nodes;
    std::vector<int> shapeId;
    int rootNodeId, nodesUsed;

    void BuildBVH(std::vector<Shape*> shapes);

    void Intersect(const Ray& ray, std::vector<Shape*> shapes, HitPayLoad& payload) const;

private:

    void UpdateNodeBounds(uint nodeId, std::vector<Shape*> shapes);

    void Subdivide(int nodeId, std::vector<Shape*> shapes);

    void IntersectBVH(const Ray& ray, std::vector<Shape*> shapes, const uint nodeId, HitPayLoad& payload) const;

    bool IntersectAABB(const Ray& ray, const glm::vec3& bmin, const glm::vec3& bmax, float tMax) const;

};