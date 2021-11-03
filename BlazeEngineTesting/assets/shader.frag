#version 450

in vec3 frag_normal;

void main()
{	
	float light = dot(frag_normal, normalize(-vec3(1, -1, 1)));
	vec3 color = vec3(1, 0.125, 0);	
	vec3 ambient = vec3(0.1, 0.0125, 0);
	gl_FragColor = vec4(color * light + ambient, 1);
	//gl_FragColor = vec4(frag_normal / 2 + 0.5, 1);
}