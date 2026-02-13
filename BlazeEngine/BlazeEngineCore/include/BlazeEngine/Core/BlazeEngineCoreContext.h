#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Container/Map.h"
#include "BlazeEngine/Core/Container/List.h"
#include "BlazeEngine/Core/Container/DualList.h"
#include "BlazeEngine/Core/Container/Array.h"
#include "BlazeEngine/Core/Memory/MemoryListener.h"
#include "BlazeEngine/Core/File/File.h"
#include "BlazeEngine/Core/Console/ConsoleOutputStream.h"
#include "BlazeEngine/Core/Debug/Log.h"
#include "BlazeEngine/External/BlazeEngineCoreSTDInterface.h"
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
			bool isStyledWriteStream;
			bool insideStyleTag;
		};
		struct LoggerOutputFileData
		{
			File file;
		};

		std::recursive_mutex contextMutex;

		ConsoleOutputStream consoleOutputStream;

		Map<std::thread::id, uint> threadIDs;
		List<Debug::LoggerListener*> loggerListeners;
		Map<Path, LoggerOutputFileData> loggerOutputFiles;
		Array<LoggerOutputStreamData> loggerOutputStreams;
		Debug::LogType logBreakpointMask = Debug::LogType::Fatal;
		Debug::LogType logCallstackPrintMask = Debug::LogType::Fatal | Debug::LogType::Error;

		List<MemoryListener*, MallocAllocator> memoryListeners;
		File memoryLogFile;
		DualList<MemoryAllocation, MallocAllocator> memoryAllocations;
		Map<void*, decltype(memoryAllocations)::Iterator, Hash<void*>, MallocAllocator> allocationMap;

		uint64 tickFrequency;
		uint64 startTicks;

		BlazeEngineCoreContext();
		~BlazeEngineCoreContext();
	};

	BLAZE_API uint GetThisThreadID();


	extern BlazeEngineCoreContext blazeEngineCoreContext;
}