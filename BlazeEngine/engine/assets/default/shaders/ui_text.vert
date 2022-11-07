#version 450 core

layout(location = 0) in vec2 i_pos1;
layout(location = 1) in vec2 i_pos2;
layout(location = 2) in vec2 i_uv1;
layout(location = 3) in vec2 i_uv2;

out vec2 geom_pos1;
out vec2 geom_pos2;
out vec2 geom_uv1;
out vec2 geom_uv2;

void main()
{
	geom_pos1 = i_pos1;
	geom_pos2 = i_pos2;
	geom_uv1 = i_uv1;
	geom_uv2 = i_uv2;
}
