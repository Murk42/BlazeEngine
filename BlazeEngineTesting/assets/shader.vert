#version 450 

layout(location = 0) in vec3 vert_pos;

uniform mat4 u_MVP;

out vec3 geom_pos;
out vec4 geom_outPos;

void main()
{
	geom_pos = vert_pos;
	geom_outPos = u_MVP * vec4(vert_pos, 1);
}