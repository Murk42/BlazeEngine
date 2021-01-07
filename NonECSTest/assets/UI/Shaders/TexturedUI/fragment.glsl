#version 330 core

in vec2 p_pos;
in vec3 p_uv;
in vec4 p_rect;

uniform float u_radius;
uniform sampler2DArray u_texture;

void main()
{	

	if ( p_pos.x < p_rect.x + u_radius && (
		(p_pos.y < p_rect.y + u_radius && distance(p_pos, p_rect.xy + vec2(u_radius, u_radius)) > u_radius) ||			
		(p_pos.y > p_rect.w - u_radius && distance(p_pos, p_rect.xw + vec2(u_radius,-u_radius)) > u_radius)) ||
		 p_pos.x > p_rect.z - u_radius && (
		(p_pos.y < p_rect.y + u_radius && distance(p_pos, p_rect.zy + vec2(-u_radius, u_radius)) > u_radius) ||			
		(p_pos.y > p_rect.w - u_radius && distance(p_pos, p_rect.zw + vec2(-u_radius,-u_radius)) > u_radius)))
			discard;	
	gl_FragColor = texture(u_texture, p_uv);	
}