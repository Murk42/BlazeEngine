#version 330

layout (points) in;

in VS_OUT {
	mat4 mat;
	float tex;
} gs_in[];

layout (triangle_strip, max_vertices = 4) out;
out vec3 p_uv;

void main()
{
	gl_Position = gs_in[0].mat * vec4(1, 0, 0, 1);
	p_uv = vec3(1, 0, gs_in[0].tex);
	EmitVertex();
	gl_Position = gs_in[0].mat * vec4(0, 0, 0, 1);
	p_uv = vec3(0, 0, gs_in[0].tex);
	EmitVertex();
	gl_Position = gs_in[0].mat * vec4(1, 1, 0, 1);
	p_uv = vec3(1, 1, gs_in[0].tex);
	EmitVertex();
	gl_Position = gs_in[0].mat * vec4(0, 1, 0, 1);
	p_uv = vec3(0, 1, gs_in[0].tex);
	EmitVertex();
	EndPrimitive();
}