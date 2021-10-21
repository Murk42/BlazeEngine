#version 450

layout(location = 0) in vec4 vert_model0;
layout(location = 1) in vec4 vert_model1;
layout(location = 2) in vec4 vert_model2;
layout(location = 3) in vec4 vert_model3;
layout(location = 4) in vec2 vert_size;
layout(location = 5) in vec4 vert_color;
layout(location = 6) in vec4 vert_outlineColor;
layout(location = 7) in float vert_radius;
layout(location = 8) in float vert_thickness;

out mat4 geom_MVP;
out vec2 geom_size;
out vec4 geom_color;
out vec4 geom_outlineColor;
out float geom_radius;
out float geom_thickness;

uniform mat4 u_PV;

void main()
{		
	mat4 model = mat4(
		vert_model0,
		vert_model1,
		vert_model2,
		vert_model3
		);
	model = transpose(model);
	geom_MVP = u_PV * model;
	geom_size = vert_size;
	geom_color = vert_color;
	geom_outlineColor = vert_outlineColor;
	geom_radius = vert_radius;
	geom_thickness = vert_thickness;
}