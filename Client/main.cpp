#include <glad/glad.h>
#pragma comment(lib, "OpenGl32.lib")

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <memory>
#include <glm/glm.hpp>
#include <stdexcept>
#include <vector>

#include "Application/Application.h"
#include "Operators/include/Perlin.h"
#include "Renderer/Renderer.h"

#include "Network/NetworkClient.h"


int main(int argc, char** argv) {
    try {
        std::unique_ptr<Application> application = std::make_unique<Application>();
        // NetworkClient client;

        // // Configure le callback pour les messages reçus
        // client.SetMessageCallback([](const std::string& message) {
        //     std::cout << "Message received: " << message << std::endl;
        //     });
        //
        //
        // // Connexion au serveur
        // client.Connect();
        // client.StartListening();
        //
        bool running = true;
		//client.Sending(running);
        while (running) {
            application->Run();
        }

        // client.Disconnect();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
