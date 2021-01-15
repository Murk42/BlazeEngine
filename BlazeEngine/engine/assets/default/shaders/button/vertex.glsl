#version 330 core

layout(location = 0) in vec2 i_size;
layout(location = 1) in vec4 i_textureCenterRect;
layout(location = 2) in float i_uv;
layout(location = 3) in float i_depth;

out VS_OUT {
	vec2 size;
	vec4 textureCenterRect;
	float uv;
	float depth;
} vs_out;

void main()
{
	vs_out.size = i_size;
	vs_out.textureCenterRect = i_textureCenterRect;
	vs_out.uv = i_uv;
	vs_out.depth = i_depth;	
}
