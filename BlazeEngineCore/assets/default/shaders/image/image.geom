#version 450																			 
																						 
in mat4 geom_matrix[];
in vec2 geom_uv[][4];
in vec2 geom_size[];
in vec4 geom_color[];
in float geom_blend[];

out vec2 frag_uv;																					 
out vec4 frag_color;																	 
out float frag_blend;
																						 
layout(points) in;																		 
layout(triangle_strip, max_vertices = 4) out;											 
																						 
layout(location = 0) uniform mat4 u_proj;												 
																						 
void main()																			 
{
	vec4 pos1 = geom_matrix[0] * vec4(             0,              0, 0, 1);
	vec4 pos2 = geom_matrix[0] * vec4(             0, geom_size[0].y, 0, 1);
	vec4 pos3 = geom_matrix[0] * vec4(geom_size[0].x,              0, 0, 1);
	vec4 pos4 = geom_matrix[0] * vec4(geom_size[0].x, geom_size[0].y, 0, 1);

	gl_Position = u_proj * pos1;
	frag_uv = geom_uv[0][0];	
	frag_color = geom_color[0];		
	frag_blend = geom_blend[0];
	EmitVertex();																		 
																						 
	gl_Position = u_proj * pos2;
	frag_uv = geom_uv[0][1];	
	frag_color = geom_color[0];		
	frag_blend = geom_blend[0];
	EmitVertex();		

	gl_Position = u_proj * pos3;
	frag_uv = geom_uv[0][2];	
	frag_color = geom_color[0];		
	frag_blend = geom_blend[0];
	EmitVertex();		

	gl_Position = u_proj * pos4;
	frag_uv = geom_uv[0][3];	
	frag_color = geom_color[0];		
	frag_blend = geom_blend[0];
	EmitVertex();		
																						 
	EndPrimitive();																		 
}																						 