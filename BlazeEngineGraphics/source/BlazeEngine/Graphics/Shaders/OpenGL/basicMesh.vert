#version 450 core
 
layout(location = 0) in vec3 i_pos;
layout(location = 1) in uint i_meshDataIndex;

struct MeshData
{
	mat4 transformMatrix;
	vec4 color;
};
layout(std140, binding = 0) uniform MeshDataUB
{
	MeshData meshData[128];
};

out vec4 geom_color;
 
void main() 
{
	gl_Position = meshData[i_meshDataIndex].transformMatrix * vec4(i_pos, 1);
	geom_color = meshData[i_meshDataIndex].color;
}
