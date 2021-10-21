#version 330 core

in vec2 p_uv;

uniform sampler2D u_texture;
uniform vec4 u_color;

const float smoothing = 0 / 64.0f;
		
float map(float value, float min1, float max1, float min2, float max2) {
	return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

const float u_buffer = 0.9;
const float u_gamma = 0.1;

void main()
{
	float dist = texture2D(u_texture, p_uv).r;
	float alpha = dist;
	gl_FragColor = vec4(u_color.rgb, alpha * u_color.a);
}