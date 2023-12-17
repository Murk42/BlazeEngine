#version 450 core

layout(location = 1) uniform sampler2D u_texture;

in vec2 frag_uv;
in vec4 frag_color;

void main()
{	
	gl_FragColor = frag_color * texture(u_texture, frag_uv).r;
}	