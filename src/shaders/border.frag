#version 330 core

#define PI 3.1415926
#define EPSILON 0.000001

out vec4 FragColor;


uniform vec3 border_color;

void main() {
	
	FragColor = vec4(border_color,1);
}
