#version 330 core

in vec2 frag_size;
in vec2 frag_pos;
in vec4 frag_color;
in vec4 frag_outlineColor;
in float frag_radius;
in float frag_thickness;

float distanceToAlpha(float dist)
{
	return smoothstep(0, 2, dist);
}

float getOutterDistance(float radius, float thickness)
{
	return length(max(abs(frag_pos - frag_size / 2) - frag_size / 2 + radius, 0));
} 

float getInnerDistance(float radius, float thickness)
{
	return length(max(abs(frag_pos - frag_size / 2) - frag_size / 2 + radius + thickness, 0));
} 

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main()
{			
	vec4 color;
	float v2 = getOutterDistance(frag_radius, frag_thickness);	
	v2 = distanceToAlpha(frag_radius - v2);
	float v1 = getInnerDistance((frag_radius - frag_thickness), frag_thickness);
	v1 = distanceToAlpha((frag_radius - frag_thickness) - v1);
	v1 = clamp(v1, 0, 1);
	v2 = clamp(v2, 0, 1);
	color = frag_outlineColor;
	color.a *= v2;
	color = color * (1.0f - v1) + frag_color * v1;		
	gl_FragColor = color;	
}