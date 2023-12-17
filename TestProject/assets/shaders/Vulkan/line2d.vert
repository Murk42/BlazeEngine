#version 450

layout(location = 0) in vec2 vert_pos;
layout(location = 1) in vec2 vert_pos1;
layout(location = 2) in vec2 vert_pos2;
layout(location = 3) in vec4 vert_color;
layout(location = 4) in float vert_width;

layout( push_constant ) uniform constants
{	
	mat4 MVP;
} PushConstants;

layout(location = 0) out vec2 frag_pos;
layout(location = 1) out vec2 frag_pos1;
layout(location = 2) out vec2 frag_pos2;
layout(location = 3) out vec4 frag_color;
layout(location = 4) out float frag_width;

void main()
{																							 
	gl_Position = PushConstants.MVP * vec4(vert_pos, 0, 1);											

	frag_color = vert_color;
	frag_pos = vert_pos;																	 
	frag_pos1 = vert_pos1;															 
	frag_pos2 = vert_pos2;
	frag_width = vert_width;							
}