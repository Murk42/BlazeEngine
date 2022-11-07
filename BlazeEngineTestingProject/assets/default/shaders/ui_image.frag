#version 450

in vec2 frag_pos;
in vec2 frag_uv;
in vec4 frag_color;

layout(location = 1) uniform sampler2D u_texture;
layout(location = 2) uniform vec4 u_clipRect;

void main()
{	
	vec4 color = texture(u_texture, frag_uv / textureSize(u_texture, 0));

	if (color.a == 0 ||
		(frag_pos.x < u_clipRect.x || frag_pos.x > u_clipRect.x + u_clipRect.z) && !isnan(u_clipRect.z) ||
		(frag_pos.y < u_clipRect.y || frag_pos.y > u_clipRect.y + u_clipRect.w) && !isnan(u_clipRect.w))
		discard;


	gl_FragColor = vec4(mix(color.rgb, frag_color.rgb, frag_color.a), color.a);	
}