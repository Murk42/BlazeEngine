#version 330 core

in vec4 p_rect;
in vec4 p_textureCenterRect;
in vec3 p_uv;

uniform sampler2DArray u_texture;

void main()
{			
	vec2 texSize = textureSize(u_texture, 0).xy;
	vec2 uv;
	
	vec2 pos = p_rect.xy;

	float tx2 = p_textureCenterRect.z;
	float ty2 = p_textureCenterRect.w;		

	float x1 = p_textureCenterRect.x;
	float x3 = texSize.x - tx2 - x1;
	float y1 = p_textureCenterRect.y;
	float y3 = texSize.y - ty2 - y1;

	float rx2 = p_rect.z - x1 - x3;
	float ry2 = p_rect.w - y1 - y3;

	if (pos.x < x1)
		uv.x = pos.x / texSize.x;
	else if (pos.x - x1 < rx2)
		uv.x = ((pos.x - x1) / rx2 * tx2 + x1) / texSize.x;
	else
		uv.x = (pos.x - x1 - rx2) / texSize.x;

	if (pos.y < y1)
		uv.y = pos.y / texSize.y;
	else if (pos.y - y1 < ry2)
		uv.y = ((pos.y - y1) / ry2 * ty2 + y1) / texSize.y;
	else
		uv.y = (pos.y - y1 - ry2) / texSize.y;

	vec4 color = texture(u_texture, vec3(uv, p_uv.z));		
	gl_FragColor = color;	
}