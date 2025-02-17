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


int main(int argc, char** argv)
{
    std::unique_ptr<Application> application = std::make_unique<Application>();
    application->Run();
    // Créer l'opérateur de Perlin noise
}
