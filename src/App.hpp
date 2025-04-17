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
            0.0f, 0.0f, 0.0f, // middle
            -0.2f, 0.6f, 0.0f, // top-right
            0.2f, 0.6f, 0.0f, // top-left
        };

        GLuint vertex_buffer;
        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX_POSITIONS), VERTEX_POSITIONS, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindVertexArray(0);

        this->m_rotor_0_transform.with_rotation_euler({ 0.0f, 0.0f, 0.0f });
        this->m_rotor_1_transform.with_rotation_euler({ 0.0f, 0.0f, glm::half_pi<float>() });
        this->m_rotor_transform.with_translation({ 0.0f, 0.2f, 0.0f });
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
        constexpr static float ROTATIONS_PER_SEC = 0.25f;
        const glm::quat rotation_quat { glm::vec3 { 0.0f, 0.0f, glm::tau<float>() * ROTATIONS_PER_SEC * delta_time } };
        const auto rotation = rotation_quat * this->m_rotor_transform.rotation();
        this->m_rotor_transform.with_rotation(rotation);

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.6f, 1.0f);

        glUseProgram(this->m_program_id);

        // Draw the stem
        glBindVertexArray(this->m_stem_vao);

        glm::mat4 stem_world = this->m_stem_transform.transform();
        glUniform3f(this->m_model_color_location, 0.8f, 0.0f, 0.0f);
        glUniformMatrix4fv(this->m_model_matrix_location, 1, GL_FALSE, glm::value_ptr(stem_world));
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // Draw the rotors.
        glBindVertexArray(this->m_rotor_vao);
        glm::mat4 rotor_0_world = this->m_rotor_0_transform.transform().with_parent(this->m_rotor_transform);
        glm::mat4 rotor_1_world = this->m_rotor_1_transform.transform().with_parent(this->m_rotor_transform);

        glUniform3f(this->m_model_color_location, 0.8f, 0.8f, 0.0f);
        glUniformMatrix4fv(this->m_model_matrix_location, 1, GL_FALSE, glm::value_ptr(rotor_0_world));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glUniformMatrix4fv(this->m_model_matrix_location, 1, GL_FALSE, glm::value_ptr(rotor_1_world));
        glDrawArrays(GL_TRIANGLES, 0, 6);

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

    Transform m_stem_transform;
    Transform m_rotor_0_transform;
    Transform m_rotor_1_transform;
    Transform m_rotor_transform;
};
