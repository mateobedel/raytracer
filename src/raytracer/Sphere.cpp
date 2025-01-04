#include "raytracer/Sphere.h"

#include "imgui/imgui.h"
#include "font/forkawesome.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui/imgui.h"
#include <string>

bool Sphere::intersect(const Ray &ray, float& intersectT) const {

    glm::vec3 origin = ray.Origin - Position;

    float a = glm::dot(ray.Direction,ray.Direction);
    float b = 2.0f * glm::dot(origin, ray.Direction);
    float c = glm::dot(origin, origin) - Radius*Radius;

    float delta = b*b - 4.0f * a * c;

    if (delta < 0.0f)
        return false;

    float t = (-b - glm::sqrt(delta)) / (2.0f * a);
    if (t < 0.0f) return false; //behind the ray

    intersectT = t; 
    return true;
}

HitPayLoad Sphere::ClosestHit(const Ray& ray, float hitDistance) {
    
    HitPayLoad payload;
    payload.HitDistance = hitDistance;

    glm::vec3 origin = ray.Origin - Position;
    payload.WorldPosition = origin + ray.Direction * hitDistance;
    payload.WorldNormal = glm::normalize(payload.WorldPosition);
    payload.WorldPosition += Position;
    payload.HitShape = this;

    return payload;
}

bool Sphere::RenderUiSettings(int index, Scene& scene) {

    bool edited = false;

    ImGui::PushID(index);

    std::string objectName = ICON_FK_CIRCLE_O " Sphere " + std::to_string(index);

    if (ImGui::TreeNode(objectName.c_str())) {

        edited |= ImGui::DragFloat3(ICON_FK_ARROWS " Position", glm::value_ptr(Position), .01f);
	    edited |= ImGui::DragFloat(ICON_FK_EXPAND " Radius", &Radius, .01f);

        edited |= RenderUiMaterial(scene);

        ImGui::TreePop();
    }
    ImGui::PopID();

    return edited;
}