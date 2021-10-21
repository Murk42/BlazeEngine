#include "World.h"
#include "Chunk.h"
#include <algorithm>

#define WORLD_DEBUG

inline Vec3<size_t> World::CalculateChunkIndexPos(Vec3<size_t> relPos)
{
	return Vec3i(
		CalculateChunkIndexPosX(relPos.x),
		CalculateChunkIndexPosY(relPos.y),
		CalculateChunkIndexPosZ(relPos.z)
		);
}
inline size_t World::CalculateChunkIndexPosX(size_t x)
{
#ifdef WORLD_DEBUG
	_ASSERT(x < loadSide);
#endif
	return (x + indexOffset.x) % loadSide;
}
inline size_t World::CalculateChunkIndexPosY(size_t y)
{
#ifdef WORLD_DEBUG
	_ASSERT(y < loadSide);
#endif
	return (y + indexOffset.y) % loadSide;
}
inline size_t World::CalculateChunkIndexPosZ(size_t z)
{
#ifdef WORLD_DEBUG
	_ASSERT(z < loadSide);
#endif
	return (z + indexOffset.z) % loadSide;
}

inline Vec3i World::CalculateChunkPos(Vec3<size_t> relPos)
{
	return Vec3i(
		CalculateChunkIndexPosX(relPos.x),
		CalculateChunkIndexPosY(relPos.y), 
		CalculateChunkIndexPosZ(relPos.z));
}
inline int World::CalculateChunkPosX(size_t relativeX)
{
	return centerPos.x + (int)relativeX;
}
inline int World::CalculateChunkPosY(size_t relativeY)
{
	return centerPos.y + (int)relativeY;
}
inline int World::CalculateChunkPosZ(size_t relativeZ)
{
	return centerPos.z + (int)relativeZ;
}

inline size_t World::CalculateChunkIndex(Vec3<size_t> indexPos)
{
#ifdef WORLD_DEBUG
	_ASSERT(indexPos.x < loadSide);
	_ASSERT(indexPos.y < loadSide);
	_ASSERT(indexPos.z < loadSide);
#endif
	return indexPos.x + indexPos.y * loadSide + indexPos.z * loadSideSqr;
}
inline void World::GenerateChunkMesh(Chunk& chunk, Vec3<size_t> relativePos, Vec3<size_t> indexPos)
{
#ifdef WORLD_DEBUG
	_ASSERT(indexPos.x < loadSide);
	_ASSERT(indexPos.y < loadSide);
	_ASSERT(indexPos.z < loadSide);
	_ASSERT(relativePos.x < loadSide);
	_ASSERT(relativePos.y < loadSide);
	_ASSERT(relativePos.z < loadSide);
#endif

	size_t x = indexPos.x;
	size_t y = indexPos.y * loadSide;
	size_t z = indexPos.z * loadSideSqr;
	
	size_t nx = CalculateChunkIndexPosX(relativePos.x - 1) + y + z;
	size_t px = CalculateChunkIndexPosX(relativePos.x + 1) + y + z;
	size_t ny = x + CalculateChunkIndexPosY(relativePos.y - 1) * loadSide + z;
	size_t py = x + CalculateChunkIndexPosY(relativePos.y + 1) * loadSide + z;
	size_t nz = x + y + CalculateChunkIndexPosZ(relativePos.z - 1) * loadSideSqr;
	size_t pz = x + y + CalculateChunkIndexPosZ(relativePos.z + 1) * loadSideSqr;
	chunk.GenerateMesh(chunkArray + px, chunkArray + nx, chunkArray + py, chunkArray + ny, chunkArray + pz, chunkArray + nz);
}

void World::RecaluclateIndexOffset()
{
	if (centerPos.x < 0)
		indexOffset.x = loadSide - (-centerPos.x) % loadSide;
	else 
		indexOffset.x = centerPos.x % loadSide;

	if (centerPos.y < 0)
		indexOffset.y = loadSide - (-centerPos.y) % loadSide;
	else
		indexOffset.y = centerPos.y % loadSide;

	if (centerPos.z < 0)
		indexOffset.z = loadSide - (-centerPos.z) % loadSide;
	else
		indexOffset.z = centerPos.z % loadSide;
}

