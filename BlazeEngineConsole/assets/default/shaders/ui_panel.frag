#version 450

in vec2 frag_pos;
in vec4 frag_rect;
in vec4 frag_fillColor;
in vec4 frag_borderColor;
in float frag_radius;
in float frag_borderWidth;

vec4 StepLerp(float x, float lower, float upper, vec4 a, vec4 b)
{	
	return mix(a, b, min(max(x - lower,0) / (upper - lower), 1));
}
float StepLerp(float x, float lower, float upper, float a, float b)
{	
	return mix(a, b, min(max(x - lower,0) / (upper - lower), 1));
}

float DistanceFromSquare(vec2 pos, vec2 size)
{
	if (pos.x >= 0 && pos.x < size.x && pos.y >= 0 && pos.y < size.y)	
	{
		float dx = abs(pos.x - size.x / 2) - size.x / 2;
		float dy = abs(pos.y - size.y / 2) - size.y / 2;

		return max(dx, dy);
	}
	else
	{
		float dx = max(max(-pos.x, pos.x - size.x), 0);
		float dy = max(max(-pos.y, pos.y - size.y), 0);

		float value = sqrt(dx*dx + dy*dy);	
		
		return value;
	}
}

void main()
{
	//Distance is 0 on the border, outside its positive inside the rectangle its -frag_radius
	float dist = DistanceFromSquare(frag_pos - frag_rect.xy - frag_radius, frag_rect.zw - frag_radius * 2) - frag_radius;			

	float borderWidth = frag_borderWidth;

	if (borderWidth > 0 && borderWidth < 2)
		borderWidth = 2;
		
	vec4 color;

	float outterAntialiasing = 1.0f;
	float innerAntialiasing = 2.0f;
	
	if (borderWidth == 0)
		color = StepLerp(dist, -outterAntialiasing / 2, outterAntialiasing / 2, frag_fillColor, vec4(frag_fillColor.rgb, 0));				
	else
		if (dist > -frag_borderWidth / 2)		
			color = StepLerp(dist, -outterAntialiasing / 2, outterAntialiasing / 2, frag_borderColor, vec4(frag_borderColor.rgb, 0));				
		else	
			color = StepLerp(dist, -borderWidth - innerAntialiasing, -borderWidth, frag_fillColor, frag_borderColor);				
			
	gl_FragColor = color;
}