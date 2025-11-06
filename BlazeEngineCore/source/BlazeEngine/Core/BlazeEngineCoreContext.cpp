#include "pch.h"
#include "BlazeEngine/Core/BlazeEngineCoreContext.h"
#include "BlazeEngine/Core/File/File.h"
#include "BlazeEngine/Core/Container/DualListImpl.h"
#include "BlazeEngine/Core/String/String.h"

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
			memoryLogFile.Open("logs/memory.txt", FileAccessPermission::Write, FileOpenParameters{ .openOption = FileOpenOptions::CreateNew});

		atexit(SaveMemoryLog);

		Debug::Logger::AddOutputStream(consoleOutputStream, true);

		LARGE_INTEGER _tickFrequency;
		QueryPerformanceFrequency(&_tickFrequency);
		tickFrequency = _tickFrequency.QuadPart;

		LARGE_INTEGER _startTicks;
		QueryPerformanceCounter(&_startTicks);
		startTicks = _startTicks.QuadPart;

	}
	BlazeEngineCoreContext::~BlazeEngineCoreContext()
	{
	}

	uint GetThisThreadID()
	{
		return blazeEngineCoreContext.threadIDs.Insert(std::this_thread::get_id(), (uint)blazeEngineCoreContext.threadIDs.Count()).iterator->value;
	}
}