/*
void World::MoveCenterX(int& x)
{	
	if (x > 0)
	{
		WaitForLoadingThreads();		
		Console::Write("Moved X+: \n");
						
		size_t blockGenerationOffset = 0;
		size_t meshGenerationOffset = 1;
		for (size_t i = 0; i < loadingThreadCount; ++i)
		{
			loadingThreadArray[i].blockGenerationBegin = Vec3<size_t>(loadSide - 1, 0, blockGenerationOffset);
			blockGenerationOffset += loadSide / loadingThreadCount;
			loadingThreadArray[i].blockGenerationEnd = Vec3<size_t>(loadSide, loadSide, blockGenerationOffset);

			loadingThreadArray[i].meshGenerationBegin = Vec3<size_t>(loadSide - 2, 1, meshGenerationOffset);
			meshGenerationOffset += (loadSide - 2) / loadingThreadCount;
			loadingThreadArray[i].meshGenerationEnd = Vec3<size_t>(loadSide - 1, loadSide - 1, meshGenerationOffset);
		}

		loadingThreadArray[loadingThreadCount - 1].blockGenerationEnd.z = loadSide;
		loadingThreadArray[loadingThreadCount - 1].meshGenerationEnd.z = loadSide - 1;

		ConstructLoadingThreads();		
		--x;		
	}
	else if (x < 0)
	{
		WaitForLoadingThreads();
		Console::Write("Moved X-: \n");

		size_t blockGenerationOffset = 0;
		size_t meshGenerationOffset = 1;
		for (size_t i = 0; i < loadingThreadCount; ++i)
		{
			loadingThreadArray[i].blockGenerationBegin = Vec3<size_t>(0, 0, blockGenerationOffset);
			blockGenerationOffset += loadSide / loadingThreadCount;
			loadingThreadArray[i].blockGenerationEnd = Vec3<size_t>(1, loadSide, blockGenerationOffset);

			loadingThreadArray[i].meshGenerationBegin = Vec3<size_t>(1, 1, meshGenerationOffset);
			meshGenerationOffset += (loadSide - 2) / loadingThreadCount;
			loadingThreadArray[i].meshGenerationEnd = Vec3<size_t>(2, loadSide - 1, meshGenerationOffset);
		}

		loadingThreadArray[loadingThreadCount - 1].blockGenerationEnd.z = loadSide;
		loadingThreadArray[loadingThreadCount - 1].meshGenerationEnd.z = loadSide - 1;

		ConstructLoadingThreads();
		++x;
	}
}
void World::MoveCenterY(int& y)
{	
	if (y > 0)
	{
		WaitForLoadingThreads();
		Console::Write("Moved Y+: \n");

		size_t blockGenerationOffset = 0;
		size_t meshGenerationOffset = 1;
		for (size_t i = 0; i < loadingThreadCount; ++i)
		{
			loadingThreadArray[i].blockGenerationBegin = Vec3<size_t>(0, loadSide - 1, blockGenerationOffset);
			blockGenerationOffset += loadSide / loadingThreadCount;
			loadingThreadArray[i].blockGenerationEnd = Vec3<size_t>(loadSide, loadSide, blockGenerationOffset);

			loadingThreadArray[i].meshGenerationBegin = Vec3<size_t>(1, loadSide - 2, meshGenerationOffset);
			meshGenerationOffset += (loadSide - 2) / loadingThreadCount;
			loadingThreadArray[i].meshGenerationEnd = Vec3<size_t>(loadSide - 1, loadSide - 1, meshGenerationOffset);
		}

		loadingThreadArray[loadingThreadCount - 1].blockGenerationEnd.z = loadSide;
		loadingThreadArray[loadingThreadCount - 1].meshGenerationEnd.z = loadSide - 1;

		ConstructLoadingThreads();
		--y;
	}
	else if (y < 0)
	{
		WaitForLoadingThreads();
		Console::Write("Moved Y-: \n");

		size_t blockGenerationOffset = 0;
		size_t meshGenerationOffset = 1;
		for (size_t i = 0; i < loadingThreadCount; ++i)
		{
			loadingThreadArray[i].blockGenerationBegin = Vec3<size_t>(0, 0, blockGenerationOffset);
			blockGenerationOffset += loadSide / loadingThreadCount;
			loadingThreadArray[i].blockGenerationEnd = Vec3<size_t>(loadSide, 1, blockGenerationOffset);

			loadingThreadArray[i].meshGenerationBegin = Vec3<size_t>(1, 1, meshGenerationOffset);
			meshGenerationOffset += (loadSide - 2) / loadingThreadCount;
			loadingThreadArray[i].meshGenerationEnd = Vec3<size_t>(loadSide - 1, 2, meshGenerationOffset);
		}

		loadingThreadArray[loadingThreadCount - 1].blockGenerationEnd.z = loadSide;
		loadingThreadArray[loadingThreadCount - 1].meshGenerationEnd.z = loadSide - 1;

		ConstructLoadingThreads();
		++y;
	}
}
void World::MoveCenterZ(int& z)
{
	if (z > 0)
	{
		WaitForLoadingThreads();
		Console::Write("Moved Z+: \n");

		size_t blockGenerationOffset = 0;
		size_t meshGenerationOffset = 1;
		for (size_t i = 0; i < loadingThreadCount; ++i)
		{
			loadingThreadArray[i].blockGenerationBegin = Vec3<size_t>(0, blockGenerationOffset, loadSide - 1);
			blockGenerationOffset += loadSide / loadingThreadCount;
			loadingThreadArray[i].blockGenerationEnd = Vec3<size_t>(loadSide, blockGenerationOffset, loadSide);

			loadingThreadArray[i].meshGenerationBegin = Vec3<size_t>(1, meshGenerationOffset, loadSide - 2);
			meshGenerationOffset += (loadSide - 2) / loadingThreadCount;
			loadingThreadArray[i].meshGenerationEnd = Vec3<size_t>(loadSide - 1, meshGenerationOffset, loadSide - 1);
		}

		loadingThreadArray[loadingThreadCount - 1].blockGenerationEnd.y = loadSide;
		loadingThreadArray[loadingThreadCount - 1].meshGenerationEnd.y = loadSide - 1;

		ConstructLoadingThreads();
		--z;
	}
	else if (z < 0)
	{
		WaitForLoadingThreads();
		Console::Write("Moved Z-: \n");

		size_t blockGenerationOffset = 0;
		size_t meshGenerationOffset = 1;
		for (size_t i = 0; i < loadingThreadCount; ++i)
		{
			loadingThreadArray[i].blockGenerationBegin = Vec3<size_t>(0, blockGenerationOffset, 0);
			blockGenerationOffset += loadSide / loadingThreadCount;
			loadingThreadArray[i].blockGenerationEnd = Vec3<size_t>(loadSide, blockGenerationOffset, 1);

			loadingThreadArray[i].meshGenerationBegin = Vec3<size_t>(1, meshGenerationOffset, 1);
			meshGenerationOffset += (loadSide - 2) / loadingThreadCount;
			loadingThreadArray[i].meshGenerationEnd = Vec3<size_t>(loadSide - 1, meshGenerationOffset, 2);
		}

		loadingThreadArray[loadingThreadCount - 1].blockGenerationEnd.y = loadSide;
		loadingThreadArray[loadingThreadCount - 1].meshGenerationEnd.y = loadSide - 1;

		ConstructLoadingThreads();
		++z;
	}
}
*/


