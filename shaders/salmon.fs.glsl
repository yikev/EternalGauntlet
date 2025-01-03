#version 330

// From Vertex Shader
in vec3 vcolor;
in vec2 vpos; // Distance from local origin

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform int hit;

// Output color
layout(location = 0) out vec4 color;

void main()
{
	if (hit == 1) {
		color = vec4(1.0, 0.0, 0.0, 1.0);
	} else {
		color = vec4(fcolor * vcolor, 1.0);
	}

}