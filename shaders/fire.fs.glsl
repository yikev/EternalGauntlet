#version 330

uniform sampler2D sampler0;
uniform float uv_x;
uniform float nx_frames;
in vec2 texcoord;

// Output color
layout(location = 0) out  vec4 color;

void main(){
  float x = 1.0 / nx_frames;
  color = texture(sampler0, vec2(texcoord.x * x, texcoord.y) + vec2(x * uv_x, 0.0));
  // color = texture(sampler0, texcoord);
  // color = vec4(0.1412, 0.1608, 0.1843, 1.0);
}