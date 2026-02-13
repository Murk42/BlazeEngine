#version 450										 
													 
in vec2 frag_uv;							
in vec2 frag_pos;
in vec4 frag_color;

layout(location = 2) uniform sampler2D u_texture;
													 
void main()										 
{													 
	vec4 color = texture(u_texture, frag_uv);		 

	if (color.r < 0.45)								  
		discard;									  
	else if (color.r <= 0.50)						  
		color = vec4(frag_color.rgb, (color.r - 0.45) / 0.05 * frag_color.a);
	else		
		color = frag_color;							
													 
	gl_FragColor = color;							 
}													 