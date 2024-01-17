#version 450

in vec4 frag_fillColor;
in vec4 frag_borderColor;
in vec2 frag_pos;
in vec2 frag_p1;
in vec2 frag_p2;
in vec2 frag_p3;
in float frag_borderWidth;
in float frag_cornerRadius;

out vec4 out_color;

vec4 StepLerp(float x, float lower, float upper, vec4 a, vec4 b)
{	
	return mix(a, b, min(max(x - lower,0) / (upper - lower), 1));
}
float StepLerp(float x, float lower, float upper, float a, float b)
{	
	return mix(a, b, min(max(x - lower,0) / (upper - lower), 1));
}

float DistanceFromLine(vec2 pos, vec2 p, vec2 v)
{
	vec2 n = vec2(-v.y, v.x);
	return -dot(p - pos, n);		
}
float DistanceFromCurvedRectangle(vec2 pos, vec2 v1, vec2 v2, float radius)
{
	float w = length(v1);
	float h = length(v2);

	vec2 v1n = v1 / w;
	vec2 v2n = v2 / h;

	pos -= (v1n + v2n) * radius;

	v1 *= 1.0 - radius * 2.0 / w;
	v2 *= 1.0 - radius * 2.0 / h;
	w -= radius * 2.0;
	h -= radius * 2.0;

	float l = DistanceFromLine(pos, vec2(0, 0), v2n);	
	float r = DistanceFromLine(pos, v1, -v2n);
	float d = DistanceFromLine(pos, vec2(0, 0), -v1n);
	float u = DistanceFromLine(pos, v2, v1n);
        
    float dist = length(vec4(max(l, 0.0), max(r, 0.0), max(u, 0.0), max(d, 0.0))) - radius;
        
    if (dist == -radius)
        return max(max(l, r), max(d, u)) - radius;
    return dist;    
}

void main()
{
	//Distance is 0 on the border, outside its positive inside the rectangle its -frag_radius
	float dist = DistanceFromCurvedRectangle(frag_pos - frag_p1, frag_p2 - frag_p1, frag_p3 - frag_p1, frag_cornerRadius);	

	float borderWidth = frag_borderWidth;
	
	if (borderWidth > 0 && borderWidth < 1)
		borderWidth = 1;
		
	vec4 color;
	
	float outterAntialiasing = 1.0f;
	float innerAntialiasing = 1.5f;
	
	if (borderWidth == 0)
		color = StepLerp(dist, -outterAntialiasing / 2, outterAntialiasing / 2, frag_fillColor, vec4(frag_fillColor.rgb, 0));				
	else
		if (dist > -frag_borderWidth / 2)		
			color = StepLerp(dist, -outterAntialiasing / 2, outterAntialiasing / 2, frag_borderColor, vec4(frag_borderColor.rgb, 0));				
		else	
			color = StepLerp(dist, -borderWidth - innerAntialiasing, -borderWidth, frag_fillColor, frag_borderColor);										

	out_color = color;
}