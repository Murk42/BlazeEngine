#version 450 core
 
layout(location = 0) in vec3 i_pos;

layout(location = 2) uniform mat4 u_modelMatrix;
 
void main() 
{
	gl_Position = u_modelMatrix * vec4(i_pos, 1);
}