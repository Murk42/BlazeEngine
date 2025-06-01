#version 450															   
																		   
in float frag_dot;

out vec4 out_color;

layout(location = 4) uniform vec3 u_ambientColor;
layout(location = 5) uniform vec4 u_color;
																		   
void main()				   									   
{					
	out_color = vec4(mix(u_ambientColor, u_color.rgb, clamp(frag_dot + 1, 0, 1)), u_color.a);
}