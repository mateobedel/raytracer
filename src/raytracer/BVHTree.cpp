#include "raytracer/BVHTree.h"

void BVHTree::Intersect(const Ray& ray, std::vector<Shape*> shapes, HitPayLoad& payload) const {

    if (nodes.empty()) return; 
    IntersectBVH(ray, shapes, rootNodeId, payload);
}

void BVHTree::BuildBVH(std::vector<Shape*> shapes) {

    if (shapes.empty()) {
        nodes.clear();
        shapeId.clear();
        rootNodeId = 0;
        nodesUsed = 0;
        return;
    }

    //Init BVH
    if (shapes.size() == 0) return;

    nodes.resize(2 * shapes.size() - 1);
    shapeId.resize(shapes.size());

    for(int i = 0; i < shapes.size(); i++) shapeId[i] = i;
    rootNodeId = 0;
    nodesUsed = 1;

    //Build bvh
    BVHNode& root = nodes[rootNodeId];
    root.LeftNodeInd = 0;
    root.firstShapeInd = 0, root.nbShape = shapes.size();
    UpdateNodeBounds(rootNodeId, shapes);
    Subdivide(rootNodeId, shapes);
}

void BVHTree::UpdateNodeBounds(uint nodeId, std::vector<Shape*> shapes) {

    BVHNode& node = nodes[nodeId];
    node.aabbMin = glm::vec3(FLT_MAX);
    node.aabbMax = glm::vec3(FLT_MIN);

    for (int first = node.firstShapeInd, i = 0; i < node.nbShape; i++) {

        int leafShapeId = shapeId[first + i];
        Shape* leafShape = shapes[leafShapeId];

        node.aabbMin = glm::min(node.aabbMin, leafShape->GetAABBMin());
        node.aabbMax = glm::max(node.aabbMax, leafShape->GetAABBMax());
    }
}

void BVHTree::Subdivide(int nodeId, std::vector<Shape*> shapes) {

    BVHNode& node = nodes[nodeId];
    if (node.nbShape <= 2) return;

    //Split axis/position
    glm::vec3 extent = node.aabbMax - node.aabbMin;
    int axis = 0;
    if (extent.y > extent.x) axis = 1;
    if (extent.z > extent[axis]) axis = 2;
    float splitPos = node.aabbMin[axis] + extent[axis] * 0.5f;

    //Partition
    int i = node.firstShapeInd;
    int j = i + node.nbShape - 1;
    while (i <= j)
    {
        if (shapes[shapeId[i]]->Position[axis] < splitPos)
            i++;
        else
            std::swap(shapeId[i], shapeId[j--]);
    }

    //Abort split if one side is empty
    int leftNbShape = i - node.firstShapeInd;
    if (leftNbShape == 0 || leftNbShape == node.nbShape) return;

    //Create child nodes
    int leftChildId = nodesUsed++;
    int rightChildId = nodesUsed++;

    nodes[leftChildId].firstShapeInd = node.firstShapeInd;
    nodes[leftChildId].nbShape = leftNbShape;
    nodes[rightChildId].firstShapeInd = i;
    nodes[rightChildId].nbShape = node.nbShape - leftNbShape;

    node.LeftNodeInd = leftChildId;
    node.nbShape = 0;

    UpdateNodeBounds(leftChildId, shapes);
    UpdateNodeBounds(rightChildId, shapes);

    //Recurse
    Subdivide(leftChildId, shapes);
    Subdivide(rightChildId, shapes);
}


void BVHTree::IntersectBVH(const Ray& ray, std::vector<Shape*> shapes, const uint nodeId, HitPayLoad& payload) const {

    const BVHNode& node = nodes[nodeId];
    if (!IntersectAABB(ray, node.aabbMin, node.aabbMax, payload.HitDistance)) return;

    if (node.isLeaf()) {
        for (uint i = 0; i < node.nbShape; i++) {
            
            Shape* shape = shapes[shapeId[node.firstShapeInd + i]];
            float t;
            if (shape->intersect(ray, t) && t < payload.HitDistance) {
                payload.HitDistance = t;
                payload.HitShape = shape;
            }
        }
    } else {
        IntersectBVH( ray, shapes, node.LeftNodeInd, payload);
        IntersectBVH( ray, shapes, node.LeftNodeInd + 1, payload);
    }
}


bool BVHTree::IntersectAABB(const Ray& ray, const glm::vec3& bmin, const glm::vec3& bmax, float tMax) const {

    glm::vec3 invDir = 1.0f / ray.Direction;
    glm::vec3 t0s = (bmin - ray.Origin) * invDir;
    glm::vec3 t1s = (bmax - ray.Origin) * invDir;

    glm::vec3 tmin = glm::min(t0s, t1s);
    glm::vec3 tmax = glm::max(t0s, t1s);

    float tminMax = glm::max(tmin.x, glm::max(tmin.y, tmin.z));
    float tmaxMin = glm::min(tmax.x, glm::min(tmax.y, tmax.z));

    return tmaxMin >= tminMax && tminMax < tMax && tmaxMin > 0;
}