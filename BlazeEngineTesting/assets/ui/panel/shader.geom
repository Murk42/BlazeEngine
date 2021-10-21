#version 330

in mat4 geom_MVP[];
in vec2 geom_size[];
in vec4 geom_color[];
in vec4 geom_outlineColor[];
in float geom_radius[];
in float geom_thickness[];

out vec2 frag_size;
out vec2 frag_pos;
out vec4 frag_color;
out vec4 frag_outlineColor;
out float frag_radius;
out float frag_thickness;

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

void main()
{		
	const float overdraw = 5;
	gl_Position = geom_MVP[0] * vec4(0 - overdraw / geom_size[0].x, 0 - overdraw / geom_size[0].y, 0, 1);		
	frag_color = geom_color[0];
	frag_size = geom_size[0];
	frag_pos = vec2(-overdraw, -overdraw);
	frag_outlineColor = geom_outlineColor[0];
	frag_radius = geom_radius[0];
	frag_thickness = geom_thickness[0];
	EmitVertex();
	
	gl_Position = geom_MVP[0] * vec4(1 + overdraw / geom_size[0].x, 0 - overdraw / geom_size[0].y, 0, 1);		
	frag_color = geom_color[0];
	frag_size = geom_size[0];
	frag_pos = vec2(geom_size[0].x + overdraw, -overdraw);
	frag_outlineColor = geom_outlineColor[0];
	frag_radius = geom_radius[0];
	frag_thickness = geom_thickness[0];
	EmitVertex();
	
	gl_Position = geom_MVP[0] * vec4(0 - overdraw / geom_size[0].x, 1 + overdraw / geom_size[0].y, 0, 1);		
	frag_color = geom_color[0];
	frag_size = geom_size[0];
	frag_pos = vec2(-overdraw, geom_size[0].y + overdraw);
	frag_outlineColor = geom_outlineColor[0];
	frag_radius = geom_radius[0];
	frag_thickness = geom_thickness[0];
	EmitVertex();
	
	gl_Position = geom_MVP[0] * vec4(1 + overdraw / geom_size[0].x, 1 + overdraw / geom_size[0].y, 0, 1);		
	frag_color = geom_color[0];	
	frag_size = geom_size[0];
	frag_pos = geom_size[0] + vec2(overdraw, overdraw);
	frag_outlineColor = geom_outlineColor[0];
	frag_radius = geom_radius[0];
	frag_thickness = geom_thickness[0];
	EmitVertex();	

	EndPrimitive();
}