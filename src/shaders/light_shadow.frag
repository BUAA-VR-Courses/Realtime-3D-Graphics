#version 330 core

#define PI 3.1415926
#define EPSILON 0.000001

out vec4 FragColor;

uniform vec3 point_light_pos;
uniform vec3 point_light_radiance;
uniform sampler2D shadow_map;				
uniform bool have_shadow;

uniform mat4 light_space_matrix;

uniform float ambient;
uniform float specular;
uniform sampler2D color_texture;

uniform vec3 camera_pos;

in VS_OUT {
    vec3 WorldPos;
    vec2 TexCoord;
    vec3 Normal;
} vs_out;

float ShadowCalculation(vec4 fragPosLightSpace)
{
	// TODO 2.2.2 : Complete the following shadow mapping calculations

	return 0;
}

void main() {
	vec3 color = texture(color_texture, vs_out.TexCoord).rgb;

	vec4 fragPosLightSpace = light_space_matrix * vec4(vs_out.WorldPos, 1.0);

	// TODO 2.2.1 : Complete the following shadow mapping calculations


	float shadow = 0.0;
	if(have_shadow)
	{
		shadow = ShadowCalculation(fragPosLightSpace);
	}

	// TODO 2.2.1 : Modify the output of frag color
	FragColor = vec4(color, 1.0);
}
