#version 330

in VS_OUT {
	vec4 rect;
	vec4 color;
} gs_in[];

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

out vec4 p_color;

uniform mat4 u_MVP;

void main()
{		
	vec2 pos = gs_in[0].rect.xy;
	vec2 size = gs_in[0].rect.zw;

	gl_Position = u_MVP * vec4(pos, 0, 1);		
	p_color = gs_in[0].color;
	EmitVertex();
			
	gl_Position = u_MVP * vec4(pos + vec2(size.x, 0), 0, 1);
	p_color = gs_in[0].color;
	EmitVertex();
		
	gl_Position = u_MVP * vec4(pos + vec2(0, size.y), 0, 1);
	p_color = gs_in[0].color;
	EmitVertex();
		
	gl_Position = u_MVP * vec4(pos + size, 0, 1);
	p_color = gs_in[0].color;
	EmitVertex();

	EndPrimitive();
}