#version 330 core

layout(location = 0) in vec3 i_pos1;
layout(location = 1) in vec3 i_pos2;
layout(location = 2) in vec3 i_pos3;
layout(location = 3) in vec3 i_color;

out VS_OUT {
	vec4 pos1;
	vec4 pos2;
	vec4 pos3;
	vec4 pos4;
	vec4 color;	
} vs_out;

void main()
{		
	vec3 i_pos4;
	i_pos4 = i_pos2 + i_pos3 - i_pos1;		
	vs_out.pos1 = vec4(i_pos1, 1);
	vs_out.pos2 = vec4(i_pos2, 1);
	vs_out.pos3 = vec4(i_pos3, 1);
	vs_out.pos4 = vec4(i_pos4, 1);
	vs_out.color = vec4(i_color, 1);
}
