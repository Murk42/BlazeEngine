#version 330 core

in vec2 p_uv;

uniform sampler2D u_texture;
uniform float limit;

float map(float value, float min1, float max1, float min2, float max2) 
{
	return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main()
{	
	float limitSqr = limit * limit;
	vec2 texSize = textureSize(u_texture, 0);
	float pixel = texture(u_texture, p_uv).r;

	if (pixel == 1.0f)
	{
		gl_FragColor = vec4(1, 0, 0, 1);
		return;
	}
	
	float value = 1.0f;
	for (float y = -limit; y <= limit; y += 1.0f)
		for (float x = -limit; x <= limit; x += 1.0f)
		{
			float dist = x * x + y * y;
			if (dist <= limitSqr)
			{				
				float pixel2 = texture(u_texture, p_uv + vec2(x, y) / texSize).r;
	
				if (pixel2 == 1.0f)				
					value = min(value,  dist / limitSqr);				
			}			
		}
	
		
	//value = value / sum ;
	vec4 color = vec4(1.0f - sqrt(value), 0, 0, 1);
	gl_FragColor = color;
}