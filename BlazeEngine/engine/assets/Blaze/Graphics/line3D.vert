#version 450

layout(location = 0) in vec3 vert_pos1;
layout(location = 1) in vec3 vert_pos2;
layout(location = 2) in vec4 vert_color;
layout(location = 3) in float vert_width;

out vec3 geom_pos1;
out vec3 geom_pos2;
out vec4 geom_color;
out float geom_width;

void main()
{
	geom_pos1 = vert_pos1;
	geom_pos2 = vert_pos2;
	geom_color = vert_color;
	geom_width = vert_width;
}