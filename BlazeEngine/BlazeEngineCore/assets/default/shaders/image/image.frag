#version 450															   
																		   
in vec2 frag_uv;
in vec4 frag_color;	   
in float frag_blend;														   
																		   
layout(location = 1) uniform sampler2D u_texture;
																		   
void main()															   
{																		   
	vec4 color = texture(u_texture, frag_uv);

	color = mix(color, frag_color, frag_blend);
																		   
	gl_FragColor = color;
}																		   