#version 330 core

in vec2 TexCoord;
uniform sampler2D image_texture;
out vec4 FragColor;

void main()
{
	FragColor = vec4(texture(image_texture, TexCoord).rgb, 1.0);
}
