#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"
#include "BlazeEngineCore/DataStructures/Map.h"
#include "BlazeEngineCore/DataStructures/List.h"
#include "BlazeEngineCore/DataStructures/DualList.h"
#include "BlazeEngineCore/DataStructures/Array.h"
#include "BlazeEngineCore/Memory/MemoryListener.h"
#include "BlazeEngineCore/File/File.h"
#include <mutex>

namespace Blaze
{
	namespace Debug
	{
		class LoggerListener;
	}

	struct AllocationHeader
	{
		size_t size;
	};
	struct MemoryAllocation
	{
		void* ptr;
		size_t size;
		char location[128];
	};

	class BlazeEngineCoreContext
	{
	public:		
		struct LoggerOutputStreamData
		{		
			WriteStream* writeStream;			
		};
		struct LoggerOutputFileData
		{		
			File file;			
		};

		std::recursive_mutex contextMutex;

		Map<std::thread::id, uint> threadIDs;
		List<Debug::LoggerListener*> loggerListeners;
		Map<Path, LoggerOutputFileData> loggerOutputFiles;
		Array<LoggerOutputStreamData> loggerOutputStreams;

		List<MemoryListener*, MallocAllocator> memoryListeners;
		File memoryLogFile;
		DualList<MemoryAllocation, MallocAllocator> memoryAllocations;
		Map<void*, decltype(memoryAllocations)::Iterator, Hash<void*>, MallocAllocator> allocationMap;

		BlazeEngineCoreContext();
		~BlazeEngineCoreContext();		
	};	

	BLAZE_CORE_API uint GetThisThreadID();	


	extern BlazeEngineCoreContext blazeEngineCoreContext;
}