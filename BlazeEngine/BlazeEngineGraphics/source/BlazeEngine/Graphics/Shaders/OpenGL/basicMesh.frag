#version 450

in float frag_dot;
in vec4 frag_color;
out vec4 out_color;

layout(location = 3) uniform vec3 u_ambientColor;

void main()
{
	out_color = vec4(mix(u_ambientColor, frag_color.rgb, clamp(frag_dot, 0, 1)), frag_color.a);
}