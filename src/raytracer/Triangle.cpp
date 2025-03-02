#include "raytracer/Triangle.h"

#include "imgui/imgui.h"
#include "font/forkawesome.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#include "raytracer/Material.h"
#include "raytracer/Sphere.h"


Triangle::Triangle() {
    Vertex V0 = {.Position = glm::vec3(-1,0,0)};
    Vertex V1 = {.Position = glm::vec3(1,0,0)};
    Vertex V2 = {.Position = glm::vec3(0,2,0)};

    V[0] = V0; V[1] = V1; V[2] = V2;
    onVertexChange();
}


Triangle::Triangle(Vertex V0, Vertex V1, Vertex V2) {
    V[0] = V0; V[1] = V1; V[2] = V2;
    E[0] = V1.Position - V0.Position; E[1] = V2.Position - V0.Position; E[2] = V2.Position - V1.Position;

    Normal = glm::normalize(glm::cross(E[0], E[1])); 
    Position = (V[0].Position + V[1].Position + V[2].Position)*0.3333f;
    dPlane = - glm::dot(Normal, V[0].Position);
}

void Triangle::onVertexChange() {
    Position = (V[0].Position + V[1].Position + V[2].Position)*0.3333f;
    E[0] = V[1].Position - V[0].Position; E[1] = V[2].Position - V[0].Position; E[2] = V[2].Position - V[1].Position;
    Normal = glm::normalize(glm::cross(E[0], E[1])); 
    dPlane = - glm::dot(Normal, V[0].Position);
}


glm::vec3 Triangle::GetAABBMin() const {
    return glm::min(glm::min(V[0].Position, V[1].Position), V[2].Position);
}

glm::vec3 Triangle::GetAABBMax() const {
    return glm::max(glm::max(V[0].Position, V[1].Position), V[2].Position);
}



 bool Triangle::intersect(const Ray& ray, float& intersectT) const {

    //Back Face Culling
    // if (glm::dot(ray.Direction, Normal) > 0)
    //     return false; 


    //Parallel
    float normalDirectionDot = dot(ray.Direction, Normal);
    if(glm::abs(normalDirectionDot) < .0001) return false;
    
    //Intersection t of the ray (Origin + Direction*t) with the plane
    float intersectPlaneT = -(glm::dot(Normal, ray.Origin) + dPlane) / normalDirectionDot;

    //Triangle is behind the ray
    if (intersectPlaneT < 0.0f) return false; 

    //Intersection point
    glm::vec3 intersectionPoint = ray.Origin + intersectPlaneT*ray.Direction;

    //Outside edge 0
    if (glm::dot(Normal, glm::cross(E[0], intersectionPoint - V[0].Position)) < 0) 
        return false; 
 
    //Outside edge1
    if (glm::dot(Normal, glm::cross(-E[1] , intersectionPoint - V[2].Position)) < 0) 
        return false; 

    //Outside edge 2
    if (glm::dot(Normal, glm::cross(E[2], intersectionPoint - V[1].Position)) < 0) 
        return false; 

    intersectT = intersectPlaneT;

    
    return true;
 }

 void Triangle::ClosestHit(const Ray& ray, HitPayLoad& payload) {

    glm::vec3 origin = ray.Origin;
    payload.WorldPosition = origin + ray.Direction * payload.HitDistance;
    payload.WorldNormal = Normal;
}

bool Triangle::RenderUiSettings(int index, Scene& scene) {

    bool edited = false;

    ImGui::PushID(index);

    std::string objectName = ICON_FK_PLAY " Triangle " + std::to_string(index);
    bool isOpen = ImGui::TreeNode(objectName.c_str());

    edited |= RenderDeleteButton(index, scene);

    if (isOpen) {

        glm::vec3 NewPosition = Position;
        
        if (ImGui::DragFloat3(ICON_FK_ARROWS " Position", glm::value_ptr(NewPosition), .01f)){
            glm::vec3 PositionDif = NewPosition - Position;
            V[0].Position += PositionDif;
            V[1].Position += PositionDif;
            V[2].Position += PositionDif;
            Position = NewPosition;
            onVertexChange(); 
            edited = true;
        }

        
        edited |= RenderUiMaterial(scene);

        if (ImGui::TreeNode(ICON_FK_DOT_CIRCLE_O " Vertex")) {
            for(int i= 0; i < 3; i++) {
     
                std::string vertexName = ICON_FK_DOT_CIRCLE_O " Vertex " + std::to_string(i);

                ImGui::PushID(i);
                if (ImGui::DragFloat3(vertexName.c_str(), glm::value_ptr(V[i].Position), .01f)) {
                    onVertexChange();
                    edited = true;
                }
                ImGui::PopID();
            }
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }
    ImGui::PopID();

    return edited;
}


