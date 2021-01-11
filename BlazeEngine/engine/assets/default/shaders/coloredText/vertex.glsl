#version 330 core

layout(location = 0) in vec2 i_pos1;
layout(location = 1) in vec2 i_pos2;
layout(location = 2) in vec2 i_uv1;
layout(location = 3) in vec2 i_uv2;
layout(location = 4) in vec4 i_color;

out VS_OUT {
	vec4 rect;
	vec4 uv_rect;
	vec4 color;	
} vs_out;

void main()
{
	vs_out.rect = vec4(i_pos1, i_pos2);	
	vs_out.uv_rect = vec4(i_uv1, i_uv2);
	vs_out.color = i_color;	
}
