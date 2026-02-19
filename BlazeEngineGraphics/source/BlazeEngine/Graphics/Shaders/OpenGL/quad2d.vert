#version 450

layout(location = 0) in vec2 vert_pos1;
layout(location = 1) in vec2 vert_pos2;
layout(location = 2) in vec2 vert_pos3;
layout(location = 3) in vec4 vert_color;

layout(location = 0) uniform mat4 u_MVP;

out vec4 frag_color;

void main()
{				
	vec2 pos;

	switch(gl_VertexID)
	{
	case 0:
		pos = vert_pos1;
		break;
	case 1:
		pos = vert_pos2;
		break;
	case 2:
		pos = vert_pos3;
		break;
	case 3:
		pos = vert_pos2 + vert_pos3 - vert_pos1;
		break;
	}

	gl_Position = u_MVP * vec4(pos, 0, 1);											

	frag_color = vert_color;
}