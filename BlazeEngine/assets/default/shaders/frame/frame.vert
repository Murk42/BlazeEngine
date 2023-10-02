#version 450 

layout(location = 0) in vec4 i_fillColor;
layout(location = 1) in vec4 i_borderColor;
layout(location = 2) in float i_radius;
layout(location = 3) in float i_borderWidth;
layout(location = 4) in vec2 i_size;
layout(location = 5) in mat4 i_mat;

out mat4 geom_mat;
out vec4 geom_fillColor;
out vec4 geom_borderColor;
out float geom_radius;
out float geom_borderWidth;
out vec2 geom_size;

void main()
{	
	geom_mat = i_mat;
	geom_fillColor = i_fillColor;
	geom_borderColor = i_borderColor;
	geom_radius = i_radius;
	geom_borderWidth = i_borderWidth;	
	geom_size = i_size;
}