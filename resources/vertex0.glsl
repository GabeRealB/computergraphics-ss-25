// Vertex Shader
#version 330 core

layout(location = 0) in vec3 vertex_pos;

out vec4 fragmentColor;

uniform mat4 view_projection_matrix;
uniform mat4 model_matrix;

void main() {
    gl_Position = view_projection_matrix * model_matrix * vec4(vertex_pos, 1.0);
    fragmentColor = vec4(0.3, 0.3, 0.3, 1);
}
