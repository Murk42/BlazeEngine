#version 330

in VS_OUT {
	vec4 rect;
	vec4 uv_rect;	
	float layer;
} gs_in[];

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

out vec2 p_uv;
out float p_layer;

uniform mat4 u_MVP;

void main()
{		
	gl_Position = u_MVP * vec4(gs_in[0].rect.xy, 1, 1);
	p_uv = gs_in[0].uv_rect.xy;	
	p_layer = gs_in[0].layer;
	EmitVertex();
	
	gl_Position = u_MVP * vec4(gs_in[0].rect.xw, 1, 1);
	p_uv = gs_in[0].uv_rect.xw;		
	p_layer = gs_in[0].layer;
	EmitVertex();
	
	gl_Position = u_MVP * vec4(gs_in[0].rect.zy, 1, 1);
	p_uv = gs_in[0].uv_rect.zy;		
	p_layer = gs_in[0].layer;
	EmitVertex();
	
	gl_Position = u_MVP * vec4(gs_in[0].rect.zw, 1, 1);
	p_uv = gs_in[0].uv_rect.zw;		
	p_layer = gs_in[0].layer;
	EmitVertex();

	EndPrimitive();
}