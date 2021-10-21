#version 330 core

layout(location = 0) in vec2 i_pos1;
layout(location = 1) in vec2 i_pos2;
layout(location = 2) in vec2 i_uv1;
layout(location = 3) in vec2 i_uv2;
layout(location = 4) in vec4 i_color;

out vec4 geom_pos1;
out vec4 geom_pos2;
out vec2 geom_uv1;
out vec2 geom_uv2;
out vec4 geom_color;

uniform mat4 u_MVP;

void main()
{
	geom_pos1 = u_MVP * vec4(i_pos1, 0, 1);
	geom_pos2 = u_MVP * vec4(i_pos2, 0, 1);
	geom_uv1 = i_uv1;
	geom_uv2 = i_uv2;
	geom_color = i_color;	
}