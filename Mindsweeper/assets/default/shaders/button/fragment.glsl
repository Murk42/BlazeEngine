#version 330 core

in vec2 p_pos;
in vec2 p_size;
in vec2 p_cornerSize;
in vec3 p_uv;

uniform sampler2DArray u_texture;

void main()
{		
	vec2 size = textureSize(u_texture, 0).xy;
	vec2 uv;
	

	if (p_pos.x < p_cornerSize.x)
		uv.x = p_pos.x / size.x;	
	else if (p_pos.x < p_size.x - p_cornerSize.x)
		uv.x = (p_cornerSize.x + (p_pos.x - p_cornerSize.x) / (p_size.x - 2 * p_cornerSize.x) * (size.x - 2 * p_cornerSize.x)) / size.x;
	else
		uv.x = (size.x - p_cornerSize.x + (p_pos.x - p_size.x + p_cornerSize.x)) / size.x;
	
	
	if (p_pos.y < p_cornerSize.y)
		uv.y = p_pos.y / size.y;	
	else if (p_pos.y < p_size.y - p_cornerSize.y)
		uv.y = (p_cornerSize.y + (p_pos.y - p_cornerSize.y) / (p_size.y - 2 * p_cornerSize.y) * (size.y - 2 * p_cornerSize.y)) / size.y;
	else
		uv.y = (size.y - p_cornerSize.y + (p_pos.y - p_size.y + p_cornerSize.y)) / size.y;

	vec4 color = texture(u_texture, vec3(uv, p_uv.z));		
	gl_FragColor = color;	
}