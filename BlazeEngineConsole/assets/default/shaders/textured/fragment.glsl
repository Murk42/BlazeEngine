#version 330 core

in vec2 p_uv;

uniform sampler2D u_texture;

void main()
{		
	vec4 color = texture(u_texture, p_uv);
				
	gl_FragColor = color;	
}