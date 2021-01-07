#version 330 core

layout(location = 0) in vec3 i_pos;

out vec3 p_uv;

uniform mat4 u_projection;
uniform mat4 u_view;

void main()
{
	p_uv = i_pos;
	gl_Position = u_projection * mat4(mat3(u_view)) * vec4(i_pos, 1);
}