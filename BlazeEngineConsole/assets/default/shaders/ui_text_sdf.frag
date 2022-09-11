#version 450										 
													 
in vec2 frag_uv;							
in vec2 frag_pos;
													 
layout(location = 1) uniform sampler2D u_texture;	 
layout(location = 2) uniform vec4 u_color;			 
layout(location = 3) uniform vec4 u_clipRect;			
													 
void main()										 
{													 
	vec4 color = texture(u_texture, frag_uv);		 

	if (color.a == 0 ||
		(frag_pos.x < u_clipRect.x || frag_pos.x > u_clipRect.z) && u_clipRect.x != u_clipRect.z || 
		(frag_pos.y < u_clipRect.y || frag_pos.y > u_clipRect.w) && u_clipRect.y != u_clipRect.w)								 
		discard;

	if (color.r < 0.45)								  
		discard;									  
	else if (color.r <= 0.50)						  
		color = vec4(u_color.rgb, (color.r - 0.45) / 0.05 * u_color.a);
	else		
		color = u_color;							
													 
	gl_FragColor = color;							 
}													 