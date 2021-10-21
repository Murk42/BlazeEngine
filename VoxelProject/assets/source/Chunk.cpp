#include "Chunk.h" 
#include "World.h"

Vertex& Chunk::AllocateVertex()
{
	if (vertexCount == vertexReserved)
	{
		vertexReserved += 256;
		void* verticesNew = realloc(vertices, vertexReserved * sizeof(Vertex));
		if (verticesNew == nullptr)
			_ASSERT(false);
		vertices = (Vertex*)verticesNew;
	}	

	return vertices[vertexCount++];				
}

Chunk::Chunk()
	: 
	validGeneration(false), pos(0, 0, 0),
	validMesh(false), vertexCount(0), vertexReserved(0), vertices(nullptr)
{		
}
void Chunk::GenerateBlocks(Vec3i pos)
{		
	validGeneration = true;

	validMesh = false;
	vertexCount = 0;
	vertexReserved = 0;

	//TODO remove next line
	free(vertices);

	vertices = nullptr;

	this->pos = pos;

	Vec3i chunkPos = pos * chunkSize;
	int i = 0, j = 0;

	for (int z = 0; z < chunkSize; ++z)
		for (int x = 0; x < chunkSize; ++x)
		{
			heightMap[j] = Random::Simplex(Vec2f(chunkPos.x + x, chunkPos.z + z) / 100) * 20 - 8;
			++j;
		}

	int offset = 0;
	Vec3i blockPos = chunkPos;
	for (int z = 0; z < chunkSize; ++z, ++blockPos.z)
	{
		for (int y = 0; y < chunkSize; ++y, ++blockPos.y)
		{
			for (int x = 0; x < chunkSize; ++x, ++blockPos.x)
			{
				j = x + z * chunkSize;

				if (blockPos.y < heightMap[j])
				{
					blocks[i].id = BlockID::Full;
				}
				else				
					blocks[i].id = BlockID::Air;

				int value = blockPos.y * 5 + 128;
				value = std::clamp(value, 0, 256);
				blocks[i].color = ColorRGB(value, value, value);

				++i;
			}
			blockPos.x = chunkPos.x;
		}
		blockPos.y = chunkPos.y;
	}
}

