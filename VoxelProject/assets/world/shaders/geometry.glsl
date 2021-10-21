#version 330

in VS_OUT {
	vec4 pos1;
	vec4 pos2;
	vec4 pos3;
	vec4 pos4;
	vec4 color;	
} gs_in[];

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 u_MVP;
uniform vec3 u_cameraPos;

out vec4 p_color;
out vec3 p_pos;

void main()
{			
	gl_Position = u_MVP * gs_in[0].pos1;	
	p_pos = gs_in[0].pos1.xyz - u_cameraPos.xyz;
	p_color = gs_in[0].color;
	EmitVertex();	
	gl_Position = u_MVP * gs_in[0].pos2;
	p_pos = gs_in[0].pos2.xyz - u_cameraPos.xyz;
	p_color = gs_in[0].color;
	EmitVertex();	
	gl_Position = u_MVP * gs_in[0].pos3;	
	p_pos = gs_in[0].pos3.xyz - u_cameraPos.xyz;
	p_color = gs_in[0].color;
	EmitVertex();	
	gl_Position = u_MVP * gs_in[0].pos4;	
	p_pos = gs_in[0].pos4.xyz - u_cameraPos.xyz;
	p_color = gs_in[0].color;
	EmitVertex();

	EndPrimitive();
}