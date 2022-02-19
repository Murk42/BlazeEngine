#version 450

in vec2 geom_pos1[];
in vec2 geom_pos2[];
in vec4 geom_color[];
in float geom_width[];

out vec4 frag_color;
out vec2 frag_pos;
out vec2 frag_pos1;
out vec2 frag_pos2;
out float frag_width;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 u_MVP;

void main()
{
	vec2 dir = geom_pos2[0] - geom_pos1[0];
	float len = length(geom_pos2[0] - geom_pos1[0]);
	dir = dir / len;
	vec2 normal = vec2(dir.y, -dir.x);
	float half_width = geom_width[0];

	vec2 pos1 = geom_pos1[0] - dir * (half_width + 1)		- normal * (half_width + 1);
	vec2 pos2 = geom_pos1[0] + dir * (half_width + len + 1)	- normal * (half_width + 1);
	vec2 pos3 = geom_pos1[0] - dir * (half_width + 1)		+ normal * (half_width + 1);
	vec2 pos4 = geom_pos1[0] + dir * (half_width + len + 1) + normal * (half_width + 1);

	gl_Position = u_MVP * vec4(pos1, 0, 1);
	frag_color = geom_color[0];	
	frag_pos = pos1;
	frag_pos1 = geom_pos1[0];
	frag_pos2 = geom_pos2[0];
	frag_width = geom_width[0];
	EmitVertex();
	
	gl_Position = u_MVP * vec4(pos2, 0, 1);
	frag_color = geom_color[0];
	frag_pos = pos2;
	frag_pos1 = geom_pos1[0];
	frag_pos2 = geom_pos2[0];
	frag_width = geom_width[0];
	EmitVertex();
	
	gl_Position = u_MVP * vec4(pos3, 0, 1);
	frag_color = geom_color[0];
	frag_pos = pos3;
	frag_pos1 = geom_pos1[0];
	frag_pos2 = geom_pos2[0];
	frag_width = geom_width[0];
	EmitVertex();
	
	gl_Position = u_MVP * vec4(pos4, 0, 1);
	frag_color = geom_color[0];
	frag_pos = pos4;
	frag_pos1 = geom_pos1[0];
	frag_pos2 = geom_pos2[0];
	frag_width = geom_width[0];
	EmitVertex();

	EndPrimitive();
}