#version 330 core

layout(location = 0) in vec2 i_pos1;
layout(location = 1) in vec2 i_pos2;
layout(location = 2) in vec4 i_color;
layout(location = 3) in float i_depth;

out VS_OUT {
	vec4 rect;
	vec4 color;
	float depth;
} vs_out;

void main()
{
	vs_out.rect = vec4(i_pos1, i_pos2);		
	vs_out.color = i_color;
	vs_out.depth = i_depth;
}
