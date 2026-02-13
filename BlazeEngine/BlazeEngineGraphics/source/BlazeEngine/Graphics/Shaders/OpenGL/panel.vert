#version 450 

layout(location = 0) in vec4 i_fillColor;
layout(location = 1) in vec4 i_borderColor;
layout(location = 2) in vec2 i_p1;
layout(location = 3) in vec2 i_p2;
layout(location = 4) in vec2 i_p3;
layout(location = 5) in float i_borderWidth;
layout(location = 6) in float i_cornerRadius;

layout(location = 0) uniform mat4 u_MVP;

out vec4 frag_fillColor;
out vec4 frag_borderColor;
out vec2 frag_pos;
out vec2 frag_p1;
out vec2 frag_p2;
out vec2 frag_p3;
out float frag_borderWidth;
out float frag_cornerRadius;

void main()
{	
	vec4 selector = vec4(gl_VertexID == 0, gl_VertexID == 1, gl_VertexID == 2, gl_VertexID == 3);
	vec2 pos = vec2(
		dot(selector, vec4(i_p1.x, i_p2.x, i_p3.x, i_p2.x + i_p3.x - i_p1.x)),
		dot(selector, vec4(i_p1.y, i_p2.y, i_p3.y, i_p2.y + i_p3.y - i_p1.y))
	);

	gl_Position = u_MVP * vec4(pos, 0, 1);

	frag_pos = pos;
	frag_fillColor = i_fillColor;
	frag_borderColor = i_borderColor;
	frag_p1 = i_p1;
	frag_p2 = i_p2;
	frag_p3 = i_p3;	
	frag_borderWidth = i_borderWidth;
	frag_cornerRadius = i_cornerRadius;
}