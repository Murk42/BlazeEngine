#version 450																			 
							
in mat4  geom_mat[];
in vec4  geom_outerColor[];
in vec4  geom_innerColor[];
in float geom_innerRadius[];
																						 
out vec4  frag_outerColor;
out vec4  frag_innerColor;
out float frag_outerRadius;
out float frag_innerRadius;
out vec2  frag_center;
out vec2  frag_pos;
																						 
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

layout(location = 0) uniform mat4 u_proj;

void main()
{																							 
	vec4 pos1 = geom_mat[0] * vec4(-1, -1, 0, 1);
	vec4 pos2 = geom_mat[0] * vec4(-1, 1, 0, 1);
	vec4 pos3 = geom_mat[0] * vec4( 1,-1, 0, 1);
	vec4 pos4 = geom_mat[0] * vec4( 1,  1, 0, 1);

	vec2 center = (pos1.xy + pos4.xy) / 2;
	float radius = distance(pos1, pos2) / 2;		

	pos1.xy = center + vec2(-radius - 2, -radius - 2);
	pos2.xy = center + vec2(-radius - 2,  radius + 2);
	pos3.xy = center + vec2( radius + 2, -radius - 2);
	pos4.xy = center + vec2( radius + 2,  radius + 2);
																					 
	gl_Position = u_proj * pos1;
	frag_pos = pos1.xy;
	frag_outerColor = geom_outerColor[0];
	frag_innerColor = geom_innerColor[0];
	frag_outerRadius = radius;
	frag_innerRadius = geom_innerRadius[0];
	frag_center = center;	
	EmitVertex();
																						 
	gl_Position = u_proj * pos2;
	frag_pos = pos2.xy;																	 
	frag_outerColor = geom_outerColor[0];
	frag_innerColor = geom_innerColor[0];
	frag_outerRadius = radius;
	frag_innerRadius = geom_innerRadius[0];
	frag_center = center;
	EmitVertex();

	gl_Position = u_proj * pos3;
	frag_pos = pos3.xy;																	 
	frag_outerColor = geom_outerColor[0];
	frag_innerColor = geom_innerColor[0];
	frag_outerRadius = radius;
	frag_innerRadius = geom_innerRadius[0];
	frag_center = center;
	EmitVertex();

	gl_Position = u_proj * pos4;
	frag_pos = pos4.xy;																	 
	frag_outerColor = geom_outerColor[0];
	frag_innerColor = geom_innerColor[0];
	frag_outerRadius = radius;
	frag_innerRadius = geom_innerRadius[0];
	frag_center = center;
	EmitVertex();
																						 
	EndPrimitive();																		 
}																						 