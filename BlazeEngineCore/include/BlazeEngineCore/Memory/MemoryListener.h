#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"
#include "BlazeEngineCore/DataStructures/Array.h"
#include "BlazeEngineCore/Memory/MallocAllocator.h"

namespace Blaze
{
	enum class MemoryEventType
	{
		Allocation,
		Deallocation
	};
	struct MemoryEvent
	{
		MemoryEventType type;
		void* ptr;
		size_t size;
	};

	class BLAZE_CORE_API MemoryListener
	{
	public:
		using ContainerType = Array<MemoryEvent, MallocAllocator>;
	private:
		bool listening;
		ContainerType events;
	public:
		MemoryListener();
		MemoryListener(const MemoryListener&) = delete;
		MemoryListener(MemoryListener&&) noexcept;
		~MemoryListener();

		void AddMemoryEvent(const MemoryEvent&);

		void StartListening();
		void StopListening();

		void ClearEvents();

		bool IsListening() const { return listening; }

		const ContainerType& GetEvents() const { return events; }

		MemoryListener& operator=(const MemoryListener&) = delete;
		MemoryListener& operator=(MemoryListener&&) noexcept;
	};
}