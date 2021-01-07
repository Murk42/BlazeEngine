#include "Terrain.h"


inline uint Terrain::GetChunkVertexWidth(size_t lod)
{
	switch (lod)
	{
	case 0: return LOD<0>::chunkVertexWidth;
	case 1: return LOD<1>::chunkVertexWidth;
	case 2: return LOD<2>::chunkVertexWidth;		
	}	
}

inline uint Terrain::GetChunkVertexCount(size_t lod)
{
	switch (lod)
	{
	case 0: return LOD<0>::chunkVertexCount;
	case 1: return LOD<1>::chunkVertexCount;
	case 2: return LOD<2>::chunkVertexCount;
	}
}

inline uint Terrain::GetChunkTriangleCount(size_t lod)
{
	switch (lod)
	{
	case 0: return LOD<0>::chunkTriangleCount;
	case 1: return LOD<1>::chunkTriangleCount;
	case 2: return LOD<2>::chunkTriangleCount;
	}
}

Terrain::Terrain()
{
}

Terrain::~Terrain()
{
}

void Terrain::SetMaterial(Material* material, const std::initializer_list<Type>& vertexLayout)
{
	this->material = material;
	this->vertexLayout = vertexLayout;
}

void Terrain::Setup()
{
	{
		uint index[LOD<0>::chunkTriangleCount * 3];
		uint* it = index;
		uint offset = 0;
		for (int i = 0; i < LOD<0>::chunkTriangleCount / 2; ++i)
		{
			it[0] = offset;
			it[1] = offset + 1;
			it[2] = offset + 2;
			it[3] = offset + 2;
			it[4] = offset + 1;
			it[5] = offset + 3;
			offset += 4;
			it += 6;
		}

		ib.AllocateData(index, LOD<0>::chunkTriangleCount * 3, Static | Draw, Type::UnsignedInt);
	}

	{
		Chunk** it = chunks;
		Vec2f pos = Vec2f(-(float)((chunkWidth + 1) / 2 - 1) * chunkSize, -(float)((chunkWidth + 1) / 2 - 1) * chunkSize);
		for (int y = 0; y < chunkWidth; ++y, pos.y += chunkSize)
		{
			uint lodY = abs(y - (int)((chunkWidth + 1) / 2 - 1));
			for (uint i = 0; i < LODcount; ++i)
				if (lodY < LODchunkWidth[i])
				{
					lodY = i;
					break;
				}
				else
					lodY -= LODchunkWidth[i];

			for (int x = 0; x < chunkWidth; ++x, ++it, pos.x += chunkSize)
			{
				uint lodX = abs(y - (int)((chunkWidth + 1) / 2 - 1));
				for (uint i = 0; i < LODcount; ++i)
					if (lodX < LODchunkWidth[i])
					{
						lodX = i;
						break;
					}
					else
						lodX -= LODchunkWidth[i];

				uint lod = std::max(lodX, lodY);

				Chunk& chunk = *(Chunk*)(malloc(sizeof(Chunk) + LOD<0>::chunkTriangleCount * 3));
				new (&chunk)Chunk();
				*it = &chunk;

				chunk.lod = lod;

				size_t vertexSize = GetChunkVertexCount(lod) * sizeof(Vertex);
				chunk.vertex = malloc(vertexSize);

				uint vertexWidth = GetChunkVertexWidth(lod);
				float offset = (float)chunkSize / vertexWidth;
				Vertex* vIt = (Vertex*)chunk.vertex;
				for (int y = 0; y < vertexWidth; ++y, pos.y += offset)
					for (int x = 0; x < vertexWidth; ++x, pos.x += offset)
					{
						vIt->pos = Vec3f(pos.x, 0, pos.y);
						vIt->pos.y = Random::Simplex(Vec2f(pos.x, pos.y));
						++vIt;
					}

				chunk.vb.AllocateData(chunk.vertex, vertexSize, Static | Draw);
				chunk.vl.SetLayout(vertexLayout, &chunk.vb, &ib);				
			}
		}
	}
}

void Terrain::Cleanup()
{
	for (uint i = 0; i < chunkCount; ++i)
	{
		free(chunks[i]->vertex);
		chunks[i]->~Chunk();
		free(chunks[i]);
	}
}

void Terrain::Render(Entity cameraEntity)
{	
	Camera* cam = cameraEntity.GetComponent<Camera>();
	Transform* tran = cameraEntity.GetComponent<Transform3D>();
	Mat4f mvp = cam->projectionMatrix * tran->mat;

	for (uint i = 0; i < chunkCount; ++i)
	{
		PropertyID id = material->GetPropertyID("u_MVP");
		material->SetProperty(id, mvp);

		material->UpdateUniforms();
		material->BindTextures();
		material->GetShaderProgram().Bind();
		chunks[i]->vl.Bind();
		
		Renderer::DrawTriangles(GetChunkTriangleCount(chunks[i]->lod) * 3, 0);
	}
}