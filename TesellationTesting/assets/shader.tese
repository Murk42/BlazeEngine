#version 430

layout (quads, fractional_odd_spacing, ccw) in;

in vec2 tese_uv[];
in vec3 tese_norm[];

out vec2 geom_uv;
out vec3 geom_norm;
out vec4 geom_pos;

uniform sampler2D u_displacement;
uniform float u_displacementAmount;

vec4 CalculatePos()
{
	vec4 mix1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
    vec4 mix2 = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);

	return mix(mix1, mix2, gl_TessCoord.y);	
}

vec2 CalculateUV()
{
	vec2 uv1 = mix(tese_uv[0], tese_uv[1], gl_TessCoord.x);
    vec2 uv2 = mix(tese_uv[2], tese_uv[3], gl_TessCoord.x);

	return mix(uv1, uv2, gl_TessCoord.y);
}

vec3 CalculateNorm()
{
	vec3 norm1 = mix(tese_norm[0], tese_norm[1], gl_TessCoord.x);
    vec3 norm2 = mix(tese_norm[2], tese_norm[3], gl_TessCoord.x);

	return mix(norm1, norm2, gl_TessCoord.y);
}

void main(void) 
{
	
	vec4 pos = CalculatePos();
	vec2 uv = CalculateUV();
	vec3 norm = CalculateNorm();

	pos += vec4(0, texture(u_displacement, uv).r * u_displacementAmount, 0, 0);
	
	geom_pos = pos;
	geom_uv = uv;
	geom_norm = norm;
}