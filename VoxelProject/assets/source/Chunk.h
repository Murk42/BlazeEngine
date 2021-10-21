#pragma once
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;
using namespace Graphics::Core;

#include "Block.h"

struct Vertex
{
	Vec3<int32> pos1;
	Vec3<int32> pos2;
	Vec3<int32> pos3;
	ColorRGB color;
};

class World;

class Chunk
{
	Vertex& AllocateVertex();
public:
	static constexpr size_t chunkSize = 16;	

	std::mutex generationMutex;	
	bool validGeneration;
	Vec3i pos;
	Block blocks[chunkSize * chunkSize * chunkSize];
	int heightMap[chunkSize * chunkSize];	

	std::mutex meshMutex;
	bool validMesh;
	size_t vertexCount;
	size_t vertexReserved;
	Vertex* vertices;


	Chunk();

	void GenerateBlocks(Vec3i pos);
	void GenerateMesh(Chunk* px, Chunk* nx, Chunk* py, Chunk* ny, Chunk* pz, Chunk* nz);	

	void Invalidate();
};