#include "App.hpp"

// ImGUI backend
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <cstdlib>
#include <iostream>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

[[noreturn]]
void exit_error(const char* error)
{
    std::cerr << error << std::endl;
    exit(EXIT_FAILURE);
}

int main()
{
    if (glfwInit() == GLFW_FALSE) {
        exit_error("Could not initialize GLFW");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    constexpr int WIDTH = 800;
    constexpr int HEIGHT = 600;
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Computer Graphics", NULL, NULL);
    glfwMakeContextCurrent(window);
    if (window == nullptr) {
        exit_error("Could not create GLFW window");
    }
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        exit_error("Failed to initialize OpenGL context");
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    App app {};
    glfwSetWindowUserPointer(window, &app);
    app.init(window);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // ImGui prepare
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        app.draw(window);

        // ImGui render
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->on_key_change(window, key, scancode, action, mode);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->on_resize(window, width, height);
    }
}
