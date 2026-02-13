#version 450

in vec2 frag_uv;
in vec2 frag_pos;
in vec4 frag_color;

layout(location = 2) uniform sampler2D u_texture;

void main()
{
	vec4 color = texture(u_texture, frag_uv);

	float l = (color.r * color.r + color.g * color.g + color.b * color.b) / 3;
	l = pow(l, 0.5);


	color = vec4(mix(color.rgb, frag_color.rgb, l), 1);	

	gl_FragColor = color;
}