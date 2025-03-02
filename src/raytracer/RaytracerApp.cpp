#include "Walnut/Application.h"
#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"
#include "Walnut/Camera.h"
#include "Walnut/Input.h"
#include "font/forkawesome.h"
#include "happly/happly.h"

#include "raytracer/Renderer.h"
#include "raytracer/Sphere.h"
#include "raytracer/Triangle.h"

#include <glm/gtc/type_ptr.hpp>

using namespace Walnut;

class ExampleLayer : public Walnut::Layer {

public:

	ExampleLayer()
		: m_Camera(45.0f, 0.1f, 100.0f) {

		Material pinkSphere;
		pinkSphere.Albedo = {1.0f, 0.0f, 1.0f};
		pinkSphere.Roughness = 0.0f;
		m_Scene.Materials.push_back(pinkSphere);
		
		Material blueSphere;
		blueSphere.Albedo = {0.0f, 0.0f, 1.0f};
		blueSphere.Roughness = .1f;
		m_Scene.Materials.push_back(blueSphere);


		Sphere* sphere = new Sphere();
		sphere->Position = {.0f, .0f, .0f};
		sphere->Radius = 1.0f;
		sphere->MaterialIndex = 0;
		m_Scene.Shapes.push_back(sphere);

		Sphere* sphere2 = new Sphere();
		sphere2->Position = {.0f, -201.0f, .0f};
		sphere2->Radius = 200.0f;
		sphere2->MaterialIndex = 1;
		m_Scene.Shapes.push_back(sphere2);

		// happly::PLYData testPly("ply/cube.ply");
		// std::vector<std::array<double, 3>> vertexPositions = testPly.getVertexPositions();
		// std::vector<std::vector<size_t>> faceIndices = testPly.getFaceIndices<size_t>();

		// for (const auto& face : faceIndices) {
			
		// 	Vertex v0 { glm::vec3(vertexPositions[face[0]][0], vertexPositions[face[0]][1], vertexPositions[face[0]][2]), glm::vec3(0.0f) };
		// 	Vertex v1 { glm::vec3(vertexPositions[face[1]][0], vertexPositions[face[1]][1], vertexPositions[face[1]][2]), glm::vec3(0.0f) };
		// 	Vertex v2 { glm::vec3(vertexPositions[face[2]][0], vertexPositions[face[2]][1], vertexPositions[face[2]][2]), glm::vec3(0.0f) };
			
		// 	Triangle* triangle = new Triangle(v0, v1, v2);
		// 	m_Scene.Shapes.push_back(triangle);
		// }

		m_Scene.bvh.BuildBVH(m_Scene.Shapes);

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
        if (ObjectTabRender()) {
			m_Renderer.ResetFrameIndex();
			m_Scene.bvh.BuildBVH(m_Scene.Shapes);
		}
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
		for (size_t i = 0; i < m_Scene.Shapes.size(); i++) 
			edited |= m_Scene.Shapes[i]->RenderUiSettings(i, m_Scene);
		
		
		ImGui::Separator();

		//Selection de l'objet à ajouter
		static int currentObjectIndex = 0;
		static const char optionsString[] =
			ICON_FK_PLAY " Triangle\0"
			ICON_FK_CIRCLE_O " Sphere\0"  
        	"\0"; 

		ImGui::Combo("##", &currentObjectIndex, optionsString);

		//Ajout de l'objet
		ImGui::SameLine();
		if (ImGui::Button("Add",ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			Shape* shape;
			switch(currentObjectIndex) {
				case 0: shape = new Triangle(); break;
				case 1: shape = new Sphere(); break;
			}
			m_Scene.Shapes.push_back(shape);
			edited = true;
		}
	

		ImGui::End();
		return edited;
	}

	bool MaterialTabRender() {

		ImGui::Begin(ICON_FK_ADJUST " Materials");
		bool edited = false;
		for(size_t i = 0; i < m_Scene.Materials.size(); i++) 
			edited |= m_Scene.Materials[i].RenderUiSettings(i);
		
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

			if (ImGui::MenuItem("Exit")) {
				app->Close();
			}
			
			ImGui::EndMenu();
		}
	});
	return app;
}