void World::WakeUpLoadingThreads()
{
	loadingThreadData.fence1 = 0;
	while (loadingThreadData.fence2 != 0);
	loadingThreadData.fence2 = loadingThreadCount;
}
void World::WaitForLoadingThreads()
{
	while (loadingThreadData.fence1 != loadingThreadCount)
		std::this_thread::sleep_for(std::chrono::microseconds(10));
}

void World::LoadingThreadFunction(World* world, size_t threadIndex)
{	
	auto& data = world->loadingThreadData;
	while (true)
	{
		++data.fence1;
		while (data.fence1 != 0)
			std::this_thread::sleep_for(std::chrono::microseconds(10));
		--data.fence2;
		while (data.fence1 != 0)
			std::this_thread::sleep_for(std::chrono::microseconds(10));

		if (data.exitFlag)
			break;

		const Vec3<size_t> generationBegin = data.generationBegin;
		const Vec3<size_t> generationEnd = data.generationEnd;
		const Vec3<size_t> generationSize = generationEnd - generationBegin;		
		const Vec3<size_t> meshBegin = data.meshBegin;
		const Vec3<size_t> meshEnd = data.meshEnd;
		const Vec3<size_t> meshSize = meshEnd - meshBegin;


	}
	/*
	const size_t threadCount = world->loadingThreadCount;
	const size_t chunkCount = world->chunkCount;	
	const size_t loadDistance = world->loadDistance;
	const size_t loadSide = world->loadSide;	
	const size_t loadSideSqr = world->loadSideSqr;
	const size_t viewDistance = world->viewDistance;
	
	size_t blockStart = loadSide * threadIndex / threadCount;
	size_t blockEnd = loadSide * (threadIndex + 1) / threadCount;
	size_t meshStart = (viewDistance * 2) * threadIndex / threadCount + 1;
	size_t meshEnd = (viewDistance * 2) * (threadIndex + 1) / threadCount + 1;

	Vec3<size_t> relativePos;
	Vec3<size_t> indexPos;
	Vec3i chunkPos;
	size_t index;	

	while (!world->loadingThreadExitFlag)
	{
		for (relativePos.z = blockStart; relativePos.z < blockEnd; ++relativePos.z)
		{
			indexPos.z = world->CalculateChunkIndexPosZ(relativePos.z);
			chunkPos.z = world->CalculateChunkPosZ(relativePos.z);

			for (relativePos.y = 0; relativePos.y < loadSide; ++relativePos.y)
			{
				indexPos.y = world->CalculateChunkIndexPosY(relativePos.y);
				chunkPos.y = world->CalculateChunkPosY(relativePos.y);

				for (relativePos.x = 0; relativePos.x < loadSide; ++relativePos.x)
				{
					indexPos.x = world->CalculateChunkIndexPosX(relativePos.x);
					chunkPos.x = world->CalculateChunkPosX(relativePos.x);
					index = world->CalculateChunkIndex(indexPos);

			 		Chunk& chunk = world->chunkArray[index];
					if (chunk.generationMutex.try_lock())
					{
						if (!chunk.validGeneration || chunk.pos != chunkPos)
							chunk.GenerateBlocks(chunkPos);

						chunk.generationMutex.unlock();					
					}
				}
			}
		}

		for (relativePos.z = meshStart; relativePos.z < meshEnd; ++relativePos.z)
		{
			indexPos.z = world->CalculateChunkIndexPosZ(relativePos.z);	
			chunkPos.z = world->CalculateChunkPosZ(relativePos.z);

			for (relativePos.y = loadDistance - viewDistance; relativePos.y < loadDistance + viewDistance; ++relativePos.y)
			{
				indexPos.y = world->CalculateChunkIndexPosY(relativePos.y);				
				chunkPos.y = world->CalculateChunkPosY(relativePos.y);

				for (relativePos.x = loadDistance - viewDistance; relativePos.x < loadDistance + viewDistance; ++relativePos.x)
				{
					indexPos.x = world->CalculateChunkIndexPosX(relativePos.x);
					chunkPos.x = world->CalculateChunkPosX(relativePos.x);
					index = world->CalculateChunkIndex(indexPos);

#ifdef WORLD_DEBUG
					_ASSERT(relativePos.x >= loadDistance - viewDistance && relativePos.x < loadDistance + viewDistance);
					_ASSERT(relativePos.y >= loadDistance - viewDistance && relativePos.y < loadDistance + viewDistance);
					_ASSERT(relativePos.z >= loadDistance - viewDistance && relativePos.z < loadDistance + viewDistance);
#endif
					size_t x = indexPos.x;
					size_t y = indexPos.y * loadSide;
					size_t z = indexPos.z * loadSideSqr;

					size_t nx = world->CalculateChunkIndexPosX(relativePos.x - 1) + y + z;
					size_t px = world->CalculateChunkIndexPosX(relativePos.x + 1) + y + z;
					size_t ny = x + world->CalculateChunkIndexPosY(relativePos.y - 1) * loadSide + z;
					size_t py = x + world->CalculateChunkIndexPosY(relativePos.y + 1) * loadSide + z;
					size_t nz = x + y + world->CalculateChunkIndexPosZ(relativePos.z - 1) * loadSideSqr;
					size_t pz = x + y + world->CalculateChunkIndexPosZ(relativePos.z + 1) * loadSideSqr;

					Chunk& chunk = world->chunkArray[index];
					Chunk* ca = world->chunkArray;

					std::lock_guard<std::mutex> lk(chunk.meshMutex);

					bool shouldGenerate;
					{
						std::lock_guard<std::mutex> lk1(chunk.generationMutex);
						std::lock_guard<std::mutex> lk2((ca + nx)->generationMutex);
						std::lock_guard<std::mutex> lk3((ca + px)->generationMutex);
						std::lock_guard<std::mutex> lk4((ca + ny)->generationMutex);
						std::lock_guard<std::mutex> lk5((ca + py)->generationMutex);
						std::lock_guard<std::mutex> lk6((ca + nz)->generationMutex);
						std::lock_guard<std::mutex> lk7((ca + pz)->generationMutex);

						shouldGenerate =
							!chunk.validMesh &&
							(ca + nx)->pos == chunkPos + Vec3i(-1, 0, 0) &&
							(ca + px)->pos == chunkPos + Vec3i(1, 0, 0) &&
							(ca + ny)->pos == chunkPos + Vec3i(0, -1, 0) &&
							(ca + py)->pos == chunkPos + Vec3i(0, 1, 0) &&
							(ca + nz)->pos == chunkPos + Vec3i(0, 0, -1) &&
							(ca + pz)->pos == chunkPos + Vec3i(0, 0, 1);

					}

					if (shouldGenerate)
						chunk.GenerateMesh(ca + px, ca + nx, ca + py, ca + ny, ca + pz, ca + nz);
				}
			}
		}
	}
	*/
	/*
	LoadingThreadData& data = world->loadingThreadData;	
	Vec3<size_t> relativePos;	
	Vec3<size_t> indexPos;
	Vec3i chunkPos;
	size_t index;

	while (true)
	{
		Console::Write("[" + String::Convert(localData.threadIndex) + "] Waiting work\n");

		++data.fence1;
		while (data.fence1 != 0)
			std::this_thread::sleep_for(std::chrono::microseconds(10));
		--data.fence2;
		while (data.fence2 != 0)
			std::this_thread::sleep_for(std::chrono::microseconds(10));

		Console::Write("[" + String::Convert(localData.threadIndex) + "] Started work\n");

		if (data.exit)
		{			
			Logger::AddLog(LogType::Message, BLAZE_FILE_NAME, __FUNCSIG__, __LINE__, "Client", "Exiting");
			return;
		}

		static std::mutex message_mutex;
		{
			std::lock_guard<std::mutex> lk{ message_mutex };
			//Console::Write(
			//	"[" + String::Convert(localData.threadIndex) + "]:\nGenerating blocks from " + String::Convert(localData.blockGenerationBegin) +
			//	" to " + String::Convert(localData.blockGenerationEnd) + "\n");
			//Console::Write(
			//	"Generating mesh from " + String::Convert(localData.meshGenerationBegin) +
			//	" to " + String::Convert(localData.meshGenerationEnd) + "\n");
		}
		for (relativePos.z = localData.blockGenerationBegin.z; relativePos.z < localData.blockGenerationEnd.z; ++relativePos.z)
		{
			indexPos.z = world->CalculateChunkIndexPosZ(relativePos.z);
			chunkPos.z = world->CalculateChunkPosZ(relativePos.z);

			//Console::Write("\n");

			for (relativePos.y = localData.blockGenerationBegin.y; relativePos.y < localData.blockGenerationEnd.y; ++relativePos.y)
			{
				indexPos.y = world->CalculateChunkIndexPosY(relativePos.y);
				chunkPos.y = world->CalculateChunkPosY(relativePos.y);

				//Console::Write("\n");

				for (relativePos.x = localData.blockGenerationBegin.x; relativePos.x < localData.blockGenerationEnd.x; ++relativePos.x)
				{
					indexPos.x = world->CalculateChunkIndexPosX(relativePos.x);
					chunkPos.x = world->CalculateChunkPosX(relativePos.x);

					index = world->CalculateChunkIndex(indexPos);

					//Console::Write(String::Convert(chunkPos) + " ");

					world->chunkArray[index].GenerateBlocks(chunkPos);
				}
			}
			//Console::Write("\n");
		}

		for (relativePos.z = localData.meshGenerationBegin.z; relativePos.z < localData.meshGenerationEnd.z; ++relativePos.z)
		{
			indexPos.z = world->CalculateChunkIndexPosZ(relativePos.z);			

			for (relativePos.y = localData.meshGenerationBegin.y; relativePos.y < localData.meshGenerationEnd.y; ++relativePos.y)
			{
				indexPos.y = world->CalculateChunkIndexPosY(relativePos.y);				

				for (relativePos.x = localData.meshGenerationBegin.x; relativePos.x < localData.meshGenerationEnd.x; ++relativePos.x)
				{
					indexPos.x = world->CalculateChunkIndexPosX(relativePos.x);					
					index = world->CalculateChunkIndex(indexPos);
										
					world->GenerateChunkMesh(world->chunkArray[index], relativePos, indexPos);
				}
			}
		}
	}
	*/
}

