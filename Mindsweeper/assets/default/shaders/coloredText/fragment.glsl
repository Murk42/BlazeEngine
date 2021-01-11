#version 330 core

in vec2 p_uv;
in vec4 p_color;

uniform sampler2D u_texture;

void main()
{		
	vec4 color = texture(u_texture, p_uv);
	
	color.a = color.r;
	color.rgb = vec3(1, 1, 1);
	color *= p_color;
		
	gl_FragColor = color;	
}