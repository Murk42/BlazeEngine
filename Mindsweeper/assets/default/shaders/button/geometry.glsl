#version 330

in VS_OUT {
	vec4 rect;
	vec2 cornerSize;
	float uv;
	float depth;
} gs_in[];

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

out vec2 p_pos;
out vec2 p_size;
out vec3 p_uv;
out vec2 p_cornerSize;

uniform mat4 u_MVP;

void main()
{		
	p_cornerSize = gs_in[0].cornerSize;
	p_pos = vec2(0, 0);
	p_size = gs_in[0].rect.zw - gs_in[0].rect.xy;
	gl_Position = u_MVP * vec4(gs_in[0].rect.xy, gs_in[0].depth, 1);
	p_uv = vec3(0, 0, gs_in[0].uv);	
	EmitVertex();
	
	p_cornerSize = gs_in[0].cornerSize;
	p_pos = vec2(0, gs_in[0].rect.w - gs_in[0].rect.y);
	p_size = gs_in[0].rect.zw - gs_in[0].rect.xy;
	gl_Position = u_MVP * vec4(gs_in[0].rect.xw, gs_in[0].depth, 1);
	p_uv = vec3(0, 1, gs_in[0].uv);	
	EmitVertex();
	
	p_cornerSize = gs_in[0].cornerSize;
	p_pos = vec2(gs_in[0].rect.z - gs_in[0].rect.x, 0);
	p_size = gs_in[0].rect.zw - gs_in[0].rect.xy;
	gl_Position = u_MVP * vec4(gs_in[0].rect.zy, gs_in[0].depth, 1);
	p_uv = vec3(1, 0, gs_in[0].uv);	
	EmitVertex();
	
	p_cornerSize = gs_in[0].cornerSize;
	p_pos = gs_in[0].rect.zw - gs_in[0].rect.xy;
	p_size = gs_in[0].rect.zw - gs_in[0].rect.xy;
	gl_Position = u_MVP * vec4(gs_in[0].rect.zw, gs_in[0].depth, 1);
	p_uv = vec3(1, 1, gs_in[0].uv);	
	EmitVertex();

	EndPrimitive();
}