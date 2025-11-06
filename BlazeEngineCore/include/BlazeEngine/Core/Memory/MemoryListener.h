#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Container/Array.h"
#include "BlazeEngine/Core/Memory/MallocAllocator.h"
#include "BlazeEngine/Core/Debug/Callstack.h"

namespace Blaze
{
	enum class MemoryEventType
	{
		Allocation,
		Deallocation
	};
	struct MemoryEventLocationData
	{
		struct StackFrame
		{
			char8_t file[256];
			uintMem line;
			const void* address;
			char locationName[256];
		};
		StackFrame stackFrames[10];
		uintMem frameCount;
	};
	struct MemoryEvent
	{
		MemoryEventType type;
		void* ptr;
		size_t size;
		uintMem locationDataIndex;
	};
	struct MemoryListenerReport
	{
		uintMem totalAllocations, totalDeallocations;
		uintMem peakAllocatedSize;
		uintMem unfreedSize;

		uintMem unfreedEventIndices[128];
		uintMem unfreedEventCount;

		String Format();
	};

	class BLAZE_API MemoryListener
	{
	public:
		MemoryListener();
		MemoryListener(bool startListening, bool recordLocations = false);
		MemoryListener(const MemoryListener&) = delete;
		MemoryListener(MemoryListener&&) noexcept;
		~MemoryListener();

		void AddMemoryEvent(const MemoryEvent&);

		void StartListening(bool recordLocations = false);
		void StopListening();

		MemoryListenerReport GenerateReport() const;

		void ClearEvents();

		bool IsListening() const { return listening; }
		bool IsRecordingLocations() const { return listening; }

		auto GetEvents() const { return events; }
		auto GetEventLocationData() const { return eventLocations; }

		MemoryListener& operator=(const MemoryListener&) = delete;
		MemoryListener& operator=(MemoryListener&&) noexcept;
	private:
		bool listening, recordLocations, paused;
		Array<MemoryEvent, MallocAllocator> events;
		Array<MemoryEventLocationData, MallocAllocator> eventLocations;
	};
}