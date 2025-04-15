// Fragment Shader
#version 330 core

in vec4 fragmentColor; // Input color from the vertex shader
out vec4 color;

void main() {
    color = fragmentColor;
}