void Chunk::GenerateMesh(Chunk* px, Chunk* nx, Chunk* py, Chunk* ny, Chunk* pz, Chunk* nz)
{		
	//Logger::AddLog(LogType::Message, "BlazeEngine", "Generating chunk mesh at position (" + String::Convert(pos.x) + ", " + String::Convert(pos.y) + ", " + String::Convert(pos.z) + ").");

	vertexCount = 0;
	validMesh = true;

	Vec3f blockPos = pos * chunkSize;

	//internal body
	{
		Vec2f uv, size;		
		uint i = 0;
		for (uint z = 0; z < (uint)chunkSize; ++z)
		{
			bool testPZ = (z < (uint)chunkSize - 1);
			bool textNZ = (z > 0);

			for (uint y = 0; y < (uint)chunkSize; ++y)
			{
				bool testPY = (y < (uint)chunkSize - 1);
				bool testNY = (y > 0);

				for (uint x = 0; x < (uint)chunkSize; ++x)
				{
					bool testPX = (x < (uint)chunkSize - 1);
					bool testNX = (x > 0);						

					Block block = blocks[i];

					if (block.id != BlockID::Air)
					{
						//px
						if (testPX && blocks[i + (1 << 0)].id == BlockID::Air)
						{
							Vertex& vertex = AllocateVertex();
							vertex.pos1 = blockPos + Vec3<int32>(x + 1, y + 1, z + 1);
							vertex.pos2 = blockPos + Vec3<int32>(x + 1, y, z + 1);
							vertex.pos3 = blockPos + Vec3<int32>(x + 1, y + 1, z);
							vertex.color = block.color;							
						}
						//nx
						if (testNX && blocks[i - (1 << 0)].id == BlockID::Air)
						{
							Vertex& vertex = AllocateVertex();
							vertex.pos1 = blockPos + Vec3<int32>(x, y, z);
							vertex.pos2 = blockPos + Vec3<int32>(x, y, z + 1);
							vertex.pos3 = blockPos + Vec3<int32>(x, y + 1, z);
							vertex.color = block.color;
						}

						//py
						if (testPY && blocks[i + (1 << 4)].id == BlockID::Air)
						{
							Vertex& vertex = AllocateVertex();
							vertex.pos1 = blockPos + Vec3<int32>(x + 1, y + 1, z + 1);
							vertex.pos2 = blockPos + Vec3<int32>(x + 1, y + 1, z);
							vertex.pos3 = blockPos + Vec3<int32>(x, y + 1, z + 1);
							vertex.color = block.color;
						}
						//ny
						if (testNY && blocks[i - (1 << 4)].id == BlockID::Air)
						{
							Vertex& vertex = AllocateVertex();
							vertex.pos1 = blockPos + Vec3<int32>(x, y, z);
							vertex.pos2 = blockPos + Vec3<int32>(x + 1, y, z);
							vertex.pos3 = blockPos + Vec3<int32>(x, y, z + 1);
							vertex.color = block.color;
						}
						//pz
						if (testPZ && blocks[i + (1 << 8)].id == BlockID::Air)
						{
							Vertex& vertex = AllocateVertex();
							vertex.pos1 = blockPos + Vec3<int32>(x + 1, y + 1, z + 1);
							vertex.pos2 = blockPos + Vec3<int32>(x, y + 1, z + 1);
							vertex.pos3 = blockPos + Vec3<int32>(x + 1, y, z + 1);
							vertex.color = block.color;
						}
						//nz
						if (textNZ && blocks[i - (1 << 8)].id == BlockID::Air)
						{
							Vertex& vertex = AllocateVertex();
							vertex.pos1 = blockPos + Vec3<int32>(x, y, z);
							vertex.pos2 = blockPos + Vec3<int32>(x, y + 1, z);
							vertex.pos3 = blockPos + Vec3<int32>(x + 1, y, z);
							vertex.color = block.color;
						}
					}

					++i;
				}
			}
		}
	}

	//sides
	{
		constexpr uint32 maskX = ~(15);
		constexpr uint32 maskY = ~(15 << 4);
		constexpr uint32 maskZ = ~(15 << 8);
		constexpr uint32 incrementX = 1;
		constexpr uint32 incrementY = 1 << 4;
		constexpr uint32 incrementZ = 1 << 8;
		uint32 iXL = 0, iXU = 0;
		uint32 iYL = 0, iYU = 0;
		uint32 iZL = 0, iZU = 0;
		Block block;
		Vec2f uv, size;
		for (uint32 i = 0; i < (uint)chunkSize; ++i)
		{
			for (uint32 j = 0; j < (uint)chunkSize; ++j)
			{
				iXU = iXL | ((chunkSize - 1) << 0);
				iYU = iYL | ((chunkSize - 1) << 4);
				iZU = iZL | ((chunkSize - 1) << 8);

				
				//px
				block = blocks[iXU];
				if (IsFullBlock(block) && !IsFullBlock(px->blocks[iXL]))
				{
					Vertex& vertex = AllocateVertex();
					vertex.pos1 = blockPos + Vec3<int32>(chunkSize, j + 1, i + 1);
					vertex.pos2 = blockPos + Vec3<int32>(chunkSize, j, i + 1);
					vertex.pos3 = blockPos + Vec3<int32>(chunkSize, j + 1, i);					
					vertex.color = block.color;
				}

				//nx
				block = blocks[iXL];
				if (IsFullBlock(block) && !IsFullBlock(nx->blocks[iXU]))
				{
					Vertex& vertex = AllocateVertex();
					vertex.pos1 = blockPos + Vec3<int32>(0, j, i);
					vertex.pos2 = blockPos + Vec3<int32>(0, j, i + 1);
					vertex.pos3 = blockPos + Vec3<int32>(0, j + 1, i);
					vertex.color = block.color;
				}

				//py
				block = blocks[iYU];
				if (IsFullBlock(block) && !IsFullBlock(py->blocks[iYL]))
				{
					Vertex& vertex = AllocateVertex();
					vertex.pos1 = blockPos + Vec3<int32>(j + 1, chunkSize, i + 1);
					vertex.pos2 = blockPos + Vec3<int32>(j + 1, chunkSize, i);
					vertex.pos3 = blockPos + Vec3<int32>(j, chunkSize, i + 1);
					vertex.color = block.color;
				}

				//ny
				block = blocks[iYL];
				if (IsFullBlock(block) && !IsFullBlock(ny->blocks[iYU]))
				{
					Vertex& vertex = AllocateVertex();
					vertex.pos1 = blockPos + Vec3<int32>(j, 0, i);
					vertex.pos2 = blockPos + Vec3<int32>(j + 1, 0, i);
					vertex.pos3 = blockPos + Vec3<int32>(j, 0, i + 1);
					vertex.color = block.color;
				}

				//pz
				block = blocks[iZU];
				if (IsFullBlock(block) && !IsFullBlock(pz->blocks[iZL]))
				{
					Vertex& vertex = AllocateVertex();
					vertex.pos1 = blockPos + Vec3<int32>(j + 1, i + 1, chunkSize);
					vertex.pos2 = blockPos + Vec3<int32>(j, i + 1, chunkSize);
					vertex.pos3 = blockPos + Vec3<int32>(j + 1, i, chunkSize);
					vertex.color = block.color;
				}

				//nz
				block = blocks[iZL];
				if (IsFullBlock(block) && !IsFullBlock(nz->blocks[iZU]))
				{
					Vertex& vertex = AllocateVertex();
					vertex.pos1 = blockPos + Vec3<int32>(j, i, 0);
					vertex.pos2 = blockPos + Vec3<int32>(j, i + 1, 0);
					vertex.pos3 = blockPos + Vec3<int32>(j + 1, i, 0);
					vertex.color = block.color;
				}

				iXL += incrementY;
				iYL += incrementX;
				iZL += incrementX;
			}

			iXL = (iXL - incrementY) & maskY;
			iYL = (iYL - incrementX) & maskX;
			iZL = (iZL - incrementX) & maskX;
			iXL += incrementZ;
			iYL += incrementZ;
			iZL += incrementY;
		}
	}

	if (vertexCount == 0)
	{
		free(vertices);
		vertices = nullptr;
		vertexReserved = 0;
	}	
}

void Chunk::Invalidate()
{
	validGeneration = false;
	validMesh = false;
	vertexCount = 0;

	//TODO remove next 3 lines
	vertexReserved = 0;
	free(vertices);
	vertices = nullptr;
}
