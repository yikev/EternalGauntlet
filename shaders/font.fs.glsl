#version 330 core
/* simpleGL freetype font fragment shader */
in vec2 texcoord;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, texcoord).r);
	color = vec4(textColor, 1.0) * sampled;
}