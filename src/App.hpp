#pragma once
// GLAD
#include "glm/fwd.hpp"
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

    void initialize_stem()
    {
        glGenVertexArrays(1, &this->m_stem_vao);
        glBindVertexArray(this->m_stem_vao);

        constexpr static float VERTEX_POSITIONS[] = {
            -0.1f, -1.0f, 0.0f, // bottom-left
            0.1f, -1.0f, 0.0f, // bottom-right
            0.1f, 0.4f, 0.0f, // top-right
            -0.1f, 0.4f, 0.0f, // top-left
        };

        GLuint vertex_buffer;
        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX_POSITIONS), VERTEX_POSITIONS, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindVertexArray(0);
    }

    void initialize_rotor()
    {
        glGenVertexArrays(1, &this->m_rotor_vao);
        glBindVertexArray(this->m_rotor_vao);

        constexpr static float VERTEX_POSITIONS[] = {
            -0.2f, -0.6f, 0.0f, // bottom-left
            0.2f, -0.6f, 0.0f, // bottom-right
            0.0f, 0.0f, 0.0f, // middle
        };

        GLuint vertex_buffer;
        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX_POSITIONS), VERTEX_POSITIONS, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindVertexArray(0);
    }

    void init(GLFWwindow*)
    {
        this->initialize_stem();
        this->initialize_rotor();

        this->m_program_id = Shader::loadShaders(to_resource_path("vertex0.glsl"), to_resource_path("fragment0.glsl"));
        this->m_model_color_location = glGetUniformLocation(this->m_program_id, "model_color");
        this->m_model_matrix_location = glGetUniformLocation(this->m_program_id, "model_matrix");
    }

    void draw(GLFWwindow*, float delta_time)
    {
        // The rotors complete one rotation every four seconds.
        // Assuming a constant angular rotation speed, we can compute the rotation angle by scaling
        // it to one second and multiplying the number of seconds that have passed.
        constexpr static float ROTATIONS_PER_SEC = 0.25f;
        constexpr static float RADIANS_PER_SEC = glm::tau<float>() * ROTATIONS_PER_SEC;
        const auto rotation_angle = RADIANS_PER_SEC * delta_time;
        this->m_rotor_rotation = std::fmod(this->m_rotor_rotation + rotation_angle, glm::tau<float>());

        // We have four rotors, each with a different rotation angle.
        // The angle of the rotor i is: (m_rotor_rotation + i * pi/2) % 2pi
        const auto rotor_0_angle = this->m_rotor_rotation;
        const auto rotor_1_angle = std::fmod(rotor_0_angle + glm::half_pi<float>(), glm::tau<float>());
        const auto rotor_2_angle = std::fmod(rotor_1_angle + glm::half_pi<float>(), glm::tau<float>());
        const auto rotor_3_angle = std::fmod(rotor_2_angle + glm::half_pi<float>(), glm::tau<float>());

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.6f, 1.0f);

        glUseProgram(this->m_program_id);

        // Draw the stem
        glBindVertexArray(this->m_stem_vao);

        // The stem is already positioned correctly in the global coordinate system.
        // We send the identity matrix so that we don't modify its position.
        glm::mat4 trans_matrix = glm::identity<glm::mat4>();
        glUniform3f(this->m_model_color_location, 0.8f, 0.0f, 0.0f);
        glUniformMatrix4fv(this->m_model_matrix_location, 1, GL_FALSE, glm::value_ptr(trans_matrix));
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // Draw the 4 rotors.
        glBindVertexArray(this->m_rotor_vao);
        glUniform3f(this->m_model_color_location, 0.8f, 0.8f, 0.0f);

        // Rotor 0
        trans_matrix = glm::translate(glm::identity<glm::mat4>(), glm::vec3{ 0.0f, 0.2f, 0.0f });
        trans_matrix = glm::rotate(trans_matrix, rotor_0_angle, glm::vec3{ 0.0f, 0.0f, 1.0f });
        glUniformMatrix4fv(this->m_model_matrix_location, 1, GL_FALSE, glm::value_ptr(trans_matrix));
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Rotor 1
        trans_matrix = glm::translate(glm::identity<glm::mat4>(), glm::vec3{ 0.0f, 0.2f, 0.0f });
        trans_matrix = glm::rotate(trans_matrix, rotor_1_angle, glm::vec3{ 0.0f, 0.0f, 1.0f });
        glUniformMatrix4fv(this->m_model_matrix_location, 1, GL_FALSE, glm::value_ptr(trans_matrix));
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Rotor 2
        trans_matrix = glm::translate(glm::identity<glm::mat4>(), glm::vec3{ 0.0f, 0.2f, 0.0f });
        trans_matrix = glm::rotate(trans_matrix, rotor_2_angle, glm::vec3{ 0.0f, 0.0f, 1.0f });
        glUniformMatrix4fv(this->m_model_matrix_location, 1, GL_FALSE, glm::value_ptr(trans_matrix));
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Rotor 3
        trans_matrix = glm::translate(glm::identity<glm::mat4>(), glm::vec3{ 0.0f, 0.2f, 0.0f });
        trans_matrix = glm::rotate(trans_matrix, rotor_3_angle, glm::vec3{ 0.0f, 0.0f, 1.0f });
        glUniformMatrix4fv(this->m_model_matrix_location, 1, GL_FALSE, glm::value_ptr(trans_matrix));
        glDrawArrays(GL_TRIANGLES, 0, 3);

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
    }

private:
    GLuint m_stem_vao; // Vertex Array Object of the stem object.
    GLuint m_rotor_vao; // Vertex Array Object of the rotor object.

    GLuint m_program_id; // Shader id.
    GLuint m_model_color_location; // Uniform location of the model color.
    GLuint m_model_matrix_location; // Uniform location of the model matrix.

    float m_rotor_rotation;
};
