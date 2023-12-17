#version 450

layout(location = 0) in vec2 vert_pos;
layout(location = 1) in vec2 vert_uv;
layout(location = 2) in vec4 vert_color;
layout(location = 3) in float vert_blend;
layout(location = 4) in float vert_alpha;

layout(location = 0) uniform mat4 u_MVP;

out vec2 frag_uv;
out vec4 frag_color;
out float frag_blend;
out float frag_alpha;

void main()
{																							 
	gl_Position = u_MVP * vec4(vert_pos, 0, 1);
	
	frag_uv = vert_uv;
	frag_color = vert_color;
	frag_blend = vert_blend;
	frag_alpha = vert_alpha;
}