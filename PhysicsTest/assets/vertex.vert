#version 450

layout(location = 0) in vec2 pos;

uniform float u_size;
uniform mat4 u_MVP;

void main()
{
	gl_Position = u_MVP * vec4(pos, 0, 1);

	gl_PointSize = u_size;	
}