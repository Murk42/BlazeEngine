#version 450    

layout(location = 0) in vec3 pos;

layout(push_constant) uniform Variables { 
    mat4 proj;
} variables;

void main() 
{
    gl_Position = variables.proj * vec4(pos, 1);
}