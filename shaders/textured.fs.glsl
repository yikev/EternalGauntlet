#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform int hit;
// uniform int is_hp;
uniform float hp_percentage;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	if (hit == 1) {
		color = vec4(1.0, 0.0, 0.0, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	} else {
		color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	}

	// if (is_hp == 1) {
	// 	float offset = texcoord.x > hp_percentage ? 0.0 : 0.5;
	// 	vec2 newTexCoord = vec2(texcoord.x / 2 + offset, texcoord.y);
	// 	color = texture(sampler0, newTexCoord);
	// }
	
}
