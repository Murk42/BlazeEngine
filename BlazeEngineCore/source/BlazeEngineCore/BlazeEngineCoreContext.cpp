#include "pch.h"
#include "BlazeEngineCore/BlazeEngineCoreContext.h"

namespace Blaze
{
	BlazeEngineCoreContext blazeEngineCoreContext;

	static constexpr bool logAllocationsToFile = false;
	
	static void SaveMemoryLog()
	{
		if (logAllocationsToFile)
		{
			std::lock_guard lk{ blazeEngineCoreContext.contextMutex };

			char buffer[128];
			uintMem messageSize = 0;

			messageSize = sprintf_s(buffer, 128, "%u unfreed allocations\n\n", (uint)blazeEngineCoreContext.memoryAllocations.Count());
			blazeEngineCoreContext.memoryLogFile.Write(buffer, messageSize);

			for (auto& allocation : blazeEngineCoreContext.memoryAllocations)
			{
				messageSize = sprintf_s(buffer, 128, "(%p) %u bytes \n%s\n", allocation.ptr, (uint)allocation.size, allocation.location);
				blazeEngineCoreContext.memoryLogFile.Write(buffer, messageSize);
			}			
		}
	}

	BlazeEngineCoreContext::BlazeEngineCoreContext()
	{
		if (logAllocationsToFile)
		{
			if (!std::filesystem::exists("logs"))
				std::filesystem::create_directories("logs");
			if (memoryLogFile.Open("logs/memory.txt", FileAccessPermission::Write))
				exit(1);
		}

		atexit(SaveMemoryLog);
	}
	BlazeEngineCoreContext::~BlazeEngineCoreContext()
	{
	}

	uint GetThisThreadID()
	{
		return blazeEngineCoreContext.threadIDs.Insert(std::this_thread::get_id(), (uint)blazeEngineCoreContext.threadIDs.Count()).iterator->value;
	}
}