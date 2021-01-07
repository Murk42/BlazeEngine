#version 330 core

in vec2 p_pos;
in vec2 p_uv;
in vec4 p_rect;

uniform sampler2D u_texture;

void main()
{		
	vec4 color = texture(u_texture, p_uv);
	color = vec4(color.r, color.r, color.r, 1);
	if (color.r == 0)
		discard;
	else
		gl_FragColor = color;	
}