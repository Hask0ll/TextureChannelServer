#include <glad/glad.h>
#pragma comment(lib, "OpenGl32.lib")

#include <GLFW/glfw3.h>

#include <stdexcept>

class Operator
{
    
};

float vertices[15] = {
     0.0f,  0.5f, 1.0f, 0.0f, 0.0f, 
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f
};

unsigned int indices[3] = { 0, 1, 2 };

const char* vsSrc = R"(#version 460 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aCol;
out vec3 vCol;
void main() { gl_Position = vec4(aPos, 0.0, 1.0); vCol = aCol; }
)";

const char* fsSrc = R"(#version 460 core
in vec3 vCol;
layout(location = 0) out vec4 oFragColor;
void main() { oFragColor = vec4(vCol, 1.0); }
)";

int main(int argc, char** argv)
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
#else /* !NDEBUG */
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,  GL_FALSE);
#endif /* !NDEBUG */
    glfwWindowHint(GLFW_SRGB_CAPABLE,          GL_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER,          GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE,             GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 480, "Engine Dev Starter Pack", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        throw std::runtime_error("Unable to create GLFW window");
    }

    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        throw std::runtime_error("Unable to initialize GLAD");
    }

    unsigned int vb, ib, va, vs, fs, sp;
    
    // Create and initialize vertex buffer
    glCreateBuffers(1, &vb);
    glNamedBufferStorage(vb, 15 * sizeof(float), vertices, 0);
    
    // Create and initialize index buffer
    glCreateBuffers(1, &ib);
    glNamedBufferStorage(ib, 3 * sizeof(unsigned int), indices, 0);
    
    // Create and setup vertex array
    glCreateVertexArrays(1, &va);
    glVertexArrayVertexBuffer(va, 0, vb, 0, 5 * sizeof(float));
    glVertexArrayElementBuffer(va, ib);
    
    // Setup position attribute
    glEnableVertexArrayAttrib(va, 0);
    glVertexArrayAttribFormat(va, 0, 2, GL_FLOAT, false, 0);
    glVertexArrayAttribBinding(va, 0, 0);
    
    // Setup color attribute
    glEnableVertexArrayAttrib(va, 1);
    glVertexArrayAttribFormat(va, 1, 3, GL_FLOAT, false, 2 * sizeof(float));
    glVertexArrayAttribBinding(va, 1, 0);

    // Create and compile vertex shader
    vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsSrc, nullptr);
    glCompileShader(vs);

    // Create and compile fragment shader
    fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsSrc, nullptr);
    glCompileShader(fs);

    // Create and link shader program
    sp = glCreateProgram();
    glAttachShader(sp, vs);
    glAttachShader(sp, fs);
    glLinkProgram(sp);

    // Main render loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Clear the screen
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw the triangle
        glUseProgram(sp);
        glBindVertexArray(va);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
        glUseProgram(0);

        glfwSwapBuffers(window);
    }

    // Cleanup
    glDeleteProgram(sp);
    glDeleteShader(fs);
    glDeleteShader(vs);
    glDeleteVertexArrays(1, &va);
    glDeleteBuffers(1, &ib);
    glDeleteBuffers(1, &vb);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
