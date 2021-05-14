#version 330 core

in vec4 p_rect;
in vec3 p_uv;

uniform sampler2DArray u_texture;
uniform float u_texScale;
uniform vec4 u_textureCenterRect;

void main()
{			
	vec2 texSize = textureSize(u_texture, 0).xy;
	vec2 uv;	
	
	vec2 pos = p_rect.xy;
	 
	float tx1 = u_textureCenterRect.x;
	float ty1 = u_textureCenterRect.y;
	float tx2 = u_textureCenterRect.z;
	float ty2 = u_textureCenterRect.w;
	float tx3 = texSize.x - tx1 - tx2;
	float ty3 = texSize.y - ty1 - ty2;

	float sx1 = tx1 * u_texScale;
	float sy1 = ty1 * u_texScale;
	float sx2 = texSize.x - (tx1 + tx3) * u_texScale;
	float sy2 = texSize.y - (ty1 + ty3) * u_texScale;
	float sx3 = tx3 * u_texScale;
	float sy3 = ty3 * u_texScale;

	float rx1 = sx1;
	float ry1 = sy1;
	float rx2 = p_rect.z - sx1 - sx3;
	float ry2 = p_rect.w - sy1 - sy3;
	float rx3 = sx3;
	float ry3 = sy3;

	//if (sx1 == 13 && sx2 == 70 && sx3 == 13)
	//{
	//	gl_FragColor = vec4(0, 1, 0, 1);
	//	return;
	//}

	if (pos.x < rx1)	
		uv.x = pos.x / rx1 * tx1 / texSize.x;
	else if (pos.x < rx1 + rx2)
		uv.x = ((pos.x - rx1) / rx2 * tx2 + tx1) / texSize.x;
	else
		uv.x = ((pos.x - rx1 - rx2) / rx3 * tx3 + tx1 + tx2)/ texSize.x;

	if (pos.y < ry1)	
		uv.y = pos.y / ry1 * ty1 / texSize.y;
	else if (pos.y < ry1 + ry2)
		uv.y = ((pos.y - ry1) / ry2 * ty2 + ty1) / texSize.y;
	else
		uv.y = ((pos.y - ry1 - ry2) / ry3 * ty3 + ty1 + ty2)/ texSize.y;

	vec4 color = texture(u_texture, vec3(uv, p_uv.z));		
	gl_FragColor = color;	
}