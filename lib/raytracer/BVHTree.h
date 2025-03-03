#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <cstdio>

#include "Shape.h"


#define BINS 100


//Node of a BVH Tree
struct BVHNode {
    glm::vec3 aabbMin, aabbMax;
    uint nbShape;   
    uint LeftFirst; // if (nbShape == 0) : contains the index of the leftChildNode
                    // else :              contains the index of the first shape index         
};


//Axis-Aligned Bounding Box
struct AABB { 

    glm::vec3 bmin{FLT_MAX}, bmax{-FLT_MAX};

    void grow(glm::vec3 p) {
        bmin = glm::min(bmin, p);
        bmax = glm::max(bmax, p); 
    }

    void grow(AABB& b) {
        if (b.bmin.x != FLT_MAX) {
            grow(b.bmin);
            grow(b.bmax);
        }
    }

    float area() { 
        glm::vec3 e = bmax - bmin;
        return e.x*e.y + e.y*e.z + e.z*e.x; 
    }
};

//Bounds of subdivised intervals 
struct Bin { 
    AABB bounds;
    int nbShape = 0; 
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

        float FindBestSplitPlane(BVHNode& node, int& axis, float& splitPos, const std::vector<Shape*>& shapes);

        float CalcNodeCost(BVHNode& node);

        void Subdivide(int nodeId, const std::vector<Shape*>& shapes);

        float EvaluateSAH(BVHNode& node, int axis, float pos, const std::vector<Shape*>& shapes);

        void IntersectBVH(const Ray& ray, const std::vector<Shape*>& shapes, const uint nodeId, HitPayLoad& payload) const;

        float IntersectAABB(const Ray& ray, const glm::vec3& bmin, const glm::vec3& bmax, float tMax) const;

};