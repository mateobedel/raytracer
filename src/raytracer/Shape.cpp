#include "raytracer/Shape.h"
#include "imgui.h"
#include "font/forkawesome.h"
#include <string>
#include "raytracer/Scene.h"
#include <algorithm>


Shape::Shape() {}
Shape::Shape(glm::vec3 pos, int i) : Position(pos), MaterialIndex(i) {}

bool Shape::RenderUiMaterial(Scene& scene) {

        bool edited = false;

        const Material& mat = scene.Materials[MaterialIndex];

        ImGui::ColorButton("##xx",ImVec4(mat.Albedo.r, mat.Albedo.g, mat.Albedo.b, 1.0f),ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoPicker,ImVec2(20, 20));
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

        return edited;
    }

bool Shape::RenderDeleteButton(int index, Scene& scene) {

    bool edited = false;
    
       float buttonWidth = 50.0f;
       ImGui::SameLine(ImGui::GetContentRegionMax().x - buttonWidth);
       ImGui::PushID(index);
   
       if (ImGui::Button(ICON_FK_TRASH, ImVec2(buttonWidth, 0))) {
            scene.Shapes.erase(
                std::remove_if(scene.Shapes.begin(), scene.Shapes.end(),
                [this](Shape* shapePtr) {return shapePtr == this;}),
                scene.Shapes.end()
        );
        edited = true;
       }
       
       ImGui::PopID();
       return edited;
}

void Shape::Miss(const Ray& ray, HitPayLoad& payload) { 
    payload.HitDistance = -1.0f;
}