#include "raytracer/Triangle.h"

#include "imgui/imgui.h"
#include "font/forkawesome.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#include "raytracer/Material.h"
#include "raytracer/Sphere.h"



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

 HitPayLoad Triangle::ClosestHit(const Ray& ray, float hitDistance) {

    HitPayLoad payload;
    payload.HitDistance = hitDistance;

    glm::vec3 origin = ray.Origin;
    payload.WorldPosition = origin + ray.Direction * hitDistance;
    payload.WorldNormal = Normal;
    payload.HitShape = this;

    return payload;
}


bool Triangle::RenderUiSettings(int index, Scene& scene) {

    bool edited = false;

    ImGui::PushID(index);

    std::string objectName = ICON_FK_PLAY " Triangle " + std::to_string(index);

    if (ImGui::TreeNode(objectName.c_str())) {

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


