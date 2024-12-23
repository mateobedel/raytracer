#include "Walnut/Application.h"

int main(int argc, char** argv) {

	while (Walnut::isRunning()) {

		Walnut::Application* app = Walnut::CreateApplication(argc, argv);
		app->Run();
		delete app;
	}
	return 0;
}