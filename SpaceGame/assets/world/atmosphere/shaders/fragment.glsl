#version 330 core

in vec3 p_pos;

uniform vec3 p_cameraPos;
uniform vec3 p_starVec;
uniform vec3 p_cameraForward;

void main()
{	
	float val = dot(p_starVec, p_cameraForward) * 20;
	float depth = 1 - distance(p_pos, p_cameraPos) / 30;	
	gl_FragColor = vec4(val, 0, 0, depth);
}