#version 330

layout (points) in;

in VS_OUT {
	vec4 rect;
	float tex;
} gs_in[];

layout (triangle_strip, max_vertices = 4) out;
out vec3 p_uv;
out vec4 p_rect;

uniform mat4 u_MVP;

void main()
{
	vec4 pos1 = u_MVP * vec4(gs_in[0].rect.xy, 0, 1);
	vec4 pos2 = u_MVP * vec4(gs_in[0].rect.zw, 0, 1);

	gl_Position = vec4(pos1.x, pos1.y, pos1.zw);
	p_uv = vec3(0, 0, gs_in[0].tex);
	p_rect = gs_in[0].rect;
	EmitVertex();

	gl_Position = vec4(pos1.x, pos2.y, pos1.zw);
	p_uv = vec3(0, 1, gs_in[0].tex);
	p_rect = gs_in[0].rect;
	EmitVertex();

	gl_Position = vec4(pos2.x, pos1.y, pos1.zw);
	p_uv = vec3(1, 0, gs_in[0].tex);
	p_rect = gs_in[0].rect;
	EmitVertex();

	gl_Position = vec4(pos2.x, pos2.y, pos1.zw);
	p_uv = vec3(1, 1, gs_in[0].tex);
	p_rect = gs_in[0].rect;
	EmitVertex();

	EndPrimitive();
}