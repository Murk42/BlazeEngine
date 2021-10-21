#version 330 core

layout(location = 0) in vec3 i_pos;

uniform mat4 u_VP;
uniform mat4 u_MVP;

out vec3 p_pos;

void main()
{					
	p_pos = (u_VP * vec4(i_pos, 1)).xyz;
	gl_Position = u_MVP * vec4(i_pos, 1);
}
 