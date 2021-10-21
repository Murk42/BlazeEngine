#version 330

in VS_OUT {
	vec4 rect;
	vec4 uv_rect;	
} gs_in[];

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

out vec2 p_uv;

uniform mat4 u_MVP;

void main()
{		
	gl_Position = u_MVP * vec4(gs_in[0].rect.xy, 1, 1);
	p_uv = gs_in[0].uv_rect.xy;	
	EmitVertex();
	
	gl_Position = u_MVP * vec4(gs_in[0].rect.xw, 1, 1);
	p_uv = gs_in[0].uv_rect.xw;		
	EmitVertex();
	
	gl_Position = u_MVP * vec4(gs_in[0].rect.zy, 1, 1);
	p_uv = gs_in[0].uv_rect.zy;		
	EmitVertex();
	
	gl_Position = u_MVP * vec4(gs_in[0].rect.zw, 1, 1);
	p_uv = gs_in[0].uv_rect.zw;		
	EmitVertex();

	EndPrimitive();
}