#version 330 core

layout(location = 0) in vec3 i_pos;
layout(location = 1) in vec2 i_uv;

uniform mat4 u_MVP;

out vec2 p_uv;

void main()
{
	p_uv = i_uv;
	gl_Position = u_MVP * vec4(i_pos, 1);
}
