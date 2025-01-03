#include "raytracer/Material.h"

#include "imgui/imgui.h"
#include "font/forkawesome.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>



bool Material::RenderUiSettings(int index) {

    bool edited = false;
    ImGui::PushID(index);

    ImGui::ColorButton("",ImVec4(Albedo.r, Albedo.g, Albedo.b, 1.0f),ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoPicker,ImVec2(20, 20));
    ImGui::SameLine(); 

    std::string matName = ICON_FK_ADJUST " Mat " + std::to_string(index);
    if (ImGui::TreeNode(matName.c_str())) {

        edited |= ImGui::ColorEdit3(ICON_FK_TINT " Albedo", glm::value_ptr(Albedo));
        edited |= ImGui::SliderFloat(ICON_FK_CERTIFICATE " Roughness", &Roughness, .0f, 1.0f);
        edited |= ImGui::SliderFloat(ICON_FK_SQUARE " Metallic", &Metallic, .0f, 1.0f);
        edited |= ImGui::ColorEdit3(ICON_FK_TINT " Emission Color", glm::value_ptr(EmissionColor));
        edited |= ImGui::DragFloat(ICON_FK_LIGHTBULB_O "Emission Power", &EmmissionPower, .05f, 0.0f, FLT_MAX);
        ImGui::TreePop();
    }

    ImGui::PopID();
    return edited;
}