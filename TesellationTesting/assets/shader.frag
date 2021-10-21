#version 400

in vec2 frag_uv;
in mat3 frag_TBN;

uniform sampler2D u_diffuse;
uniform sampler2D u_normal;
uniform vec3 u_lightDir;

void main()
{	
	vec3 normal;
	normal = texture(u_normal, frag_uv).rgb;
	normal = normal * 2 - 1;
	normal = normalize(frag_TBN * normal);	

	float value = dot(normal, -u_lightDir);

	vec4 diffuse = texture(u_diffuse, frag_uv);

	gl_FragColor = diffuse * vec4(value, value, value, 1) + vec4(0.2, 0.2, 0.2, 0);
	//gl_FragColor = vec4(u_lightDir, 1);\
	//gl_FragColor = vec4(normal , 1);
}