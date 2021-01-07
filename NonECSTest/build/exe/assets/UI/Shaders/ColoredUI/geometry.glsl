#version 330

in VS_OUT {
	vec4 rect;
	vec4 color;
	float depth;
} gs_in[];

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

out vec2 p_pos;
out vec4 p_color;
out vec4 p_rect;

uniform mat4 u_MVP;

void main()
{	
	p_pos = gs_in[0].rect.xy;
	gl_Position = u_MVP * vec4(p_pos, gs_in[0].depth, 1);
	p_color = gs_in[0].color;
	p_rect = gs_in[0].rect;
	EmitVertex();

	p_pos = gs_in[0].rect.xw;
	gl_Position = u_MVP * vec4(p_pos, gs_in[0].depth, 1);
	p_color = gs_in[0].color;
	p_rect = gs_in[0].rect;
	EmitVertex();

	p_pos = gs_in[0].rect.zy;
	gl_Position = u_MVP * vec4(p_pos, gs_in[0].depth, 1);
	p_color = gs_in[0].color;
	p_rect = gs_in[0].rect;
	EmitVertex();

	p_pos = gs_in[0].rect.zw;
	gl_Position = u_MVP * vec4(p_pos, gs_in[0].depth	, 1);
	p_color = gs_in[0].color;
	p_rect = gs_in[0].rect;
	EmitVertex();

	EndPrimitive();
}