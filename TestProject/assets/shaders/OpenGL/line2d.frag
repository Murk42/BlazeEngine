#version 450															   
																		   
in vec4 frag_color;													   
in vec2 frag_pos;														   
in vec2 frag_pos1;														   
in vec2 frag_pos2;														   
in float frag_width;	

float line_distance(vec2 pointA, vec2 direction, vec2 point)
{	
	float a = -direction.y;
	float b = direction.x;
	float c = -(a * pointA.x + b * pointA.y);

	return a * point.x + b * point.y + c;
}
																		   
float segment_distance(vec2 pointA, vec2 pointB, vec2 point)
{																		   
	vec2 direction = pointB - pointA;
    float d = length(direction);
	vec2 offset = point - pointA;
	float t = dot(offset, direction) / (d * d);

	if (t < 0.0)
		return length(offset) * 1.25;
	else if (t > 1.0)
		return length(point - pointB) * 1.25;
	else
		return abs(line_distance(pointA, direction / d, point));
}												
																		   
void main()															   
{						
	float width = frag_width;
	float d = segment_distance(frag_pos1, frag_pos2, frag_pos);						   
	d = smoothstep(width - 1, width + 1, d);		
			
	gl_FragColor = vec4(vec3(frag_color.rgb), mix(frag_color.a, 0, d));    
}																		   