#version 330 core

layout(location = 0) in vec4 i_vec0;
layout(location = 1) in vec4 i_vec1;
layout(location = 2) in vec4 i_vec2;
layout(location = 3) in vec4 i_vec3;
layout(location = 4) in float i_tex;

out VS_OUT {
	mat4 mat;
	float tex;
} vs_out;

uniform mat4 u_VP;

void main()
{
	vs_out.mat = u_VP * mat4(i_vec0, i_vec1, i_vec2, i_vec3);
	vs_out.tex = i_tex;
}