World::World()
	: centerPos(0, 0, 0), loadDistance(0), loadSide(0), loadSideSqr(0), viewDistance(0), chunkCount(0), chunkArray(nullptr), indexOffset(0, 0, 0), loadingThreadArray(nullptr), loadingThreadCount(0), loadingThreadExitFlag(false)
{		
	va.SetVertexAttributeData(0, AttributeData(&vb, sizeof(Vertex), 0, AttributeType::Int32, 3, false, 0));
	va.SetVertexAttributeData(1, AttributeData(&vb, sizeof(Vertex), 0, AttributeType::Int32, 3, false, sizeof(int32) * 3));
	va.SetVertexAttributeData(2, AttributeData(&vb, sizeof(Vertex), 0, AttributeType::Int32, 3, false, sizeof(int32) * 6));
	va.SetVertexAttributeData(3, AttributeData(&vb, sizeof(Vertex), 0, AttributeType::Uint8, 4, true, sizeof(int32) * 9));	

	vb.AllocateData(BufferView(nullptr, maxVertexCount * sizeof(Vertex)));		

	loadingThreadData.fence1 = 0;
	loadingThreadData.fence2 = loadingThreadCount;
	loadingThreadCount = Hardware::CPU::NumberOfCores() - 1;
	for (size_t i = 0; i < loadingThreadCount; ++i)
		new (loadingThreadArray + i)std::thread(LoadingThreadFunction, this, i);		
	loadingThreadArray = (std::thread*)new uint8[sizeof(std::thread) * loadingThreadCount];
}

