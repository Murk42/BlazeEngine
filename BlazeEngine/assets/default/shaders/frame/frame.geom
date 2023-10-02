#version 450

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in mat4 geom_mat[];
in vec4 geom_fillColor[];
in vec4 geom_borderColor[];
in float geom_radius[];
in float geom_borderWidth[];
in vec2 geom_size[];

out vec2 frag_pos;
out vec4 frag_rect;
out vec4 frag_fillColor;
out vec4 frag_borderColor;
out float frag_radius;
out float frag_borderWidth;

layout(location = 0) uniform mat4 u_proj;

void main()
{
	vec4 p1 = geom_mat[0] * vec4(0, 0, 0, 1);
	vec4 p2 = geom_mat[0] * vec4(0, geom_size[0].y, 0, 1);
	vec4 p3 = geom_mat[0] * vec4(geom_size[0].x, 0, 0, 1);
	vec4 p4 = geom_mat[0] * vec4(geom_size[0].x, geom_size[0].y, 0, 1);
	vec4 rect = vec4(p3.xy - p1.xy, p2.xy - p1.xy);
	
	gl_Position = u_proj * p1;
	frag_pos = p1.xy - p1.xy;
	frag_rect = rect;
	frag_fillColor = geom_fillColor[0];
	frag_borderColor = geom_borderColor[0];
	frag_radius = geom_radius[0];
	frag_borderWidth = geom_borderWidth[0];	
	EmitVertex();

	gl_Position = u_proj * p2;
	frag_pos = p2.xy - p1.xy;
	frag_rect = rect;
	frag_fillColor = geom_fillColor[0];
	frag_borderColor = geom_borderColor[0];
	frag_radius = geom_radius[0];
	frag_borderWidth = geom_borderWidth[0];	
	EmitVertex();

	gl_Position = u_proj * p3;
	frag_pos = p3.xy - p1.xy;
	frag_rect = rect;
	frag_fillColor = geom_fillColor[0];
	frag_borderColor = geom_borderColor[0];
	frag_radius = geom_radius[0];
	frag_borderWidth = geom_borderWidth[0];	
	EmitVertex();

	gl_Position = u_proj * p4;
	frag_pos = p4.xy - p1.xy;
	frag_rect = rect;
	frag_fillColor = geom_fillColor[0];
	frag_borderColor = geom_borderColor[0];
	frag_radius = geom_radius[0];
	frag_borderWidth = geom_borderWidth[0];	
	EmitVertex();

	EndPrimitive();
}