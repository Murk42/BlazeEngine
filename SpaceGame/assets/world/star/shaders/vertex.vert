#version 330 core

layout(location = 0) in vec3 i_pos;

uniform mat4 u_MVP;
uniform float u_size;

void main()
{					
	gl_Position = u_MVP * vec4(i_pos * u_size, 1);
}
 