#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <cstdio>

#include "Shape.h"

//Node of a BVH Tree
struct BVHNode {
    glm::vec3 aabbMin, aabbMax;
    uint nbShape;   
    uint LeftFirst; // if (nbShape == 0) : contains the index of the leftChildNode
                    // else :              contains the index of the first shape index         
};

//Axis-Aligned Bounding Box
struct AABB { 

    glm::vec3 bmin{FLT_MAX}, bmax{FLT_MIN};

    void grow(glm::vec3 p) {
        bmin = glm::min(bmin, p);
        bmax = glm::max(bmax, p); 
    }

    float area() { 
        glm::vec3 e = bmax - bmin;
        return e.x*e.y + e.y*e.z + e.z*e.x; 
    }
};

class BVHTree {

    public:
        std::vector<BVHNode> nodes;
        std::vector<int> shapeId;
        int rootNodeId, nodesUsed;

        void BuildBVH(const std::vector<Shape*>& shapes);

        void Intersect(const Ray& ray, const std::vector<Shape*>& shapes, HitPayLoad& payload) const;

    private:

        void UpdateNodeBounds(uint nodeId, const std::vector<Shape*>& shapes);

        void Subdivide(int nodeId, const std::vector<Shape*>& shapes);

        float EvaluateSAH(BVHNode& node, int axis, float pos, const std::vector<Shape*>& shapes);

        void IntersectBVH(const Ray& ray, const std::vector<Shape*>& shapes, const uint nodeId, HitPayLoad& payload) const;

        float IntersectAABB(const Ray& ray, const glm::vec3& bmin, const glm::vec3& bmax, float tMax) const;

};