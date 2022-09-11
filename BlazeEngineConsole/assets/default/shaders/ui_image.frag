#version 450

in vec2 frag_uv;
in vec4 frag_color;

layout(location = 1) uniform sampler2D u_texture;

void main()
{
	if (frag_color.a == 1)
		gl_FragColor = frag_color;
	else
	{
		vec4 color = texture(u_texture, frag_uv / textureSize(u_texture, 0));
		gl_FragColor = vec4(mix(color.rgb, frag_color.rgb, frag_color.a), color.a);
	}
}