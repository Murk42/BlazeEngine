#version 450															  
																		  
layout(points) in;														  
layout(triangle_strip, max_vertices = 4) out;							  
																		  
in vec2 geom_pos1[];													  
in vec2 geom_pos2[];													  
in vec2 geom_uv1[];													  
in vec2 geom_uv2[];													  
																		  
out vec2 frag_uv;				
out vec2 frag_pos;
																		  
layout(location = 0) uniform mat4 u_MVP;								  
layout(location = 1) uniform vec2 u_scaling;								  
layout(location = 2) uniform float u_depth;
																		  
void main()															  
{																		  
	gl_Position = u_MVP * vec4(u_scaling * vec2(geom_pos1[0].x, geom_pos1[0].y), u_depth, 1);
	frag_uv = vec2(geom_uv1[0].x, geom_uv1[0].y);						  
	frag_pos = u_scaling * vec2(geom_pos1[0].x, geom_pos1[0].y);
	EmitVertex();														  
																		  
	gl_Position = u_MVP * vec4(u_scaling * vec2(geom_pos1[0].x, geom_pos2[0].y), u_depth, 1);
	frag_uv = vec2(geom_uv1[0].x, geom_uv2[0].y);						  
	frag_pos = u_scaling * vec2(geom_pos1[0].x, geom_pos2[0].y);
	EmitVertex();														  
																		  
	gl_Position = u_MVP * vec4(u_scaling * vec2(geom_pos2[0].x, geom_pos1[0].y), u_depth, 1);
	frag_uv = vec2(geom_uv2[0].x, geom_uv1[0].y);		
	frag_pos = u_scaling * vec2(geom_pos2[0].x, geom_pos1[0].y);
	EmitVertex();														  
																		  
	gl_Position = u_MVP * vec4(u_scaling * vec2(geom_pos2[0].x, geom_pos2[0].y), u_depth, 1);
	frag_uv = vec2(geom_uv2[0].x, geom_uv2[0].y);	
	frag_pos = u_scaling * vec2(geom_pos2[0].x, geom_pos2[0].y);
	EmitVertex();														  
																		  
	EndPrimitive();														  
}																		  