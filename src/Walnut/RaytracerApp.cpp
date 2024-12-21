#include "Application.h"
#include "Image.h"
#include "Random.h"
#include "Timer.h"

using namespace Walnut;

class ExampleLayer : public Walnut::Layer {
public:

	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last Render %.3fms", m_LastRenderTime);
		if (ImGui::Button("Render")) Render();
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(.0f, .0f));
		ImGui::Begin("Viewport");

		
		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;

		if (m_Image)
			ImGui::Image((ImTextureID)m_Image->GetDescriptorSet(), {(float)m_Image->GetWidth(), (float)m_Image->GetHeight()});

		ImGui::End();
		ImGui::PopStyleVar();

		Render();
	}

	void Render() {

		Timer timer;

		if (!m_Image || m_ViewportWidth!= m_Image->GetWidth() || m_ViewportHeight != m_Image->GetHeight()) {

			m_Image = std::make_shared<Image>(m_ViewportWidth, m_ViewportHeight, ImageFormat::RGBA);
			delete[] m_ImageData;
			m_ImageData = new uint32_t[m_ViewportWidth * m_ViewportHeight];
		}

		for(uint32_t i = 0; i < m_ViewportHeight*m_ViewportWidth; i++) 
			m_ImageData[i] = Random::UInt() | 0xff000000;

		m_Image->SetData(m_ImageData);

		m_LastRenderTime = timer.ElapsedMillis();
	}

private:
	std::shared_ptr<Image> m_Image;
	u_int32_t* m_ImageData = nullptr;
	uint32_t m_ViewportHeight, m_ViewportWidth;

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


int main(int argc, char** argv) {

	while (Walnut::isRunning()) {

		Walnut::Application* app = Walnut::CreateApplication(argc, argv);
		app->Run();
		delete app;
	}
	return 0;
}
