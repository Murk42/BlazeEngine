#version 450																  
																			  
in vec2 geom_pos[];														  
in vec4 geom_color[];														  
in float geom_radius[];													  
																			  
out vec4 frag_color;														  
out vec2 frag_pos;															  
out vec2 frag_center;														  
out float frag_radius;														  
																			  
layout(points) in;															  
layout(triangle_strip, max_vertices = 4) out;								  
																			  
layout(location = 0) uniform mat4 u_MVP;									  
																			  
void main()																  
{																			  
	vec2 p1 = geom_pos[0] + vec2(-geom_radius[0] - 1, -geom_radius[0] - 1);	  
	vec2 p2 = geom_pos[0] + vec2(-geom_radius[0] - 1, geom_radius[0] + 1);	  
	vec2 p3 = geom_pos[0] + vec2(geom_radius[0] + 1, -geom_radius[0] - 1);	  
	vec2 p4 = geom_pos[0] + vec2(geom_radius[0] + 1, geom_radius[0] + 1);	  
																			  
	gl_Position = u_MVP * vec4(p1, 0, 1);									  
	frag_pos = p1;															  
	frag_color = geom_color[0];												  
	frag_center = geom_pos[0];												  
	frag_radius = geom_radius[0];											  
	EmitVertex();															  
																			  
	gl_Position = u_MVP * vec4(p2, 0, 1);									  
	frag_pos = p2;															  
	frag_color = geom_color[0];												  
	frag_center = geom_pos[0];												  
	frag_radius = geom_radius[0];											  
	EmitVertex();															  
																			  
	gl_Position = u_MVP * vec4(p3, 0, 1);									  
	frag_pos = p3;															  
	frag_color = geom_color[0];												  
	frag_center = geom_pos[0];												  
	frag_radius = geom_radius[0];											  
	EmitVertex();															  
																			  
	gl_Position = u_MVP * vec4(p4, 0, 1);									  
	frag_pos = p4;															  
	frag_color = geom_color[0];												  
	frag_center = geom_pos[0];												  
	frag_radius = geom_radius[0];											  
	EmitVertex();															  
																			  
	EndPrimitive();															  
}																			  