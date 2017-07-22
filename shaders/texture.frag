#version 330 core

in vec2 tex_coord;
out vec4 frag_color;

uniform sampler2D texture1;
uniform float mix_ratio;
uniform vec3 mix_color;

void main()
{
  frag_color = mix(texture(texture1, tex_coord), vec4(mix_color,1), mix_ratio);
}
