#include "pch.h"
#include "BlazeEngine/Core/Memory/MemoryListener.h"

namespace Blaze
{
	namespace Memory
	{
		void AddListener(MemoryListener* listener);
		void RemoveListener(MemoryListener* listener);
		void ChangeListener(MemoryListener* old, MemoryListener* ptr);
	}

	MemoryListener::MemoryListener()
		: listening(false), recordLocations(false), paused(false)
	{

	}
	MemoryListener::MemoryListener(bool startListening, bool recordLocations)
		: MemoryListener()
	{
		if (startListening)
			StartListening(recordLocations);
	}
	MemoryListener::MemoryListener(MemoryListener&& o) noexcept
		: events(std::move(o.events)), eventLocations(std::move(o.eventLocations)), listening(o.listening), recordLocations(o.recordLocations), paused(false)
	{
		if (o.listening)
			Memory::ChangeListener(&o, this);
	}
	MemoryListener::~MemoryListener()
	{

	}
	void MemoryListener::AddMemoryEvent(const MemoryEvent& _event)
	{
		if (paused)
			return;

		MemoryEvent event = _event;

		if (recordLocations)
		{
			paused = true;
			{
				Debug::Callstack callstack{ 2 };

				if (callstack.Count() != 0)
				{
					event.locationDataIndex = eventLocations.Count();

					auto& eventLocation = *eventLocations.AddBack();
					for (uintMem i = 0; i < callstack.Count() && i < _countof(MemoryEventLocationData::stackFrames); ++i)
					{
						auto& stackFrame = callstack[i];
						auto& dstStackFrame = eventLocation.stackFrames[i];
						dstStackFrame.address = stackFrame.Address();
						strncpy_s((char*)dstStackFrame.file, sizeof(dstStackFrame.file), (const char*)stackFrame.GetFilePath().Ptr(), stackFrame.GetFilePath().Count());
						strncpy_s((char*)dstStackFrame.locationName, sizeof(dstStackFrame.locationName), (const char*)stackFrame.GetLocationName().Ptr(), stackFrame.GetLocationName().Count());
						dstStackFrame.line = stackFrame.GetFileLine();
						++eventLocation.frameCount;
					}
				}
				else
					event.locationDataIndex = SIZE_MAX;
			}
			paused = false;
		}
		else
			event.locationDataIndex = SIZE_MAX;

		events.AddBack(event);
	}
	void MemoryListener::StartListening(bool recordLocations)
	{
		this->recordLocations = recordLocations;

		if (listening)
			return;

		Memory::AddListener(this);
		listening = true;
	}
	void MemoryListener::StopListening()
	{
		if (!listening)
			return;

		Memory::RemoveListener(this);
		listening = false;
	}

	MemoryListenerReport MemoryListener::GenerateReport() const
	{
		MemoryListenerReport report{ };
		report.totalAllocations = 0;
		report.totalDeallocations = 0;
		report.peakAllocatedSize = 0;
		report.unfreedEventCount = 0;

		uintMem allocatedAmount = 0;
		for (uintMem i = 0; i < events.Count(); ++i)
		{
			switch (events[i].type)
			{
			case MemoryEventType::Allocation:
				++report.totalAllocations;
				allocatedAmount += events[i].size;
				report.peakAllocatedSize = report.peakAllocatedSize > allocatedAmount ? report.peakAllocatedSize : allocatedAmount;
				break;
			case MemoryEventType::Deallocation:
				++report.totalDeallocations;
				allocatedAmount -= events[i].size;
				break;
			}

			if (events[i].type == MemoryEventType::Allocation)
			{
				uintMem j = i + 1;
				for (; j < events.Count(); ++j)
					if (events[j].type == MemoryEventType::Deallocation && events[i].ptr == events[j].ptr)
						break;

				if (j == events.Count() && report.unfreedEventCount < _countof(report.unfreedEventIndices))
					report.unfreedEventIndices[report.unfreedEventCount++] = i;
			}
		}

		report.unfreedSize = allocatedAmount;

		return report;
	}

	void MemoryListener::ClearEvents()
	{
		events.Clear();
	}

	MemoryListener& MemoryListener::operator=(MemoryListener&& o) noexcept
	{
		events = std::move(o.events);
		eventLocations = std::move(o.eventLocations);
		listening = o.listening;
		recordLocations = o.recordLocations;
		if (o.listening)
			Memory::ChangeListener(&o, this);
		return *this;
	}
	String MemoryListenerReport::Format()
	{
		return ::Blaze::Format(
			"totalAllocations: {}\n"
			"totalDeallocations: {}\n"
			"peakAllocatedSize: {}\n"
			"unfreedSize: {}\n"
			"\n"
			"unfreedEventCount: {}"
			, totalAllocations, totalDeallocations, peakAllocatedSize, unfreedSize, unfreedEventCount);
	}
}