// Vertex Shader
#version 330 core

layout(location = 0) in vec3 vertex_pos;

out vec2 uvPos;

uniform uint parametrisation;
uniform vec3 model_center;
uniform vec3 model_size;

uniform mat4 view_projection_matrix;
uniform mat4 model_matrix;

const float PI = 3.1415926535897932384626433832795;

vec2 compute_cylindrical_uv_coords(vec3 vertex) {
    vec3 vertex_centered = vertex - model_center;
    float u = acos(vertex_centered.x / (0.5 * model_size.x)) / PI;
    float v = (vertex_centered.y / model_size.y) + 0.5;
    return vec2(u, v);
}

vec2 compute_spherical_uv_coords(vec3 vertex) {
    vec3 vertex_centered = vertex - model_center;
    vec3 vertex_normalized = normalize(vertex_centered);
    float theta = acos(vertex_normalized.y);
    float u = acos(vertex_normalized.x / sin(theta)) / PI;
    float v = theta / PI;
    return vec2(u, v);
}

vec2 compute_uv_coords(vec3 vertex) {
    if (parametrisation == 0u) {
        return compute_cylindrical_uv_coords(vertex);
    } else {
        return compute_spherical_uv_coords(vertex);
    }
}

void main() {
    gl_Position = view_projection_matrix * model_matrix * vec4(vertex_pos, 1.0);
    uvPos = compute_uv_coords(vertex_pos);
}
