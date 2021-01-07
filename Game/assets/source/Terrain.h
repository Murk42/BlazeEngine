#pragma once
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

class Terrain
{	
	template<size_t>
	struct LOD;

	template<>
	struct LOD<0>
	{		
		static constexpr uint chunkVertexWidth = 20;
		static constexpr uint chunkVertexCount = chunkVertexWidth * chunkVertexWidth;
		static constexpr uint chunkTriangleCount = (chunkVertexWidth - 1) * (chunkVertexWidth - 1) * 2;		
	};
	template<>
	struct LOD<1>
	{		
		static constexpr uint chunkVertexWidth = 10;
		static constexpr uint chunkVertexCount = chunkVertexWidth * chunkVertexWidth;
		static constexpr uint chunkTriangleCount = (chunkVertexWidth - 1) * (chunkVertexWidth - 1) * 2;		
	};
	template<>
	struct LOD<2>
	{
		static constexpr uint chunkVertexWidth = 5;
		static constexpr uint chunkVertexCount = chunkVertexWidth * chunkVertexWidth;
		static constexpr uint chunkTriangleCount = (chunkVertexWidth - 1) * (chunkVertexWidth - 1) * 2;		
	};
	static inline uint GetChunkVertexWidth(size_t lod);
	static inline uint GetChunkVertexCount(size_t lod);
	static inline uint GetChunkTriangleCount(size_t lod);

	static constexpr uint chunkSize = 10;
	static constexpr uint LODchunkWidth[]{ 5, 5, 5 };
	static constexpr uint LODcount = sizeof(LODchunkWidth) / sizeof(*LODchunkWidth);	
	
	static constexpr uint chunkWidth = []()
	{
		uint sum = 0;
		for (uint i = 0; i < LODcount; ++i)
			sum += LODchunkWidth[i];
		return sum * 2 - 1;
	}();
	static constexpr uint chunkCount = chunkWidth * chunkWidth;

	inline static 

	struct Vertex
	{
		Vec3f pos;
	};

	IndexBuffer ib;
	struct Chunk
	{
		uint lod;
		VertexBuffer vb;
		VertexLayout vl;
		void* vertex;	
	};
	
	Chunk* chunks[chunkCount];

	Material* material;
	std::vector<Type> vertexLayout;
public:	
	Terrain();
	~Terrain();

	void SetMaterial(Material* material, const std::initializer_list<Type>& vertexLayout);	

	void Setup();
	void Update();
	void Cleanup();

	void Render(Entity cameraEntity);
};