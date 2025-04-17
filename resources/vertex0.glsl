// Vertex Shader
#version 330 core

layout(location = 0) in vec3 vertex_pos;

out vec4 fragmentColor;

uniform mat4 model_matrix;
uniform vec3 model_color;

void main() {
    gl_Position = model_matrix * vec4(vertex_pos, 1.0);
    fragmentColor = vec4(model_color, 1);
}
