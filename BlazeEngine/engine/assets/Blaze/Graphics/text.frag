#version 450

in vec2 frag_uv;

layout(location = 1) uniform sampler2D u_texture;
layout(location = 2) uniform vec4 u_color;

void main()
{		
	vec4 color = texture(u_texture, frag_uv);
	
	color = vec4(1, 1, 1, color.r) * u_color;			
	if (color.a == 0)
		discard;

	gl_FragColor = color;	
}