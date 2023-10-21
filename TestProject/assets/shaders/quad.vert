#version 450 core

layout(location = 0) in vec3 pos;

layout(location = 0) uniform mat4 u_proj;

void main()
{
	gl_Position = u_proj * vec4(pos, 1);
}