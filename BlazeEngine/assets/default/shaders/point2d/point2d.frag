#version 450													  
																  
in vec4 frag_color;											  
in vec2 frag_pos;												  
in vec2 frag_center;											  
in float frag_radius;											  
																  
void main()													  
{																  
	float d = distance(frag_pos, frag_center);					  
	d = smoothstep(frag_radius - 1, frag_radius + 1, d);		  
																  
	gl_FragColor = vec4(frag_color.rgb, mix(frag_color.a, 0, d)); 
}																  