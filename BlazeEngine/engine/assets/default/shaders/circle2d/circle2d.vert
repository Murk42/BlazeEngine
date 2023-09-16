#version 450

layout(location = 0) in mat4 vert_mat;
layout(location = 4) in vec4 vert_outerColor;
layout(location = 5) in vec4 vert_innerColor;
layout(location = 6) in float vert_innerRadius;

out mat4 geom_mat;
out vec4  geom_outerColor;
out vec4  geom_innerColor;
out float geom_innerRadius;

void main()
{
	geom_mat = vert_mat;
	geom_outerColor = vert_outerColor;
	geom_innerColor = vert_innerColor;	
	geom_innerRadius = vert_innerRadius;
}