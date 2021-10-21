#pragma once
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;
using namespace Graphics;
using namespace Core;

#include "Block.h"

class Chunk;
struct Vertex;

class World
{
	class WorldMaterial : public Material
	{
	public:		
		Property<Vec3f> cameraPos = "u_cameraPos";
		Property<float> viewDistance = "u_viewDistance";
		Property<Vec4f> clearColor = "u_clearColor";
	};

private:						
	struct {		
		std::atomic<bool> exitFlag;	
		std::atomic<int> fence1;
		std::atomic<int> fence2;

		Vec3<size_t> generationBegin;
		Vec3<size_t> generationEnd;
		Vec3<size_t> meshBegin;
		Vec3<size_t> meshEnd;
	} loadingThreadData;
	size_t loadingThreadCount;
	std::thread* loadingThreadArray;

	Vec3i centerPos;	
	Vec3f cameraPos;
	Vec3<std::atomic<size_t>> indexOffset;
	
	size_t loadDistance;	
	size_t loadSide;
	size_t loadSideSqr;	
	size_t viewDistance;	

	size_t chunkCount;		
	Chunk* chunkArray;		
		
	static constexpr size_t maxVertexCount = 1024 * 1024 * 16;
	VertexArray va;	
	StaticBuffer<BufferStorage::Dynamic, BufferMapping::NoMapping, BufferMappingMode::Monochrome> vb;	
	
	inline Vec3<size_t> CalculateChunkIndexPos(Vec3<size_t> relativePos);
	inline size_t CalculateChunkIndexPosX(size_t x);
	inline size_t CalculateChunkIndexPosY(size_t y);
	inline size_t CalculateChunkIndexPosZ(size_t z);
	
	inline Vec3i CalculateChunkPos(Vec3<size_t> relativePos);
	inline int CalculateChunkPosX(size_t relativeX);
	inline int CalculateChunkPosY(size_t relativeY);
	inline int CalculateChunkPosZ(size_t relativeZ);

	inline size_t CalculateChunkIndex(Vec3<size_t> indexPos);	
	inline void GenerateChunkMesh(Chunk& chunk, Vec3<size_t> relativePos, Vec3<size_t> indexPos);	

	void GenerateAllChunks();
	void RecaluclateIndexOffset();
	void MoveCenterX(int& x);
	void MoveCenterY(int& y);
	void MoveCenterZ(int& z);	

	void WakeUpLoadingThreads();
	void WaitForLoadingThreads();
	static void LoadingThreadFunction(World* world, size_t threadIndex);

	void DebugMainThreadChunkAcces(Vec3<size_t> relativePos, Vec3<size_t> indexPos);
	void DebugLoadingThreadChunkAcces(Vec3<size_t> relativePos, Vec3<size_t> indexPos);
public:
	WorldMaterial material;	

	World();
	~World();

	void SetViewDistance(size_t distance);	

	//pos - world cordinates
	void UpdateChunks(Vec3f pos);		

	void Render(const Mat4f& VP3D);
};