#version 450															   
																		   
in vec4  frag_outerColor;
in vec4  frag_innerColor;
in float frag_outerRadius;
in float frag_innerRadius;
in vec2  frag_center;
in vec2  frag_pos;

float circle_distance(vec2 center, float radius, vec2 pos)
{
	return distance(pos, center) - radius;	
}

vec4 mixSteps(float d, float threshold1, float threshold2, vec4 color1, vec4 color2, vec4 color3)
{
	const float antialiasing = 1.5f;
				
	float s1 = (threshold1 > 0 ? smoothstep(threshold1 - antialiasing / 2, threshold1 + antialiasing / 2, d) : 1.0);
	float s2 = smoothstep(threshold2 - antialiasing / 2, threshold2 + antialiasing / 2, d);	

	float t1 = 1 - s1;
	float t2 = s1 - s2;
	float t3 = s2;

	return color1 * t1 + color2 * t2 + color3 * t3;
}
																		   
void main()															   
{							
	float d = distance(frag_center, frag_pos);	

	vec4 color = mixSteps(d, frag_innerRadius, frag_outerRadius, frag_innerColor, frag_outerColor, vec4(frag_outerColor.xyz, 0));
																		   	
	gl_FragColor = color;    
}																		   