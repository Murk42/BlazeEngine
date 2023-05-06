#version 450

in vec2 frag_uv;
in vec4 frag_color;
in float frag_blend;

layout(location = 1) uniform sampler2D u_texture;

void main()
{	
	vec4 texColor = texture(u_texture, frag_uv);
	vec4 color = mix(frag_color, texColor, frag_blend);	
	gl_FragColor = color;
}