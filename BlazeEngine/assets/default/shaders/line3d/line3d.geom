#version 460														 
																	 
layout(points) in;													 
layout(triangle_strip, max_vertices = 4) out;						 
																	 
in vec3 geom_pos1[];												 
in vec3 geom_pos2[];												 
in vec4 geom_color[];												 
in float geom_width[];												 
																	 
out vec4 frag_color;												 
out vec2 frag_pos;													 
out vec2 frag_pos1;												 
out vec2 frag_pos2;												 
out float frag_width;												 
																	 
layout(location = 0) uniform mat4 u_VP3D;							 
layout(location = 1) uniform vec2 u_viewportSize;					 
																	 
bool isInCube(vec3 pos)											 
{																	 
	return															 
		pos.x >= -1 && pos.x <= 1 &&								 
		pos.y >= -1 && pos.y <= 1 &&								 
		pos.z >= -1 && pos.z <= 1;									 
}																	 
																	 
void swap(inout vec3 a, inout vec3 b)								 
{																	 
	vec3 t = a;														 
	a = b;															 
	b = t;															 
}																	 
																	 
void main()														 
{																	 
	vec4 p1 = u_VP3D * vec4(geom_pos1[0], 1);						 
	vec4 p2 = u_VP3D * vec4(geom_pos2[0], 1);						 
																	 
	vec3 ndc1 = p1.xyz / p1.w;										 
	vec3 ndc2 = p2.xyz / p2.w;										 
																	 
	bool inside1 = isInCube(ndc1);									 
	bool inside2 = isInCube(ndc2);									 
																	 
	if (!inside1 || !inside2)										 
	{																 
		if (ndc2.x < ndc1.x)										 
			swap(ndc1, ndc2);										 
																	 
		vec3 vec = normalize(ndc2 - ndc1);							 
																	 
		if (ndc2.x < -1 || ndc1.x > 1) return;						 
		if (ndc1.x < -1) ndc1 = ndc1 + vec * (-ndc1.x - 1);			 
		if (ndc2.x > 1) ndc2 = ndc2 - vec * (ndc2.x - 1);			 
																	 
		if (ndc2.y < -1 || ndc1.y > 1) return;						 
		if (ndc1.y < -1) ndc1 = ndc1 + vec * (-ndc1.y - 1);			 
		if (ndc2.y > 1) ndc2 = ndc2 - vec * (ndc2.y - 1);			 
																	 
		if (ndc2.z < -1 || ndc1.z > 1) return;						 
		if (ndc1.z < -1) ndc1 = ndc1 + vec * (-ndc1.z - 1);			 
		if (ndc2.z > 1) ndc2 = ndc2 - vec * (ndc2.z - 1);			 
	}																 
																	 
	vec2 sp1 = (ndc1.xy * .5f + .5f) * u_viewportSize;				 
	vec2 sp2 = (ndc2.xy * .5f + .5f) * u_viewportSize;				 
																	 
	vec2 dir = normalize(sp2.xy - sp1.xy);							 
	vec2 normal = vec2(-dir.y, dir.x);								 
																	 
	vec2 pos;														 
																	 
	pos = sp1 + (normal - dir) * geom_width[0];						 
	frag_color = geom_color[0];										 
	frag_pos = pos;													 
	frag_pos1 = sp1.xy;												 
	frag_pos2 = sp2.xy;												 
	frag_width = geom_width[0];										 
	gl_Position = vec4(vec2(pos / u_viewportSize * 2 - 1), 0, 1);	 
	EmitVertex();													 
																	 
	pos = sp2 + (normal + dir) * geom_width[0];						 
	frag_color = geom_color[0];										 
	frag_pos = pos;													 
	frag_pos1 = sp1.xy;												 
	frag_pos2 = sp2.xy;												 
	frag_width = geom_width[0];										 
	gl_Position = vec4(vec2(pos / u_viewportSize * 2 - 1), 0, 1);	 
	EmitVertex();													 
																	 
	pos = sp1 + (-normal + dir) * geom_width[0];					 
	frag_color = geom_color[0];										 
	frag_pos = pos;													 
	frag_pos1 = sp1.xy;												 
	frag_pos2 = sp2.xy;												 
	frag_width = geom_width[0];										 
	gl_Position = vec4(vec2(pos / u_viewportSize * 2 - 1), 0, 1);	 
	EmitVertex();													 
																	 
	pos = sp2 + (-normal + dir) * geom_width[0];					 
	frag_color = geom_color[0];										 
	frag_pos = pos;													 
	frag_pos1 = sp1.xy;												 
	frag_pos2 = sp2.xy;												 
	frag_width = geom_width[0];										 
	gl_Position = vec4(vec2(pos / u_viewportSize * 2 - 1), 0, 1);	 
	EmitVertex();													 
																	 
	EndPrimitive();													 
}																	 