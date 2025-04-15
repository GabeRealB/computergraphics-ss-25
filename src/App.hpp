#pragma once
// GLAD
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

class App {
public:
    App()
        : m_triangle_vao { 0 }
        , m_house_vao { 0 }
        , m_program_id { 0 }
    {
    }

    void initialize_triangle_vao()
    {
        // Generates a single Vertex Array Object (VAO) and stores its ID in 'm_triangle_vao'
        glGenVertexArrays(1, &this->m_triangle_vao);
        // Binds the VAO to define how vertex attributes are stored
        glBindVertexArray(this->m_triangle_vao);

        constexpr static float VERTEX_POSITIONS[] = {
            -1.0f, -1.0f, 0.0f, // First vertex of a triangle (bottom-left)
            1.0f, -1.0f, 0.0f, // Second vertex of a triangle (bottom-right)
            0.0f, 1.0f, 0.0f, // Third vertex of a triangle (top-center)
        };

        GLuint vertex_buffer;
        // Generates a single OpenGL buffer and stores its ID in 'vertex_buffer'
        glGenBuffers(1, &vertex_buffer);
        // Binds the newly created buffer to the GL_ARRAY_BUFFER target
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        // Creates and initializes the buffer object's data store with vertex data
        glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX_POSITIONS), VERTEX_POSITIONS, GL_STATIC_DRAW);

        // Enables a generic vertex attribute array
        glEnableVertexAttribArray(0);
        // Binds the vertex buffer to the GL_ARRAY_BUFFER target again (for drawing)
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glVertexAttribPointer(
            0, // Specifies the index of the generic vertex attribute
            3, // Specifies the number of components per generic vertex attribute
            GL_FLOAT, // Specifies the data type of each component
            GL_FALSE, // Specifies whether fixed-point data values should be normalized
            0, // Specifies the byte offset between consecutive generic vertex attributes
            (void*)0 // Specifies a pointer to the start of the first component of the first generic vertex attribute in the array
        );

        // Unbind the Vertex Array Object
        glBindVertexArray(0);
    }

    void initialize_house_vao()
    {
        // Generates a single Vertex Array Object (VAO) and stores its ID in 'm_house_vao'
        glGenVertexArrays(1, &this->m_house_vao);
        // Binds the VAO to define how vertex attributes are stored
        glBindVertexArray(this->m_house_vao);

        // Coordinates for "Haus vom Niklaus" (a simple house shape)
        constexpr static float VERTEX_POSITIONS[] = {
            -0.5f, -0.5f, 0.0f, // A (bottom-left)
            0.5f, -0.5f, 0.0f, // B (bottom-right)
            -0.5f, 0.5f, 0.0f, // C (top-left)
            0.5f, 0.5f, 0.0f, // D (top-right)
            0.0f, 1.0f, 0.0f, // E (roof peak)
            -0.5f, 0.5f, 0.0f, // C (repeated for drawing continuity)
            -0.5f, -0.5f, 0.0f, // A (repeated for drawing continuity)
            0.5f, 0.5f, 0.0f, // D (repeated for drawing continuity)
            0.5f, -0.5f, 0.0f, // B (repeated for drawing continuity)
        };

        GLuint vertex_buffer;
        // Generates a single OpenGL buffer and stores its ID in 'vertex_buffer'
        glGenBuffers(1, &vertex_buffer);
        // Binds the newly created buffer to the GL_ARRAY_BUFFER target
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        // Creates and initializes the buffer object's data store with vertex data
        glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX_POSITIONS), VERTEX_POSITIONS, GL_STATIC_DRAW);

        // Enables a generic vertex attribute array
        glEnableVertexAttribArray(0);
        // Binds the vertex buffer to the GL_ARRAY_BUFFER target again (for drawing)
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glVertexAttribPointer(
            0, // Specifies the index of the generic vertex attribute
            3, // Specifies the number of components per generic vertex attribute
            GL_FLOAT, // Specifies the data type of each component
            GL_FALSE, // Specifies whether fixed-point data values should be normalized
            0, // Specifies the byte offset between consecutive generic vertex attributes
            (void*)0 // Specifies a pointer to the start of the first component of the first generic vertex attribute in the array
        );

        // Unbind the Vertex Array Object
        glBindVertexArray(0);
    }

    void init(GLFWwindow*)
    {
        this->initialize_triangle_vao();
        this->initialize_house_vao();

        // Set the triangle as the active shape.
        this->m_active_vao = this->m_triangle_vao;
        this->m_drawing_mode = GL_TRIANGLES;
        this->m_vertices_count = 3;

        // Loads and compiles shaders from files and links them into a program
        this->m_program_id = Shader::loadShaders(to_resource_path("vertex0.glsl"), to_resource_path("fragment0.glsl"));
        // Installs the program object as part of the current rendering state
        glUseProgram(this->m_program_id);
    }

    void draw(GLFWwindow*, float)
    {
        // Clears the color buffer to preset values
        glClear(GL_COLOR_BUFFER_BIT);

        // Sets the clear color for the color buffer
        glClearColor(1.0f, 1.0f, 0.2f, 1.0f);

        // Bind the the vertex array object of the shape.
        glBindVertexArray(this->m_active_vao);

        // Renders primitives from array data
        glDrawArrays(this->m_drawing_mode, 0, this->m_vertices_count);

        // Unbind the vertex array object.
        glBindVertexArray(0);
    }

    void on_key_change(GLFWwindow* window, int key, int, int action, int)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        // Switch vertex buffer data based on key presses to toggle between triangle and house shapes
        if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
            this->m_active_vao = this->m_triangle_vao;
            this->m_drawing_mode = GL_TRIANGLES;
            this->m_vertices_count = 3;
        }
        if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
            this->m_active_vao = this->m_house_vao;
            this->m_drawing_mode = GL_LINE_STRIP;
            this->m_vertices_count = 9;
        }
    }

    void on_resize(GLFWwindow*, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

private:
    GLuint m_triangle_vao; // Vertex Array Object for the triangle
    GLuint m_house_vao; // Vertex Array Object for the house
    GLuint m_program_id; // OpenGL ID for the shader program

    GLuint m_active_vao; // Selected Vertex Array Object
    GLenum m_drawing_mode; // Drawing mode of the selected Vertex Array Object
    int m_vertices_count; // Number of vertices in the selected Vertex Array Object
};
