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
#include <limits>
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
        if (!load_obj(to_resource_path("cow.obj"), triangles)) {
            std::exit(EXIT_FAILURE);
        }

        GLuint vertex_buffer;
        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle) * triangles.size(), triangles.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glm::vec3 min { std::numeric_limits<float>::max() };
        glm::vec3 max { std::numeric_limits<float>::lowest() };
        glm::vec3 center {};
        for (const auto& triangle : triangles) {
            min = glm::min(min, triangle.v1);
            min = glm::min(min, triangle.v2);
            min = glm::min(min, triangle.v3);

            max = glm::max(max, triangle.v1);
            max = glm::max(max, triangle.v2);
            max = glm::max(max, triangle.v3);

            center += triangle.v1;
            center += triangle.v2;
            center += triangle.v3;
        }
        center /= triangles.size() * 3.0f;
        this->m_model_center = center;
        this->m_model_size = max - min;
        this->m_model_transform.with_translation(-center);
        this->m_model_vertex_count = static_cast<GLsizei>(triangles.size() * 3);

        glBindVertexArray(0);
    }

    void load_texture()
    {
        Image texture { to_resource_path("grid.bmp") };

        // Generate one texture for the dice image.
        glGenTextures(1, &this->m_texture);
        // Bind the newly created texture to the GL_TEXTURE_2D target.
        glBindTexture(GL_TEXTURE_2D, this->m_texture);
        // Upload the image data to the texture.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.width(), texture.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, texture.data());
        // Set the texture options.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Unbind the texture.
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void init(GLFWwindow* window)
    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        this->m_aspect = static_cast<float>(width) / static_cast<float>(height);

        this->load_model();
        this->load_texture();

        this->m_program_id = Shader::loadShaders(to_resource_path("vertex0.glsl"), to_resource_path("fragment0.glsl"));
        this->m_parametrisation_location = glGetUniformLocation(this->m_program_id, "parametrisation");
        this->m_model_center_location = glGetUniformLocation(this->m_program_id, "model_center");
        this->m_model_size_location = glGetUniformLocation(this->m_program_id, "model_size");
        this->m_view_projection_location = glGetUniformLocation(this->m_program_id, "view_projection_matrix");
        this->m_model_matrix_location = glGetUniformLocation(this->m_program_id, "model_matrix");
        this->m_texture_location = glGetUniformLocation(this->m_program_id, "textureSampler");
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
        glUniform1ui(this->m_parametrisation_location, this->m_cylindrical_parametrisation ? 0 : 1);
        glUniform3fv(this->m_model_center_location, 1, glm::value_ptr(this->m_model_center));
        glUniform3fv(this->m_model_size_location, 1, glm::value_ptr(this->m_model_size));
        glUniformMatrix4fv(this->m_view_projection_location, 1, GL_FALSE, glm::value_ptr(view_proj_mat));
        glUniformMatrix4fv(this->m_model_matrix_location, 1, GL_FALSE, glm::value_ptr(model_mat));

        // Set the location of the texture.
        glUniform1i(this->m_texture_location, 0);
        // Bind the texture to the first slot.
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->m_texture);

        // Draw the model.
        glBindVertexArray(this->m_model_vao);
        glDrawArrays(GL_TRIANGLES, 0, this->m_model_vertex_count);

        glBindVertexArray(0);
    }

    void on_key_change(GLFWwindow* window, int key, int, int action, int)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        } else if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
            this->m_cylindrical_parametrisation = true;
        } else if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
            this->m_cylindrical_parametrisation = false;
        }
    }

    void on_resize(GLFWwindow*, int width, int height)
    {
        glViewport(0, 0, width, height);
        this->m_aspect = static_cast<float>(width) / static_cast<float>(height);
    }

private:
    float m_aspect;
    bool m_cylindrical_parametrisation { true };
    glm::vec3 m_camera_polar { 2.0f, 0.0f, 0.0f };

    GLuint m_model_vao;
    glm::vec3 m_model_size;
    glm::vec3 m_model_center;
    Transform m_model_transform;
    GLsizei m_model_vertex_count;

    GLuint m_texture;

    GLuint m_program_id;
    GLuint m_parametrisation_location;
    GLuint m_model_center_location;
    GLuint m_model_size_location;
    GLuint m_view_projection_location;
    GLuint m_model_matrix_location;
    GLuint m_texture_location;
};
