#version 450

in vec3 geom_pos1[];
in vec3 geom_pos2[];
in vec4 geom_color[];
in float geom_width[];

out vec4 frag_color;
out vec2 frag_pos;
out vec2 frag_pos1;
out vec2 frag_pos2;
out float frag_width;

layout(points) in;
layout(line_strip, max_vertices = 2) out;

uniform mat4 u_VP3D;
uniform mat4 u_proj2D;
uniform vec2 u_viewportSize;

void main()
{
	//vec4 proj1 = u_VP3D * vec4(geom_pos1[0], 1);
	//vec4 proj2 = u_VP3D * vec4(geom_pos2[0], 1);
	//vec2 screenPos1 = (proj1.xy / proj1.w + 1.0f) * u_viewportSize / 2;
	//vec2 screenPos2 = (proj2.xy / proj2.w + 1.0f) * u_viewportSize / 2;	
	//
	//vec2 dir = screenPos2.xy - screenPos1.xy;
	//float len = length(dir);
	//dir = dir / len;
	//vec2 normal = vec2(dir.y, -dir.x);	
	//float half_width = geom_width[0] / 2;
	//
	//vec2 pos;
	//pos = screenPos1.xy - dir * half_width - normal * half_width;
	//gl_Position = proj1;
	//frag_color = geom_color[0];	
	//frag_pos = pos;
	//frag_pos1 = screenPos1.xy;
	//frag_pos2 = screenPos2.xy;
	//frag_width = half_width;
	//EmitVertex();
	//
	//pos = screenPos1.xy + dir * (half_width + len) - normal * half_width;
	//gl_Position = proj2;
	//frag_color = geom_color[0];
	//frag_pos = pos;
	//frag_pos1 = screenPos1.xy;
	//frag_pos2 = screenPos2.xy;
	//frag_width = half_width;
	//EmitVertex();
	
	//pos = screenPos1.xy - dir * half_width + normal * half_width;
	//gl_Position = proj1;
	//frag_color = geom_color[0];
	//frag_pos = pos;
	//frag_pos1 = screenPos1.xy;
	//frag_pos2 = screenPos2.xy;
	//frag_width = half_width;
	//EmitVertex();
	//
	//pos = screenPos1.xy + dir * (half_width + len) + normal * half_width;
	//gl_Position = proj2;
	//frag_color = geom_color[0];
	//frag_pos = pos;
	//frag_pos1 = screenPos1.xy;
	//frag_pos2 = screenPos2.xy;
	//frag_width = half_width;
	//EmitVertex();

	//EndPrimitive();

	gl_Position = u_VP3D * vec4(geom_pos1[0], 1);
	frag_color = geom_color[0];
	EmitVertex();
	gl_Position = u_VP3D * vec4(geom_pos2[0], 1);
	frag_color = geom_color[0];
	EmitVertex();

	EndPrimitive();
}