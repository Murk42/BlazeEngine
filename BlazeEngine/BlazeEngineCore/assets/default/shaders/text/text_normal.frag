#version 450

in vec2 frag_uv;
in vec2 frag_pos;
in vec4 frag_color;

layout(location = 2) uniform sampler2D u_texture;

void main()
{
	vec4 color = texture(u_texture, frag_uv);

	color = vec4(1, 1, 1, color.r) * frag_color;	
	gl_FragColor = color;
}