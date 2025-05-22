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
#include <sstream>

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

    void init(GLFWwindow* window)
    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        this->init_program();
        this->init_fullscreen_quad();
        this->upload_triangles();
        this->upload_camera(width, height);
    }

    void init_program()
    {
        this->m_program_id = Shader::loadShaders(to_resource_path("vertex0.glsl"), to_resource_path("fragment0.glsl"));
        this->m_camera_position_location = glGetUniformLocation(this->m_program_id, "camera.position");
        this->m_camera_near_plane_p0_location = glGetUniformLocation(this->m_program_id, "camera.near_plane_p0");
        this->m_camera_near_plane_right_location = glGetUniformLocation(this->m_program_id, "camera.near_plane_right");
        this->m_camera_near_plane_up_location = glGetUniformLocation(this->m_program_id, "camera.near_plane_up");
        this->m_camera_pixel_size_location = glGetUniformLocation(this->m_program_id, "camera.pixel_size");
    }

    void init_fullscreen_quad()
    {
        glGenVertexArrays(1, &this->m_fullscreen_quad_vao);
        glBindVertexArray(this->m_fullscreen_quad_vao);

        constexpr static glm::vec2 VERTICES[] {
            glm::vec2 { -1.0f, -1.0f },
            glm::vec2 { 1.0f, -1.0f },
            glm::vec2 { 1.0f, 1.0f },
            glm::vec2 { -1.0f, 1.0f },
        };

        GLuint buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void upload_triangles()
    {
        constexpr static Triangle TRIANGLES[] {
            {
                glm::vec3 { -2.5f, -0.5f, -0.5f },
                glm::vec3 { -1.5f, -0.5f, -0.5f },
                glm::vec3 { -2.0f, 0.5f, -0.5f },
            },
            {
                glm::vec3 { 1.0f, -0.5f, 0.5f },
                glm::vec3 { 2.0f, -0.5f, 0.5f },
                glm::vec3 { 1.5f, 0.5f, 0.5f },
            },
            {
                glm::vec3 { -0.5f, 0.5f, -0.5f },
                glm::vec3 { 0.5f, 1.5f, 0.5f },
                glm::vec3 { -0.5f, 1.5f, -0.5f },
            },
            {
                glm::vec3 { 0.5f, -0.5f, -0.5f },
                glm::vec3 { 0.5f, 0.5f, 0.5f },
                glm::vec3 { 0.5f, 0.5f, -0.5f },
            },
            {
                glm::vec3 { -0.5f, -2.0f, -0.5f },
                glm::vec3 { 0.5f, -0.5f, 0.5f },
                glm::vec3 { 0.0f, 0.5f, 0.0f },
            }
        };

        glUseProgram(this->m_program_id);

        // Set the number of triangles.
        constexpr unsigned int num_triangles = sizeof(TRIANGLES) / sizeof(Triangle);
        GLint num_triangles_position = glGetUniformLocation(this->m_program_id, "num_triangles");
        glUniform1ui(num_triangles_position, num_triangles);

        // Set the individual triangles.
        for (std::size_t i = 0; i < num_triangles; i++) {
            std::ostringstream element_stream {};
            element_stream << "triangles[" << i << "]";
            std::string v0_name = element_stream.str() + ".v0";
            std::string v1_name = element_stream.str() + ".v1";
            std::string v2_name = element_stream.str() + ".v2";

            GLint v0_position = glGetUniformLocation(this->m_program_id, v0_name.c_str());
            GLint v1_position = glGetUniformLocation(this->m_program_id, v1_name.c_str());
            GLint v2_position = glGetUniformLocation(this->m_program_id, v2_name.c_str());

            glUniform3fv(v0_position, 1, glm::value_ptr(TRIANGLES[i].v1));
            glUniform3fv(v1_position, 1, glm::value_ptr(TRIANGLES[i].v2));
            glUniform3fv(v2_position, 1, glm::value_ptr(TRIANGLES[i].v3));
        }
    }

    void upload_camera(int width, int height)
    {
        constexpr static glm::vec3 POSITION { 0.0f, 0.0f, 3.0f };
        constexpr static glm::vec3 FORWARDS { 0.0f, 0.0f, -1.0f };
        constexpr static glm::vec3 UP { 0.0f, 1.0f, 0.0f };
        constexpr static float FOV = glm::radians(78.0f);
        constexpr static float NEAR_PLANE = 1.0f;

        const auto width_f = static_cast<float>(width);
        const auto height_f = static_cast<float>(height);
        const auto aspect_ratio = height_f / width_f;

        float near_plane_width = 2.0f * NEAR_PLANE * glm::tan(FOV / 2.0f);
        float near_plane_height = near_plane_width * aspect_ratio;

        float pixel_width = near_plane_width / width_f;
        float pixel_height = near_plane_height / height_f;
        glm::vec2 pixel_size { pixel_width, pixel_height };

        glm::vec3 near_plane_center = POSITION + NEAR_PLANE * FORWARDS;
        glm::vec3 camera_right = glm::normalize(glm::cross(FORWARDS, UP));

        glm::vec3 near_plane_center_offset_right = near_plane_width / 2.0f * camera_right;
        glm::vec3 near_plane_center_offset_up = near_plane_height / 2.0f * UP;
        glm::vec3 near_plane_center_offset = near_plane_center_offset_right + near_plane_center_offset_up;
        glm::vec3 near_plane_bottom_left = near_plane_center - near_plane_center_offset;

        glUseProgram(this->m_program_id);
        glUniform3fv(this->m_camera_position_location, 1, glm::value_ptr(POSITION));
        glUniform3fv(this->m_camera_near_plane_p0_location, 1, glm::value_ptr(near_plane_bottom_left));
        glUniform3fv(this->m_camera_near_plane_right_location, 1, glm::value_ptr(camera_right));
        glUniform3fv(this->m_camera_near_plane_up_location, 1, glm::value_ptr(UP));
        glUniform2fv(this->m_camera_pixel_size_location, 1, glm::value_ptr(pixel_size));
    }

    void draw(GLFWwindow*, float)
    {
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glUseProgram(this->m_program_id);
        glBindVertexArray(this->m_fullscreen_quad_vao);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
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
        this->upload_camera(width, height);
    }

private:
    GLuint m_fullscreen_quad_vao;

    GLuint m_program_id;
    GLuint m_camera_position_location;
    GLuint m_camera_near_plane_p0_location;
    GLuint m_camera_near_plane_right_location;
    GLuint m_camera_near_plane_up_location;
    GLuint m_camera_pixel_size_location;
};
