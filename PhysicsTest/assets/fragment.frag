#version 450

void main()
{
	vec2 pos = gl_PointCoord.xy;
	float len = length(pos * 2 - 1);		
	gl_FragColor = vec4(1, 1, 1, 1 - len);
}