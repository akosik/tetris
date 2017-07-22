#version 330 core

in vec2 tex_coord;
out vec4 frag_color;

uniform sampler2D texture1;
uniform vec4 in_color;

void main()
{
  frag_color = in_color;
}
