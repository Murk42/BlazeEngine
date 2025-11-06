#version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

layout(location = 0) uniform mat4 u_projectionMatrix;
layout(location = 1) uniform mat4 u_viewMatrix;
layout(location = 3) uniform vec3 u_lightDir;
 
out float frag_dot;

void main() 
{
    mat4 VP = u_projectionMatrix * u_viewMatrix;
    vec3 ab = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 ac = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

    vec3 normal = normalize(cross(ab, ac));
    float lightDot = dot(normal, u_lightDir);

    gl_Position = VP * gl_in[0].gl_Position; 
    frag_dot = lightDot;
    EmitVertex();

    gl_Position = VP * gl_in[1].gl_Position;
    frag_dot = lightDot;
    EmitVertex();

    gl_Position = VP * gl_in[2].gl_Position;
    frag_dot = lightDot;
    EmitVertex();
    
    EndPrimitive();
}  