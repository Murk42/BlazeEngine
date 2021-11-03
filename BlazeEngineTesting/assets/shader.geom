#version 450

in vec3 geom_pos[];
in vec4 geom_outPos[];
out vec3 frag_normal;

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 u_MVP;

void main()
{
	vec3 a = geom_pos[1] - geom_pos[0]; 
	vec3 b = geom_pos[2] - geom_pos[0]; 
	vec3 normal = normalize(cross(a, b));

	frag_normal = normal;
	gl_Position = geom_outPos[0];
	EmitVertex();

	frag_normal = normal;
	//gl_Position = u_MVP * vec4(geom_pos[0] + a * 0.1, 1);
	gl_Position = geom_outPos[1];
	EmitVertex();

	frag_normal = normal;
	//gl_Position = u_MVP * vec4(geom_pos[0] + normal * 0.1, 1);	
	gl_Position = geom_outPos[2];
	EmitVertex();

	EndPrimitive();
}