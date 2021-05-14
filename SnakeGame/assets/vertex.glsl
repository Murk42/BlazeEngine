#version 330 core

layout(location = 0) in vec4 i_rect;
layout(location = 1) in vec4 i_color;

out VS_OUT {
	vec4 rect;
	vec4 color;
} vs_out;

void main()
{
	vs_out.rect = i_rect;
	vs_out.color = i_color;
}
