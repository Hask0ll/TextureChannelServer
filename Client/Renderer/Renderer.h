#pragma once
#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm/glm.hpp>
#include <stdexcept>
#include <vector>
#pragma comment(lib, "OpenGl32.lib")


class Renderer
{
public:

    static void Init()
    {
        if (!glfwInit())
            throw std::runtime_error("Unable to initialize GLFW");

        // Set context as OpenGL 4.6 Core, forward compat, with debug depending on build config
        glfwWindowHint(GLFW_CLIENT_API,            GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE,        GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#ifndef NDEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,  GL_TRUE);
#else
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,  GL_FALSE);
#endif
        glfwWindowHint(GLFW_SRGB_CAPABLE,          GL_TRUE);
        glfwWindowHint(GLFW_DOUBLEBUFFER,          GL_TRUE);
        glfwWindowHint(GLFW_RESIZABLE,             GL_FALSE);

        window = glfwCreateWindow(1280, 720, "Procedural Generation Tool", nullptr, nullptr);
        if (!window)
        {
            glfwTerminate();
            throw std::runtime_error("Unable to create GLFW window");
        }

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            glfwDestroyWindow(window);
            glfwTerminate();
            throw std::runtime_error("Unable to initialize GLAD");
        }

        // Setup Dear ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 460");
    }

    static void SetUpdateCallback(const std::function<void()>& callback) {
        applicationUpdateCallback = callback;
    }

    static void Update()
    {
        // Main render loop
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();

            // Start ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Clear the screen
            glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            applicationUpdateCallback();

            // Render ImGui
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }
    }

    static void Cleanup()
    {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    static inline GLFWwindow* window;
    static inline std::function<void()>& applicationUpdateCallback = std::function<void()>();
};
