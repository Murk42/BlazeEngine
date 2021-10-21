#version 330

in VS_OUT {
	vec2 size;	
	float uv;
	float depth;
} gs_in[];

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

out vec4 p_rect;
out vec3 p_uv;

uniform mat4 u_MVP;

void main()
{		
	vec2 size = gs_in[0].size;

	p_rect = vec4(0, 0, size);	
	gl_Position = u_MVP * vec4(0, 0, gs_in[0].depth, 1);
	p_uv = vec3(0, 0, gs_in[0].uv);	
	EmitVertex();
		
	p_rect = vec4(0, size.y, size);	
	gl_Position = u_MVP * vec4(0, size.y, gs_in[0].depth, 1);
	p_uv = vec3(0, 1, gs_in[0].uv);	
	EmitVertex();
	
	p_rect = vec4(size.x, 0, size);	
	gl_Position = u_MVP * vec4(size.x, 0, gs_in[0].depth, 1);
	p_uv = vec3(1, 0, gs_in[0].uv);	
	EmitVertex();
	
	p_rect = vec4(size, size);	
	gl_Position = u_MVP * vec4(size, gs_in[0].depth, 1);
	p_uv = vec3(1, 1, gs_in[0].uv);	
	EmitVertex();

	EndPrimitive();
}