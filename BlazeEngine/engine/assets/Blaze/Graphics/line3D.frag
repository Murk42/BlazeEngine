#version 450

in vec4 frag_color;
in vec2 frag_pos;
in vec2 frag_pos1;
in vec2 frag_pos2;
in float frag_width;

float dist(vec2 p1, vec2 p2, vec2 p)
{
	const vec2 dir = p2 - p1;
	const float l2 = dir.x * dir.x + dir.y * dir.y;

	if (l2 == 0.0) return distance(p, p1);   
	
	const float t = max(0, min(1, dot(p - p1, dir) / l2));
	const vec2 projection = p1 + t * dir;
	return distance(p, projection);
}

void main()
{
	//float d = dist(frag_pos1, frag_pos2, frag_pos);
	//d = (d - frag_width + 2) / 2; 	
	//d = clamp(d, 0, 1);
	//
	//gl_FragColor = vec4(frag_color.rgb, mix(frag_color.a, 0, d));
	gl_FragColor = frag_color;
}