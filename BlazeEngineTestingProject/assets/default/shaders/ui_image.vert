#version 450 

layout(location = 0) in vec3 i_pos;
layout(location = 1) in vec2 i_uv;
layout(location = 2) in vec4 i_color;

layout(location = 0) uniform mat4 u_MVP;

out vec2 frag_pos;
out vec2 frag_uv;
out vec4 frag_color;

void main()
{	
	frag_pos = i_pos.xy;
	frag_uv = i_uv;
	frag_color = i_color;
	gl_Position = u_MVP * vec4(i_pos, 1);
}