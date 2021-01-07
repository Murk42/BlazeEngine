#version 330

in VS_OUT {
	vec4 rect;
	vec4 uv_rect;
	vec4 color;
	float depth;
} gs_in[];

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

out vec2 p_uv;
out vec4 p_color;

uniform mat4 u_MVP;

void main()
{		
	gl_Position = u_MVP * vec4(gs_in[0].rect.xy, gs_in[0].depth, 1);
	p_uv = vec2(gs_in[0].uv_rect.x, gs_in[0].uv_rect.y);
	p_color = gs_in[0].color;
	EmitVertex();
	
	gl_Position = u_MVP * vec4(gs_in[0].rect.xw, gs_in[0].depth, 1);
	p_uv = vec2(gs_in[0].uv_rect.x, gs_in[0].uv_rect.w);	
	p_color = gs_in[0].color;
	EmitVertex();
	
	gl_Position = u_MVP * vec4(gs_in[0].rect.zy, gs_in[0].depth, 1);
	p_uv = vec2(gs_in[0].uv_rect.z, gs_in[0].uv_rect.y);	
	p_color = gs_in[0].color;
	EmitVertex();
	
	gl_Position = u_MVP * vec4(gs_in[0].rect.zw, gs_in[0].depth, 1);
	p_uv = vec2(gs_in[0].uv_rect.z, gs_in[0].uv_rect.w);	
	p_color = gs_in[0].color;
	EmitVertex();

	EndPrimitive();
}