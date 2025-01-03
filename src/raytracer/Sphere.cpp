#include "raytracer/Sphere.h"

#include "imgui/imgui.h"
#include "font/forkawesome.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

float Sphere::intersect(const Ray &ray) const {

    glm::vec3 origin = ray.Origin - Position;

    float a = glm::dot(ray.Direction,ray.Direction);
    float b = 2.0f * glm::dot(origin, ray.Direction);
    float c = glm::dot(origin, origin) - Radius*Radius;

    float delta = b*b - 4.0f * a * c;

    if (delta < 0.0f)
        return __FLT_MIN__;

    float closestT = (-b - glm::sqrt(delta)) / (2.0f * a); //Smaller solution
    return closestT;
}

bool Sphere::RenderUiSettings(int index, Scene& scene) {

    bool edited = false;

    ImGui::PushID(index);

    std::string objectName = ICON_FK_CIRCLE_O " Sphere " + std::to_string(index);

    if (ImGui::TreeNode(objectName.c_str())) {

        edited |= ImGui::DragFloat3(ICON_FK_ARROWS " Position", glm::value_ptr(Position), .01f);
	    edited |= ImGui::DragFloat(ICON_FK_EXPAND " Radius", &Radius, .01f);

        std::shared_ptr<Material> mat = scene.Materials[MaterialIndex];

        ImGui::ColorButton("##xx",ImVec4(mat->Albedo.r, mat->Albedo.g, mat->Albedo.b, 1.0f),ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoPicker,ImVec2(20, 20));
        ImGui::SameLine(); 

        std::vector<const char*> materialNames;
        for (int i = 0; i < scene.Materials.size(); ++i) {
            char* name = new char[32]; 
            snprintf(name, 32, "Material  %d", i);
            materialNames.push_back(name);
        }
        ImGui::SetNextItemWidth(ImGui::CalcItemWidth() - (20.0f+ImGui::GetStyle().ItemSpacing.x));
        edited |= ImGui::Combo(ICON_FK_ADJUST " Material", &MaterialIndex, materialNames.data(), materialNames.size());

        for (auto name : materialNames) delete[] name;

        ImGui::TreePop();
    }
    ImGui::PopID();

    return edited;
}