#version 450

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec2 geom_pos1[];
in vec2 geom_pos2[];
in vec2 geom_uv1[];
in vec2 geom_uv2[];

out vec2 frag_uv;
out vec2 frag_pos;

layout(location = 0) uniform mat4 u_model;
layout(location = 1) uniform mat4 u_proj;

void main()
{
	vec4 screenSpace;

	screenSpace = u_model * vec4(geom_pos1[0].x, geom_pos1[0].y, 0, 1);
	gl_Position = u_proj * screenSpace;
	frag_pos = screenSpace.xy;
	frag_uv = vec2(geom_uv1[0].x, geom_uv1[0].y);
	EmitVertex();

	screenSpace = u_model * vec4(geom_pos1[0].x, geom_pos2[0].y, 0, 1);
	gl_Position = u_proj * screenSpace;
	frag_pos = screenSpace.xy;
	frag_uv = vec2(geom_uv1[0].x, geom_uv2[0].y);
	EmitVertex();

	screenSpace = u_model * vec4(geom_pos2[0].x, geom_pos1[0].y, 0, 1);
	gl_Position = u_proj * screenSpace;
	frag_pos = screenSpace.xy;
	frag_uv = vec2(geom_uv2[0].x, geom_uv1[0].y);
	EmitVertex();

	screenSpace = u_model * vec4(geom_pos2[0].x, geom_pos2[0].y, 0, 1);
	gl_Position = u_proj * screenSpace;
	frag_pos = screenSpace.xy;
	frag_uv = vec2(geom_uv2[0].x, geom_uv2[0].y);	
	EmitVertex();

	EndPrimitive();
}