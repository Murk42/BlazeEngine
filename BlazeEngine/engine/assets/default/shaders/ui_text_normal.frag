#version 450

in vec2 frag_uv;
in vec2 frag_pos;

layout(location = 2) uniform sampler2D u_texture;
layout(location = 3) uniform vec4 u_color;
layout(location = 4) uniform vec4 u_clipRect;

void main()
{
	vec4 color = texture(u_texture, frag_uv);

	color = vec4(1, 1, 1, color.r) * u_color;
	if (color.a == 0 ||
		(frag_pos.x < u_clipRect.x || frag_pos.x > u_clipRect.x + u_clipRect.z) && !isnan(u_clipRect.z) ||
		(frag_pos.y < u_clipRect.y || frag_pos.y > u_clipRect.y + u_clipRect.w) && !isnan(u_clipRect.w))
		discard;

	gl_FragColor = color;
}