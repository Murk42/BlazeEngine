#version 330 core

in vec4 p_color;
in vec3 p_pos;

uniform float u_viewDistance;
uniform vec4 u_clearColor;

void main()
{			
	vec4 color = p_color;	
	//float x = clamp(length(p_pos) / u_viewDistance, 0, 1);
	//gl_FragColor = mix(p_color, u_clearColor, x * x * x * x);	
	gl_FragColor = p_color;
}