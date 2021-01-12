#version 330 core

layout(location = 0) in vec2 i_pos1;
layout(location = 1) in vec2 i_pos2;
layout(location = 2) in vec2 i_cornerSize;
layout(location = 3) in float i_uv;
layout(location = 4) in float i_depth;

out VS_OUT {
	vec4 rect;
	vec2 cornerSize;
	float uv;
	float depth;
} vs_out;

void main()
{
	vs_out.rect = vec4(i_pos1, i_pos2);	
	vs_out.cornerSize = i_cornerSize;
	vs_out.uv = i_uv;
	vs_out.depth = i_depth;	
}
