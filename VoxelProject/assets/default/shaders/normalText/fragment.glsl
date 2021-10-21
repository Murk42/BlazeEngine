#version 330 core

in vec2 p_uv;

uniform sampler2D u_texture;
uniform vec4 u_color;

void main()
{		
	vec4 color = texture(u_texture, p_uv);
	
	color.a = color.r;
	color.rgb = vec3(1, 1, 1);
	color *= u_color;	
		
	gl_FragColor = color;	
}