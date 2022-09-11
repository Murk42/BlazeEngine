#version 330 core

in vec2 p_uv;
in float p_layer;

uniform sampler2DArray u_texture;
uniform vec4 u_color;

void main()
{		
	vec4 color = texture(u_texture, vec3(p_uv, p_layer));
	
	color = vec4(1, 1, 1, color.r) * u_color;			
	if (color.a == 0)
		discard;
	gl_FragColor = color;	
}