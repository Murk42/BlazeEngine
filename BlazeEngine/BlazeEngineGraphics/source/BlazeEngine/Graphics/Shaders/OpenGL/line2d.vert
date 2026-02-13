#version 450

layout(location = 0) in vec2 vert_pos1;
layout(location = 1) in vec2 vert_pos2;
layout(location = 2) in vec4 vert_color;
layout(location = 3) in float vert_width;

layout(location = 0) uniform mat4 u_MVP;

out vec2 frag_pos;
out vec2 frag_pos1;
out vec2 frag_pos2;
out vec4 frag_color;
out float frag_width;

void main()
{				
	vec2 dir = vert_pos2 - vert_pos1;
	float len = length(dir);
	dir /= len;
	vec2 normal = vec2(dir.y, -dir.x);

	vec2 pos;

	//+1 is to make the rectangle around the line wider so antialiased pixels will fit inside.
	switch(gl_VertexID)
	{
	case 0:
		pos = vert_pos1 - dir * (vert_width + 1) - normal * (vert_width + 1);
		break;
	case 1:
		pos = vert_pos2 + dir * (vert_width + 1) - normal * (vert_width + 1);
		break;
	case 2:
		pos = vert_pos1- dir * (vert_width + 1) + normal * (vert_width + 1);
		break;
	case 3:
		pos = vert_pos2 + dir * (vert_width + 1) + normal * (vert_width + 1);
		break;
	}

	gl_Position = u_MVP * vec4(pos, 0, 1);											

	frag_color = vert_color;
	frag_pos = pos;																	 
	frag_pos1 = vert_pos1;															 
	frag_pos2 = vert_pos2;
	frag_width = vert_width;							
}