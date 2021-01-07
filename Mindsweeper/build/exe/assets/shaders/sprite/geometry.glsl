#version 330

in VS_OUT {
	vec4 rect;
	float tex;
	float depth;
} gs_in[];

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

out vec3 p_uv;

uniform mat4 u_MVP;

void main()
{		
	gl_Position = u_MVP * vec4(gs_in[0].rect.xy, gs_in[0].depth, 1);
	p_uv = vec3(0, 0, gs_in[0].tex);	
	EmitVertex();
	
	gl_Position = u_MVP * vec4(gs_in[0].rect.xw, gs_in[0].depth, 1);
	p_uv = vec3(0, 1, gs_in[0].tex);	
	EmitVertex();
	
	gl_Position = u_MVP * vec4(gs_in[0].rect.zy, gs_in[0].depth, 1);
	p_uv = vec3(1, 0, gs_in[0].tex);	
	EmitVertex();
	
	gl_Position = u_MVP * vec4(gs_in[0].rect.zw, gs_in[0].depth, 1);
	p_uv = vec3(1, 1, gs_in[0].tex);	
	EmitVertex();

	EndPrimitive();
}