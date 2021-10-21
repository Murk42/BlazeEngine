#version 400

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 norm;

out vec2 tesc_uv;
out vec3 tesc_norm;

void main()
{
	tesc_uv = uv;
	tesc_norm = norm;
	gl_Position = vec4(pos, 1);
}