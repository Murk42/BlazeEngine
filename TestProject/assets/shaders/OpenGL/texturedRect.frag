#version 450															   
																		   
in vec2 frag_uv;
in vec4 frag_color;
in float frag_blend;				
in float frag_alpha;

layout(location = 1) uniform sampler2D u_texture;
																		   
void main()															   
{						
	vec4 color = texture(u_texture, frag_uv);

	gl_FragColor = mix(color, frag_color, frag_blend) * vec4(1, 1, 1, frag_alpha);
}