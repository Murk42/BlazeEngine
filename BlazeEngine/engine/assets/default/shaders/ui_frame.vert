#version 450 

layout(location = 0) in vec3 i_pos;
layout(location = 1) in vec4 i_rect;
layout(location = 2) in vec4 i_fillColor;
layout(location = 3) in vec4 i_borderColor;
layout(location = 4) in float i_radius;
layout(location = 5) in float i_borderWidth;

layout(location = 0) uniform mat4 u_MVP;

out vec2 frag_pos;
out vec4 frag_fillColor;
out vec4 frag_borderColor;
out vec4 frag_rect;
out float frag_radius;
out float frag_borderWidth;

void main()
{
	frag_pos = i_pos.xy;
	frag_rect = i_rect;
	frag_fillColor = i_fillColor;
	frag_borderColor = i_borderColor;
	frag_radius = i_radius;
	frag_borderWidth = i_borderWidth;
	gl_Position = u_MVP * vec4(i_pos, 1);
}