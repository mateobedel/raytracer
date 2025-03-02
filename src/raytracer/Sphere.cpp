#include "raytracer/Sphere.h"

#include "imgui/imgui.h"
#include "font/forkawesome.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui/imgui.h"
#include <string>


Sphere::Sphere() {}

Sphere::Sphere(glm::vec3 p, int i, float r) : Shape(p,i), Radius(r) {}

glm::vec3 Sphere::GetAABBMin() const {
    return Position - glm::vec3(Radius);
}

glm::vec3 Sphere::GetAABBMax() const {
    return Position + glm::vec3(Radius);
}

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

void Sphere::ClosestHit(const Ray& ray, HitPayLoad& payload) {

    glm::vec3 origin = ray.Origin - Position;
    payload.WorldPosition = origin + ray.Direction * payload.HitDistance;
    payload.WorldNormal = glm::normalize(payload.WorldPosition);
    payload.WorldPosition += Position;
}

bool Sphere::RenderUiSettings(int index, Scene& scene) {

    bool edited = false;

    ImGui::PushID(index);

    std::string objectName = ICON_FK_CIRCLE_O " Sphere " + std::to_string(index);
    bool isOpen = ImGui::TreeNode(objectName.c_str());

    edited |= RenderDeleteButton(index, scene);

    if (isOpen) {

        edited |= ImGui::DragFloat3(ICON_FK_ARROWS " Position", glm::value_ptr(Position), .01f);
	    edited |= ImGui::DragFloat(ICON_FK_EXPAND " Radius", &Radius, .01f);

        edited |= RenderUiMaterial(scene);

        ImGui::TreePop();
    }
    ImGui::PopID();

    return edited;
}