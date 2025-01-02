#include "Walnut/Application.h"
#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"
#include "Walnut/Camera.h"
#include "Walnut/Input.h"
#include "font/forkawesome.h"

#include "raytracer/Renderer.h"

#include <glm/gtc/type_ptr.hpp>

using namespace Walnut;

class ExampleLayer : public Walnut::Layer {

public:

	ExampleLayer()
		: m_Camera(45.0f, 0.1f, 100.0f) {

		Material& pinkSphere = m_Scene.Materials.emplace_back();
		pinkSphere.Albedo = {1.0f, 0.0f, 1.0f};
		pinkSphere.Roughness = 0.0f;

		Material& blueSphere = m_Scene.Materials.emplace_back();
		blueSphere.Albedo = {0.0f, 0.0f, 1.0f};
		blueSphere.Roughness = .1f;

		Sphere sphere;
		sphere.Position = {.0f, .0f, .0f};
		sphere.Radius = 1.0f;
		sphere.MaterialIndex = 0;
		m_Scene.Spheres.push_back(sphere);

		Sphere sphere2;
		sphere2.Position = {.0f, -201.0f, .0f};
		sphere2.Radius = 200.0f;
		sphere2.MaterialIndex = 1;
		m_Scene.Spheres.push_back(sphere2);
	}

	virtual void OnUpdate(float ts) override {
		if (m_Camera.OnUpdate(ts))
			m_Renderer.ResetFrameIndex();
	}


	virtual void OnUIRender() override
	{
		ImGui::GetStyle().FrameRounding = 5.0f;

		ImGui::Begin(ICON_FK_COG "Settings");

			ImGui::Text("Last Render %.3fms (%d FPS)", m_LastRenderTime, (1000/(1+(int)m_LastRenderTime)));

			if (ImGui::Button("Render")) Render();
			ImGui::SameLine();
			if (ImGui::Button("Reset")) m_Renderer.ResetFrameIndex();

			ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);
			

		ImGui::End();

		//Tabs
        if (ObjectTabRender()) m_Renderer.ResetFrameIndex();
		if (MaterialTabRender()) m_Renderer.ResetFrameIndex();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(.0f, .0f));
		ImGui::Begin(ICON_FK_PICTURE_O " Viewport");

			m_ViewportWidth = ImGui::GetContentRegionAvail().x;
			m_ViewportHeight = ImGui::GetContentRegionAvail().y;

			//Render image
			auto image = m_Renderer.GetFinalImage();
			if (image) ImGui::Image((ImTextureID)image->GetDescriptorSet(), {(float)image->GetWidth(), (float)image->GetHeight()}, ImVec2(0,1), ImVec2(1,0));

			//Camera control
			if (ImGui::IsMouseClicked(0) && !ImGui::IsMouseDragging(0) && ImGui::IsWindowHovered()) 
				m_Camera.SetCameraControl(true);
			
			if (Input::IsKeyDown(KeyCode::Escape))
				m_Camera.SetCameraControl(false);
			
	
		ImGui::End();
		ImGui::PopStyleVar();

		Render();
	}

	void Render() {

		Timer timer;

		//resize
		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render(m_Scene, m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}

private:       
                
	bool ObjectTabRender() {
		ImGui::Begin(ICON_FK_CUBES " Scene");

		bool edited = false;

		
		for(size_t i = 0; i < m_Scene.Spheres.size(); i++) {

			ImGui::PushID(i);
			std::string objectName = ICON_FK_CIRCLE_O " Sphere " + std::to_string(i);

			if (ImGui::TreeNode(objectName.c_str())) {

				edited |= ImGui::DragFloat3(ICON_FK_ARROWS " Position", glm::value_ptr(m_Scene.Spheres[i].Position), .01f);
				edited |= ImGui::DragFloat(ICON_FK_EXPAND " Radius", &m_Scene.Spheres[i].Radius, .01f);

				Material& material = m_Scene.Materials[m_Scene.Spheres[i].MaterialIndex];
				ImGui::ColorButton("##xx",ImVec4(material.Albedo.r, material.Albedo.g, material.Albedo.b, 1.0f),ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoPicker,ImVec2(20, 20));
				ImGui::SameLine(); 

				std::vector<const char*> materialNames;
				for (int i = 0; i < m_Scene.Materials.size(); ++i) {
					char* name = new char[32]; 
					snprintf(name, 32, "Material  %d", i);
					materialNames.push_back(name);
				}
				ImGui::SetNextItemWidth(ImGui::CalcItemWidth() - (20.0f+ImGui::GetStyle().ItemSpacing.x));
				edited |= ImGui::Combo(ICON_FK_ADJUST " Material", &m_Scene.Spheres[i].MaterialIndex, materialNames.data(), materialNames.size());

				for (auto name : materialNames) delete[] name;

				
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
		

		ImGui::Separator();


		if (ImGui::Button("+",ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			Sphere sphere;
			m_Scene.Spheres.push_back(sphere);
			edited = true;
		}

		ImGui::End();
		return edited;
	}

	bool MaterialTabRender() {

		ImGui::Begin(ICON_FK_ADJUST " Materials");
		bool edited = false;
		for(size_t i = 0; i < m_Scene.Materials.size(); i++) {
	
			ImGui::PushID(i);

			Material& material = m_Scene.Materials[i];

			
    		ImGui::ColorButton("",ImVec4(material.Albedo.r, material.Albedo.g, material.Albedo.b, 1.0f),ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoPicker,ImVec2(20, 20));
			ImGui::SameLine(); 

			std::string matName = ICON_FK_ADJUST " Mat " + std::to_string(i);
			if (ImGui::TreeNode(matName.c_str())) {

				edited |= ImGui::ColorEdit3(ICON_FK_TINT " Albedo", glm::value_ptr(material.Albedo));
				edited |= ImGui::SliderFloat(ICON_FK_CERTIFICATE " Roughness", &material.Roughness, .0f, 1.0f);
				edited |= ImGui::SliderFloat(ICON_FK_SQUARE " Metallic", &material.Metallic, .0f, 1.0f);
				edited |= ImGui::ColorEdit3(ICON_FK_TINT " Emission Color", glm::value_ptr(material.EmissionColor));
				edited |= ImGui::DragFloat(ICON_FK_LIGHTBULB_O "Emission Power", &material.EmmissionPower, .05f, 0.0f, FLT_MAX);
				ImGui::TreePop();
			}
			ImGui::PopID();
		}

		ImGui::Separator();

		if (ImGui::Button("+",ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			Material mat;
			m_Scene.Materials.push_back(mat);
		}

		ImGui::End();
		return edited;
	}

	Renderer m_Renderer;
	Camera m_Camera;
	Scene m_Scene;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	float m_LastRenderTime = .0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv) {

	Walnut::ApplicationSpecification spec;
	spec.Name = "Raytracer";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();

	app->SetMenubarCallback([app]() {

		if (ImGui::BeginMenu("File")) {

			if (ImGui::MenuItem("Exit")) 
				app->Close();
			
			ImGui::EndMenu();
		}
	});
	return app;
}

