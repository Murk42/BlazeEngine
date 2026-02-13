#version 450															   
																		   
in vec4 frag_color;													   
in vec2 frag_pos1;														   
in vec2 frag_pos2;														   
in float frag_width;	
 
out vec4 out_color;

layout(location = 2) uniform vec2 u_targetSize;

//float line_distance(vec2 direction, vec2 point)
//{	
//	float a = -direction.y;
//	float b = direction.x;
//	float c = -(a * pointA.x + b * pointA.y);
//
//	return a * point.x + b * point.y + c;
//}
																		   
float segment_distance(vec2 pointA, vec2 pointB, vec2 point)
{											
	point -= pointA;
	pointB -= pointA;

	float dist = length(pointB);
	vec2 dir = pointB / dist;
	vec2 norm = vec2(-dir.y, dir.x);

	vec2 lineCoords = mat2(dir.x, -dir.y, dir.y, dir.x) * point;

	if (lineCoords.x < 0)	
		return length(lineCoords);	
	if (lineCoords.x > dist)
		return length(vec2(lineCoords.x - 1, lineCoords.y));
	return abs(lineCoords.y);	
}												
																		   
void main()															   
{							
	float d = segment_distance(frag_pos1, frag_pos2, gl_FragCoord.xy);						   	

	d = smoothstep(frag_width - 1, frag_width + 1, d);		

	
	//if (d == 1)
	//	discard;	
		out_color = vec4(1, 1, 1, 1);
	//else
		//out_color = vec4(gl_FragCoord.xy / u_targetSize, 0, 1);

	//out_color = vec4(frag_color.rgb, mix(frag_color.a, 0, d));    
}																		   