World::~World()
{		
	WaitForLoadingThreads();
	loadingThreadData.exitFlag = true;	
	WakeUpLoadingThreads();

	for (size_t i = 0; i < loadingThreadCount; ++i)
	{
		loadingThreadArray[i].join();
		loadingThreadArray[i].~thread();
	}	

	delete[] chunkArray;
}


void World::SetViewDistance(size_t newViewDistance)
{	
	WaitForLoadingThreads();
	size_t newLoadDistance = newViewDistance + 1;	
	centerPos.x += loadDistance - newLoadDistance;	
	centerPos.y += loadDistance - newLoadDistance;
	centerPos.z += loadDistance - newLoadDistance;

	delete[] chunkArray;
	
	viewDistance = newViewDistance;
	loadDistance = newLoadDistance;	
	loadSide = loadDistance * 2;
	loadSideSqr = loadSide * loadSide;
	
	chunkCount = loadSide * loadSide * loadSide;	
	chunkArray = new Chunk[chunkCount];

	RecaluclateIndexOffset();	
}

void World::UpdateChunks(Vec3f pos)
{
	cameraPos = pos;
	Vec3i delta;	
	Vec3i newCenterPos;	
	newCenterPos.x = std::floor((pos.x + Chunk::chunkSize / 2) / Chunk::chunkSize) - loadDistance;
	newCenterPos.y = std::floor((pos.y + Chunk::chunkSize / 2) / Chunk::chunkSize) - loadDistance;
	newCenterPos.z = std::floor((pos.z + Chunk::chunkSize / 2) / Chunk::chunkSize) - loadDistance;
	delta = newCenterPos - centerPos;
	centerPos = newCenterPos;

	RecaluclateIndexOffset();

	static size_t y = 0;
	if (Input::GetKeyState(Key::R).state == KeyState::Pressed)
	{
		y = (y + 1) % loadSide;
		Console::Write("y: " + String::Convert(y) + "\n");
	}
	if (Input::GetKeyState(Key::F).state == KeyState::Pressed)
	{
		y = (y + loadSide - 1) % loadSide;
		Console::Write("y: " + String::Convert(y) + "\n");
	}
	if (Input::GetKeyState(Key::T).state == KeyState::Pressed)
	{
		for (size_t z = 0; z < loadSide; ++z)
		{
			for (size_t x = 0; x < loadSide; ++x)
			{
				Vec3<size_t> indexPos = CalculateChunkIndexPos(Vec3<size_t>(x, y, z));
				size_t index = CalculateChunkIndex(indexPos);

				std::lock_guard<std::mutex> lk(chunkArray[index].meshMutex);
				if (chunkArray[index].validMesh)
					Console::Write("1");
				else
					Console::Write("0");
			}
			Console::Write("\n");
		}

		Console::Write("\n");
	}

		
	/*
	while (delta.x != 0)
		MoveCenterX(delta.x);
	while (delta.y != 0)
		MoveCenterY(delta.y);
	while (delta.z != 0)
		MoveCenterZ(delta.z);
		*/

	/*
	//Generate meshes
	{		
		for (int z = 0; z < loadSide; ++z)
		{
			for (int y = 0; y < loadSide; ++y)
			{
				for (int x = 0; x < loadSide; ++x)
				{
					Vec3i indexPos = CalculateChunkIndexPos(x, y, z);
					size_t i = indexPos.x + indexPos.y * loadSide + indexPos.z * loadSideSqr;
					Chunk* chunk = chunkArray + i;

					if (x > 0 && x < loadSide - 1 &&
						y > 0 && y < loadSide - 1 &&
						z > 0 && z < loadSide - 1 &&
						!chunk->hasMesh)
					{
						size_t nx = CalculateChunkIndex(CalculateChunkIndexPosX(x - 1), indexPos.y, indexPos.z);
						size_t px = CalculateChunkIndex(CalculateChunkIndexPosX(x + 1), indexPos.y, indexPos.z);
						size_t ny = CalculateChunkIndex(indexPos.x, CalculateChunkIndexPosY(y - 1), indexPos.z);
						size_t py = CalculateChunkIndex(indexPos.x, CalculateChunkIndexPosY(y + 1), indexPos.z);
						size_t nz = CalculateChunkIndex(indexPos.x, indexPos.y, CalculateChunkIndexPosZ(z - 1));
						size_t pz = CalculateChunkIndex(indexPos.x, indexPos.y, CalculateChunkIndexPosZ(z + 1));
						chunk->GenerateMesh(chunkArray + px, chunkArray + nx, chunkArray + py, chunkArray + ny, chunkArray + pz, chunkArray + nz);
					}
				}				
			}			
		}
	}*/
}

