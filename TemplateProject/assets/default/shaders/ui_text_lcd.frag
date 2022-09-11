#version 450										 
													 
in vec2 frag_uv;							
in vec2 frag_pos;
													 
layout(location = 3) uniform sampler2D u_texture;	 
layout(location = 4) uniform vec4 u_color;			 
layout(location = 5) uniform vec4 u_clipRect;			
													 
void main()										 
{													 
	vec4 color = texture(u_texture, frag_uv);		 

	float l = (color.r * color.r + color.g * color.g + color.b * color.b) / 3;
	l = pow(l, 0.5);

	color = vec4(mix(color.rgb, u_color.rgb, l), 1);	   					  
	if (color.rgb == vec3(0, 0, 0))					  
		discard;									 
													 
	gl_FragColor = color;							 
}													 