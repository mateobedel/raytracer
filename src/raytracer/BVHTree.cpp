#include "raytracer/BVHTree.h"

void BVHTree::Intersect(const Ray& ray, const std::vector<Shape*>& shapes, HitPayLoad& payload) const {

    if (nodes.empty()) return; 
    IntersectBVH(ray, shapes, rootNodeId, payload);
}

void BVHTree::BuildBVH(const std::vector<Shape*>& shapes) {

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
    root.LeftFirst = 0;
    root.nbShape = shapes.size();
    UpdateNodeBounds(rootNodeId, shapes);
    Subdivide(rootNodeId, shapes);
}

void BVHTree::UpdateNodeBounds(uint nodeId, const std::vector<Shape*>& shapes) {

    BVHNode& node = nodes[nodeId];
    node.aabbMin = glm::vec3(FLT_MAX);
    node.aabbMax = glm::vec3(FLT_MIN);

    for (int first = node.LeftFirst, i = 0; i < node.nbShape; i++) {

        int leafShapeId = shapeId[first + i];
        Shape* leafShape = shapes[leafShapeId];

        node.aabbMin = glm::min(node.aabbMin, leafShape->GetAABBMin());
        node.aabbMax = glm::max(node.aabbMax, leafShape->GetAABBMax());
    }
}

void BVHTree::Subdivide(int nodeId, const std::vector<Shape*>& shapes) {

    BVHNode& node = nodes[nodeId];
    
    // Calculate parent SAH cost
    glm::vec3 e = node.aabbMax - node.aabbMin;
    float parentArea = e.x * e.y + e.y * e.z + e.z * e.x;
    float parentCost = node.nbShape * parentArea;

    // Determine split axis using SAH
    int bestAxis = -1;
    float bestPos = 0, bestCost = 1e30f;

    for( int axis = 0; axis < 3; axis++ ) {
        for( uint i = 0; i < node.nbShape; i++ ) {
        
            Shape* shape = shapes[shapeId[node.LeftFirst + i]];
            float candidatePos = shape->Position[axis];

            float cost = EvaluateSAH(node, axis, candidatePos, shapes);
            if (cost < bestCost) {
                bestPos = candidatePos;
                bestAxis = axis;
                bestCost = cost;
            }
        }
    }

    // Abort if split does not reduce cost
    if (bestCost >= parentCost) return;

    int axis = bestAxis;
    float splitPos = bestPos; 

    //Partition
    int i = node.LeftFirst;
    int j = i + node.nbShape - 1;
    while (i <= j) {
        if (shapes[shapeId[i]]->Position[axis] < splitPos)
            i++;
        else
            std::swap(shapeId[i], shapeId[j--]);
    }

    //Abort split if one side is empty
    int leftNbShape = i - node.LeftFirst;
    if (leftNbShape == 0 || leftNbShape == node.nbShape) return;

    //Create child nodes
    int leftChildId = nodesUsed++;
    int rightChildId = nodesUsed++;

    nodes[leftChildId].LeftFirst = node.LeftFirst;
    nodes[leftChildId].nbShape = leftNbShape;
    nodes[rightChildId].LeftFirst = i;
    nodes[rightChildId].nbShape = node.nbShape - leftNbShape;

    node.LeftFirst = leftChildId;
    node.nbShape = 0;

    UpdateNodeBounds(leftChildId, shapes);
    UpdateNodeBounds(rightChildId, shapes);

    //Recurse
    Subdivide(leftChildId, shapes);
    Subdivide(rightChildId, shapes);
}

float BVHTree::EvaluateSAH(BVHNode& node, int axis, float pos, const std::vector<Shape*>& shapes) {

    AABB leftBox, rightBox;
    int leftCount = 0, rightCount = 0;

    for( uint i = 0; i < node.nbShape; i++ ) {

        Shape* shape = shapes[shapeId[node.LeftFirst + i]];
        if (shape->Position[axis] < pos) {
            leftCount++;
            leftBox.grow(shape->GetAABBMin());
            leftBox.grow(shape->GetAABBMax());
        } else {
            rightCount++;
            rightBox.grow(shape->GetAABBMin());
            rightBox.grow(shape->GetAABBMax());
        }
    }
    float cost = leftCount * leftBox.area() + rightCount * rightBox.area();
    return cost > 0 ? cost : FLT_MAX;
}


void BVHTree::IntersectBVH(const Ray& ray, const std::vector<Shape*>& shapes, const uint nodeId, HitPayLoad& payload) const  {

    const BVHNode* node = &nodes[rootNodeId];
    const BVHNode* stack[64]; //Stack to visit node front to back
    uint stackPtr = 0;

    while(true) {
        if (node->nbShape > 0) {
            for (uint i = 0; i < node->nbShape; i++) {
                Shape* shape = shapes[shapeId[node->LeftFirst + i]];
                float t;
            
                if (shape->intersect(ray, t) && t < payload.HitDistance) {
                    payload.HitDistance = t;
                    payload.HitShape = shape;
                }
            }
            if (stackPtr == 0) break;
            else node = stack[--stackPtr];
            continue;
        }

        const BVHNode* child1 = &nodes[node->LeftFirst];
        const BVHNode* child2 = &nodes[node->LeftFirst + 1];

        float dist1 = IntersectAABB(ray, child1->aabbMin, child1->aabbMax, payload.HitDistance);
        float dist2 = IntersectAABB(ray, child2->aabbMin, child2->aabbMax, payload.HitDistance);
        if (dist1 > dist2) { 
            std::swap( dist1, dist2); 
            std::swap( child1, child2);
        } 
        if (dist1 == FLT_MAX) { //miss
            if (stackPtr == 0) break; 
            else node = stack[--stackPtr];
        } else {
            node = child1;
            if (dist2 != FLT_MAX) stack[stackPtr++] = child2;
        }

    }
}


float BVHTree::IntersectAABB(const Ray& ray, const glm::vec3& bmin, const glm::vec3& bmax, float tMax) const {

    glm::vec3 invDir = 1.0f / ray.Direction;
    glm::vec3 t0s = (bmin - ray.Origin) * invDir;
    glm::vec3 t1s = (bmax - ray.Origin) * invDir;

    glm::vec3 tmin = glm::min(t0s, t1s);
    glm::vec3 tmax = glm::max(t0s, t1s);

    float tminMax = glm::max(tmin.x, glm::max(tmin.y, tmin.z));
    float tmaxMin = glm::min(tmax.x, glm::min(tmax.y, tmax.z));

    return (tmaxMin >= tminMax && tminMax < tMax && tmaxMin > 0) ? tminMax : FLT_MAX;
}