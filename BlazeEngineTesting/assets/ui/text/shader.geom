#version 330															
																		
in vec4 geom_pos1[];													
in vec4 geom_pos2[];
in vec2 geom_uv1[];
in vec2 geom_uv2[];
in vec4 geom_color[];
																		
layout (points) in;														
layout (triangle_strip, max_vertices = 4) out;							
																		
out vec2 frag_uv;	
out vec4 frag_color;																												
																		
void main()																
{																		
	gl_Position = vec4(geom_pos1[0].x, geom_pos1[0].y, geom_pos1[0].zw);	
	frag_uv = vec2(geom_uv1[0].x, geom_uv1[0].y);				
	frag_color = geom_color[0];
	EmitVertex();														
																		
	gl_Position = vec4(geom_pos2[0].x, geom_pos1[0].y, geom_pos1[0].zw);	
	frag_uv = vec2(geom_uv2[0].x, geom_uv1[0].y);				
	frag_color = geom_color[0];
	EmitVertex();														
																		
	gl_Position = vec4(geom_pos1[0].x, geom_pos2[0].y, geom_pos1[0].zw);	
	frag_uv = vec2(geom_uv1[0].x, geom_uv2[0].y);						
	frag_color = geom_color[0];
	EmitVertex();														
																		
	gl_Position = vec4(geom_pos2[0].x, geom_pos2[0].y, geom_pos1[0].zw);	
	frag_uv = vec2(geom_uv2[0].x, geom_uv2[0].y);						
	frag_color = geom_color[0];
	EmitVertex();														
																		
	EndPrimitive();														
}																		