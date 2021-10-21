#version 330

in vec2 geom_uv[];
in vec3 geom_norm[];
in vec4 geom_pos[];

out vec2 frag_uv;
out mat3 frag_TBN;

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 u_MVP;
uniform mat4 u_rotation;

void main()
{
	vec3 edge1 = geom_pos[1].xyz - geom_pos[0].xyz;
	vec3 edge2 = geom_pos[2].xyz - geom_pos[0].xyz;
	vec2 deltaUV1 = geom_uv[1] - geom_uv[0];
	vec2 deltaUV2 = geom_uv[2] - geom_uv[0];  
	
	vec3 tangent;
	vec3 bitangent;	

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
	
	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	
	bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);


	vec3 T = vec3(u_rotation * vec4(tangent,   0.0));
	vec3 B = vec3(u_rotation * vec4(bitangent, 0.0));	

	frag_uv = geom_uv[0];	
	frag_TBN = mat3(T, B, vec3(u_rotation * vec4(geom_norm[0], 0.0)));
	gl_Position = u_MVP * geom_pos[0];
	EmitVertex();

	frag_uv = geom_uv[1];	
	frag_TBN = mat3(T, B, vec3(u_rotation * vec4(geom_norm[1], 0.0)));
	gl_Position = u_MVP * geom_pos[1];
	EmitVertex();

	frag_uv = geom_uv[2];
	frag_TBN = mat3(T, B, vec3(u_rotation * vec4(geom_norm[2], 0.0)));
	gl_Position = u_MVP * geom_pos[2];
	EmitVertex();

	EndPrimitive();
}