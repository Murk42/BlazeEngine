#version 450 core
 
layout(location = 0) in vec4 i_color;
layout(location = 1) in vec2 i_p1;
layout(location = 2) in vec2 i_p2;
layout(location = 3) in vec2 i_p3;
layout(location = 4) in vec2 i_uv1;
layout(location = 5) in vec2 i_uv2;
layout(location = 6) in float i_blend;
layout(location = 7) in float i_alpha;
layout(location = 8) in float i_textureIndex;

layout(location = 0) uniform mat4 u_proj;
 
out float frag_blend;
out float frag_alpha; 
out float frag_textureIndex; 
out vec2 frag_uv;
out vec4 frag_color;  
 
void main() 
{
	vec4 selector = vec4(gl_VertexID == 0, gl_VertexID == 1, gl_VertexID == 2, gl_VertexID == 3);
	vec2 pos = vec2(
		dot(selector, vec4(i_p1.x, i_p2.x, i_p3.x, i_p2.x + i_p3.x - i_p1.x)),
		dot(selector, vec4(i_p1.y, i_p2.y, i_p3.y, i_p2.y + i_p3.y - i_p1.y))
	);
	vec2 uv = vec2(
		dot(selector, vec4(i_uv1.x, i_uv2.x, i_uv1.x, i_uv2.x)),
		dot(selector, vec4(i_uv1.y, i_uv1.y, i_uv2.y, i_uv2.y))
	);

	gl_Position = u_proj * vec4(pos, 0, 1);


	frag_blend = i_blend;
	frag_alpha = i_alpha;
	frag_textureIndex = i_textureIndex;
	frag_uv = uv;
	frag_color = i_color;
}