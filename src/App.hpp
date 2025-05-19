#pragma once
// GLAD
#include "glm/common.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/trigonometric.hpp"
#include <glad/gl.h>
// GLFW (include after glad)
#include <GLFW/glfw3.h>
// GLM
#ifdef _MSVC_VER
#pragma warning(push, 3)
#endif
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#ifdef _MSVC_VER
#pragma warning(pop)
#endif
// ImGUI
#include <imgui.h>

#include <cmath>
#include <cstdlib>

#include "Image.hpp"
#include "ObjectLoader.hpp"
#include "Resources.hpp"
#include "Shader.hpp"
#include "Transform.hpp"

class App {
public:
    App()
    {
    }

    void load_model()
    {
        glGenVertexArrays(1, &this->m_model_vao);
        glBindVertexArray(this->m_model_vao);

        std::vector<Triangle> triangles {};
        if (!load_obj(to_resource_path("teapot.obj"), triangles)) {
            std::exit(EXIT_FAILURE);
        }

        GLuint vertex_buffer;
        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle) * triangles.size(), triangles.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glm::vec3 center {};
        for (const auto& triangle : triangles) {
            center += triangle.v1;
            center += triangle.v2;
            center += triangle.v3;
        }
        center /= triangles.size() * 3.0f;
        this->m_model_transform.with_translation(-center);
        this->m_model_vertex_count = static_cast<GLsizei>(triangles.size() * 3);

        glBindVertexArray(0);
    }

    void init(GLFWwindow* window)
    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        this->m_aspect = static_cast<float>(width) / static_cast<float>(height);

        this->load_model();

        this->m_program_id = Shader::loadShaders(to_resource_path("vertex0.glsl"), to_resource_path("fragment0.glsl"));
        this->m_view_projection_location = glGetUniformLocation(this->m_program_id, "view_projection_matrix");
        this->m_model_matrix_location = glGetUniformLocation(this->m_program_id, "model_matrix");
    }

    void update_camera_pos(GLFWwindow* window, float delta_time)
    {
        constexpr static float DISTANCE_PER_SEC = 1.0f;
        constexpr static float RADIANS_PER_SEC = 2.0f;
        const auto distance = delta_time * DISTANCE_PER_SEC;
        const auto radians = delta_time * RADIANS_PER_SEC;

        // Radius
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            this->m_camera_polar.x -= distance;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            this->m_camera_polar.x += distance;
        }

        // Azimuth
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            this->m_camera_polar.y -= radians;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            this->m_camera_polar.y += radians;
        }

        // Polar angle
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            this->m_camera_polar.z -= radians;
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            this->m_camera_polar.z += radians;
        }

        constexpr static float POLAR_MAX = glm::half_pi<float>() - glm::epsilon<float>();
        this->m_camera_polar.x = glm::max(0.0f, this->m_camera_polar.x);
        this->m_camera_polar.y = std::fmod(this->m_camera_polar.y, glm::tau<float>());
        this->m_camera_polar.z = glm::clamp(this->m_camera_polar.z, -POLAR_MAX, POLAR_MAX);
    }

    glm::vec3 get_camera_pos()
    {
        const auto distance = this->m_camera_polar.x;
        const auto azimuth_angle = this->m_camera_polar.y;
        const auto polar_angle = this->m_camera_polar.z;

        const auto x = distance * glm::cos(polar_angle) * glm::sin(azimuth_angle);
        const auto y = distance * glm::sin(polar_angle);
        const auto z = distance * glm::cos(polar_angle) * glm::cos(azimuth_angle);
        return { x, y, z };
    }

    glm::mat4 get_view_proj_mat()
    {
        constexpr static float FOV = glm::radians(75.0f);
        constexpr static float NEAR = 0.01f;
        constexpr static float FAR = 1000.0f;

        const auto camera_matrix = glm::lookAt(this->get_camera_pos(), glm::vec3 { 0.0f }, glm::vec3 { 0.0f, 1.0f, 0.0f });
        const auto projection = glm::perspective(FOV, this->m_aspect, NEAR, FAR);

        return projection * camera_matrix;
    }

    void draw(GLFWwindow* window, float delta_time)
    {
        this->update_camera_pos(window, delta_time);

        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.6f, 1.0f);

        glUseProgram(this->m_program_id);

        const auto view_proj_mat = this->get_view_proj_mat();
        const auto model_mat = static_cast<glm::mat4>(this->m_model_transform);
        glUniformMatrix4fv(this->m_view_projection_location, 1, GL_FALSE, glm::value_ptr(view_proj_mat));
        glUniformMatrix4fv(this->m_model_matrix_location, 1, GL_FALSE, glm::value_ptr(model_mat));

        // Draw the model.
        glBindVertexArray(this->m_model_vao);
        glDrawArrays(GL_TRIANGLES, 0, this->m_model_vertex_count);

        glBindVertexArray(0);
    }

    void on_key_change(GLFWwindow* window, int key, int, int action, int)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }

    void on_resize(GLFWwindow*, int width, int height)
    {
        glViewport(0, 0, width, height);
        this->m_aspect = static_cast<float>(width) / static_cast<float>(height);
    }

private:
    float m_aspect;
    glm::vec3 m_camera_polar { 2.0f, 0.0f, 0.0f };

    GLuint m_model_vao;
    Transform m_model_transform;
    GLsizei m_model_vertex_count;

    GLuint m_program_id;
    GLuint m_view_projection_location;
    GLuint m_model_matrix_location;
};
