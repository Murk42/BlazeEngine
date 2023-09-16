#version 450    

layout(location = 0) in vec2 i_pos;
layout(location = 1) in vec2 i_uv;
layout(location = 2) in vec4 i_color;

layout(push_constant) uniform Variables { 
    mat4 proj;
	mat4 model;
} variables;

layout(location = 0) out vec2 frag_pos;
layout(location = 1) out vec2 frag_uv;
layout(location = 2) out vec4 frag_color;


void main()
{
	vec4 screenSpace = variables.model * vec4(i_pos.x, i_pos.y, 0, 1);
	gl_Position = variables.proj * screenSpace;
	frag_pos = screenSpace.xy;
	frag_uv = i_uv;
	frag_color = i_color;	
}
