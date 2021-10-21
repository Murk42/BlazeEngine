#version 330 core												
																
in vec2 frag_uv;			
in vec4 frag_color;
																
uniform sampler2D u_texture;														
																
void main()														
{																
	float distance = texture(u_texture, frag_uv).r;                
																
	vec4 color = frag_color;			                            
	color.a = distance;											
	if (color.a <= 0.0f)                                        
		discard;                                                
	gl_FragColor = color;							            
}																