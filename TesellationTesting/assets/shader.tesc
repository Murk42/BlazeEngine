#version 440 core

in vec2 tesc_uv[];
in vec3 tesc_norm[];

out vec2 tese_uv[];
out vec3 tese_norm[];

layout (vertices = 4) out;

uniform float u_tessellation;

void main(void) 
{    
    if (gl_InvocationID == 0)
    {
        gl_TessLevelInner[0] = u_tessellation;
        gl_TessLevelInner[1] = u_tessellation;

        gl_TessLevelOuter[0] = u_tessellation;
        gl_TessLevelOuter[1] = u_tessellation;
        gl_TessLevelOuter[2] = u_tessellation;    
        gl_TessLevelOuter[3] = u_tessellation;    
    }

    tese_uv[gl_InvocationID] = tesc_uv[gl_InvocationID];
    tese_norm[gl_InvocationID] = tesc_norm[gl_InvocationID];
    // Everybody copies their input to their output
    gl_out[gl_InvocationID].gl_Position =
        gl_in[gl_InvocationID].gl_Position;
}