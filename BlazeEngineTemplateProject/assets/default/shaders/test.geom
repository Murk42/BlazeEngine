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

void main()
{		
	vec4 p1 = u_VP3D * vec4(geom_pos1[0], 1);
	vec4 p2 = u_VP3D * vec4(geom_pos2[0], 1);
	vec2 sp1 = (p1.xy * .5f + .5f) * u_viewportSize;
	vec2 sp2 = (p2.xy * .5f + .5f) * u_viewportSize;

	vec2 dir = normalize(sp2.xy - sp1.xy);
	vec2 normal = vec2(-dir.y, dir.x);

	frag_color = geom_color[0];
	frag_pos = sp1 + (normal - dir) * geom_width[0];
	frag_pos1 = p1.xy;
	frag_pos2 = p2.xy;
	frag_width = geom_width[0];
	EmitVertex();

	frag_color = geom_color[0];
	frag_pos = sp2 + (normal + dir) * geom_width[0];
	frag_pos1 = p1.xy;
	frag_pos2 = p2.xy;
	frag_width = geom_width[0];
	EmitVertex();

	frag_color = geom_color[0];
	frag_pos = sp1 + (-normal - dir) * geom_width[0];
	frag_pos1 = p1.xy;
	frag_pos2 = p2.xy;
	frag_width = geom_width[0];
	EmitVertex();

	frag_color = geom_color[0];
	frag_pos = sp2 + (-normal + dir) * geom_width[0];
	frag_pos1 = p1.xy;
	frag_pos2 = p2.xy;
	frag_width = geom_width[0];
	EmitVertex();

	EndPrimitive();
}