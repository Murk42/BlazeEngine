#version 330 core

layout(location = 0) in vec2 i_pos1;
layout(location = 1) in vec2 i_pos2;
layout(location = 2) in unsigned i_color;

out VS_OUT {
	vec4 rect;
	vec4 color;
} vs_out;

void main()
{
	vs_out.rect = vec4(i_pos1, i_pos2);	
	vs_out.color = vec4(
		(i_color & 0x000000FF) >> 0,
		(i_color & 0x0000FF00) >> 8,
		(i_color & 0x00FF0000) >> 16,
		(i_color & 0xFF000000) >> 24,
		);
}
