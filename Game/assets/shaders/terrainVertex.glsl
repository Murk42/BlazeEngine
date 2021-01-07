#version 330 core

layout(location = 0) in vec3 i_pos;

uniform mat4 u_MVP;

out float y;

void main()
{	
	y = i_pos.y;
	gl_Position = u_MVP * vec4(i_pos, 1);
}
