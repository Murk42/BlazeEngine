#version 450

in float frag_blend;
in float frag_alpha;
in float frag_textureIndex;
in vec2 frag_uv;
in vec4 frag_color;

out vec4 out_color;

layout(location = 1) uniform sampler2D u_texture1;
layout(location = 2) uniform sampler2D u_texture2;
layout(location = 3) uniform sampler2D u_texture3;
layout(location = 4) uniform sampler2D u_texture4;
layout(location = 5) uniform sampler2D u_texture5;
layout(location = 6) uniform sampler2D u_texture6;
layout(location = 7) uniform sampler2D u_texture7;
layout(location = 8) uniform sampler2D u_texture8;

void main()
{
	vec4 color = vec4(0, 0, 0, 1);

	if (frag_textureIndex < 0.5f)
		color = texture(u_texture1, frag_uv);
	else if (frag_textureIndex  < 1.5f)
		color = texture(u_texture2, frag_uv);
	else if (frag_textureIndex  < 2.5f)
		color = texture(u_texture3, frag_uv);
	else if (frag_textureIndex  < 3.5f)
		color = texture(u_texture4, frag_uv);
	else if (frag_textureIndex  < 4.5f)
		color = texture(u_texture5, frag_uv);
	else if (frag_textureIndex  < 5.5f)
		color = texture(u_texture6, frag_uv);
	else if (frag_textureIndex  < 6.5f)
		color = texture(u_texture7, frag_uv);
	else if (frag_textureIndex  < 7.5f)
		color = texture(u_texture8, frag_uv);

	if (color.a == 0)
		discard;

	out_color = vec4(mix(color.rgb, frag_color.rgb, frag_blend), color.a * frag_color.a * frag_alpha);
}