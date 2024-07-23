#version 330 core

in vec3 vert_color;
out vec4 FragColor;

void main() {
   FragColor = vec4(vert_color, 1.0);
}