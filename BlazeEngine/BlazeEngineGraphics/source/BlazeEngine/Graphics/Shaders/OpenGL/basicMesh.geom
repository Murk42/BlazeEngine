#version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

layout(location = 0) uniform mat4 u_projectionMatrix;
layout(location = 1) uniform mat4 u_viewMatrix;
layout(location = 2) uniform vec3 u_lightDir;
 
in vec4 geom_color[];

out float frag_dot;
out vec4 frag_color;

void main() 
{
    mat4 VP = u_projectionMatrix * u_viewMatrix;
    vec3 ab = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 ac = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

    vec3 normal = normalize(cross(ab, ac));
    float lightDot = dot(normal, u_lightDir);

    gl_Position = VP * gl_in[0].gl_Position; 
    frag_color = geom_color[0];
    frag_dot = lightDot;
    EmitVertex();

    gl_Position = VP * gl_in[1].gl_Position;
    frag_color = geom_color[1];
    frag_dot = lightDot;
    EmitVertex();

    gl_Position = VP * gl_in[2].gl_Position;
    frag_color = geom_color[2];
    frag_dot = lightDot;
    EmitVertex();
    
    EndPrimitive();
}  