void World::Render(const Mat4f &VP3D)
{
	VertexArray::Bind(va);
	material.defaultProperties.MVP.value = VP3D;
	material.cameraPos.value = cameraPos;
	material.viewDistance.value = (loadDistance - 2) * Chunk::chunkSize;
	material.clearColor.value = Renderer::GetClearColor().ToVector();
	material.UpdateProperties();
	material.GetProgram()->Use();

	size_t vertexCount = 0;	
	size_t renderCalls = 0;	

	Vec3<size_t> relativePos;
	Vec3<size_t> indexPos;
	size_t index;
	for (relativePos.z = loadDistance - viewDistance; relativePos.z < loadDistance + viewDistance; ++relativePos.z)
	{
		indexPos.z = CalculateChunkIndexPosZ(relativePos.z);
		for (relativePos.y = loadDistance - viewDistance; relativePos.y < loadDistance + viewDistance; ++relativePos.y)
		{
			indexPos.y = CalculateChunkIndexPosY(relativePos.y);
			for (relativePos.x = loadDistance - viewDistance; relativePos.x < loadDistance + viewDistance; ++relativePos.x)
			{
				indexPos.x = CalculateChunkIndexPosX(relativePos.x);

#ifdef WORLD_DEBUG
				_ASSERT(relativePos.x > 0 && relativePos.x < loadSide - 1);
				_ASSERT(relativePos.y > 0 && relativePos.y < loadSide - 1);
				_ASSERT(relativePos.z > 0 && relativePos.z < loadSide - 1);
#endif

				index = CalculateChunkIndex(indexPos);

				Chunk& chunk = chunkArray[index];
				std::lock_guard<std::mutex> lk(chunk.meshMutex);

				if (chunk.validMesh && chunk.vertexCount > 0)
				{

					size_t chunkVertexCountToRender = chunk.vertexCount;
					size_t chunkVertexOffset = 0;
					while (vertexCount + chunkVertexCountToRender > maxVertexCount)
					{
						chunkVertexCountToRender -= maxVertexCount - vertexCount;
						vb.ChangeData(BufferView(chunk.vertices + chunkVertexOffset, (maxVertexCount - vertexCount) * sizeof(Vertex)), vertexCount * sizeof(Vertex));

						Renderer::RenderPointArray(maxVertexCount, 0);
						++renderCalls;

						chunkVertexOffset += maxVertexCount - vertexCount;
						vertexCount = 0;
						//vb.ChangeData(BufferView(chunk.vertices, (maxVertexCount - vertexCount) * sizeof(Vertex)), vertexCount * sizeof(Vertex));
						//Renderer::RenderPointArray(maxVertexCount, 0);
						//chunkVertexOffset += maxVertexCount - vertexCount;
						//++renderCalls;
						//vertexCount = 0;
					}

					vb.ChangeData(BufferView(chunk.vertices + chunkVertexOffset, chunkVertexCountToRender * sizeof(Vertex)), vertexCount * sizeof(Vertex));
					vertexCount += chunkVertexCountToRender;					
				}
			}
		}
	}	

	if (vertexCount > 0)
	{		
		Renderer::RenderPointArray(vertexCount, 0);		
		++renderCalls;
	}
}