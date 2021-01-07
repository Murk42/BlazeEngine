#version 330 core

in vec2 p_uv;

uniform sampler2D u_texture;

void main()
{
	vec4 color = texture(u_texture, p_uv);
	if (color.a == 0.0f)
		discard;
	else	
		gl_FragColor = color;
}