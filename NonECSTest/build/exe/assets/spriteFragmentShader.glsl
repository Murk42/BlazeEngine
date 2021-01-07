#version 330 core

in vec3 p_uv;

uniform sampler2DArray u_texture;

void main()
{	
	gl_FragColor = texture(u_texture, p_uv);
}