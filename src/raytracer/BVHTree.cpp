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
    node.aabbMax = glm::vec3(-FLT_MAX);

    for (int first = node.LeftFirst, i = 0; i < node.nbShape; i++) {

        int leafShapeId = shapeId[first + i];
        Shape* leafShape = shapes[leafShapeId];

        node.aabbMin = glm::min(node.aabbMin, leafShape->GetAABBMin());
        node.aabbMax = glm::max(node.aabbMax, leafShape->GetAABBMax());
    }
}


float BVHTree::FindBestSplitPlane(BVHNode& node, int& axis, float& splitPos, const std::vector<Shape*>& shapes) {

    float bestCost = FLT_MAX;

    for(int a = 0; a < 3; a++ ) {

        //Find BoundsMin/boundsMax with centroids (Shape.Position)
        float boundsMin = FLT_MAX, boundsMax = -FLT_MAX;
        for (int i = 0; i < node.nbShape; i++) {
            Shape* shape = shapes[shapeId[node.LeftFirst + i]];
            boundsMin = glm::min(boundsMin, shape->Position[a]);
            boundsMax = glm::max(boundsMax, shape->Position[a]);
        }
        if (boundsMin == boundsMax) continue;

        //Populate the BINS
        Bin bin[BINS];
        float scale = BINS / (boundsMax - boundsMin);
        for (uint i = 0; i < node.nbShape; i++) {

            Shape* shape = shapes[shapeId[node.LeftFirst + i]];
            int binIdx = glm::min(BINS-1, (int)((shape->Position[a] - boundsMin)*scale));
            bin[binIdx].nbShape++;
            bin[binIdx].bounds.grow(shape->GetAABBMin());
            bin[binIdx].bounds.grow(shape->GetAABBMax());
        }

        
        //Gather data of in-between planes 
        float leftArea[BINS - 1], rightArea[BINS - 1];
        int leftCount[BINS - 1], rightCount[BINS - 1];
        AABB leftBox, rightBox;
        int leftSum = 0, rightSum = 0;
        for (int i = 0; i < BINS - 1; i++) {
            leftSum += bin[i].nbShape;
            leftCount[i] = leftSum;
            leftBox.grow(bin[i].bounds);
            leftArea[i] = leftBox.area();
            rightSum += bin[BINS - 1 - i].nbShape;
            rightCount[BINS - 2 - i] = rightSum;
            rightBox.grow(bin[BINS - 1 - i].bounds);
            rightArea[BINS - 2 - i] = rightBox.area();
        }

        //Calc SAH cost of in-between planes
        scale = (boundsMax - boundsMin)/BINS;
        for (uint i = 0; i < BINS-1; i++) {
            float planeCost = leftCount[i] * leftArea[i] + rightCount[i] * rightArea[i];
            if (planeCost < bestCost) {
                axis = a;
                splitPos = boundsMin + scale * (i + 1);
                bestCost = planeCost;
            }
        }
    }
    return bestCost;
}


float BVHTree::CalcNodeCost(BVHNode& node) {
    glm::vec3 e = node.aabbMax - node.aabbMin; 
    float surfaceArea = e.x * e.y + e.y * e.z + e.z * e.x;
    return node.nbShape * surfaceArea;
}


void BVHTree::Subdivide(int nodeId, const std::vector<Shape*>& shapes) {

    BVHNode& node = nodes[nodeId];

    // Determine split axis using SAH
    int axis;
    float splitPos;
    float splitCost = FindBestSplitPlane(node, axis, splitPos, shapes);

    // Abort if split does not reduce cost
    float nosplitCost = CalcNodeCost(node);
    if (splitCost >= nosplitCost) return;


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