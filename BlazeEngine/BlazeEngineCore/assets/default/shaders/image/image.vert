#version 450

layout(location = 0) in mat4 vert_matrix;
layout(location = 4) in vec2 vert_uv[4];
layout(location = 8) in vec2 vert_size;
layout(location = 9) in vec4 vert_color;
layout(location = 10) in float vert_blend;

out mat4 geom_matrix;
out vec2 geom_uv[4];
out vec2 geom_size;
out vec4 geom_color;
out float geom_blend;

void main()
{
	geom_matrix = vert_matrix;
	geom_uv = vert_uv;	
	geom_size = vert_size;
	geom_color = vert_color;
	geom_blend = vert_blend;
}