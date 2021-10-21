#version 330 core

uniform vec3 u_starPos;

in vec3 p_pos;
in vec3 p_norm;

void main()
{						
	float val = dot(p_norm, normalize(u_starPos - p_pos));
	gl_FragColor = vec4(p_norm, 1);	
}