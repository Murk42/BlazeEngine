#version 330 core

in float y;

void main()
{
	gl_FragColor = vec4(y, y, y, 1);	
}