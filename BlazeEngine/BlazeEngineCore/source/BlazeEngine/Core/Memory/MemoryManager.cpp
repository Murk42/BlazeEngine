#include "pch.h"
#include "BlazeEngine/Core/Memory/MemoryManager.h"
#include "BlazeEngine/Core/BlazeEngineCoreContext.h"
#include "BlazeEngine/Core/Debug/Logger.h"
#include "BlazeEngine/Core/Debug/Callstack.h"

namespace Blaze::Memory
{
	static constexpr bool trackAllocations = false;

	void AddListener(MemoryListener* listener)
	{
		std::lock_guard lk{ blazeEngineCoreContext.contextMutex };
		blazeEngineCoreContext.memoryListeners.AddFront(listener);
	}
	void RemoveListener(MemoryListener* listener)
	{
		std::lock_guard lk{ blazeEngineCoreContext.contextMutex };
		if (blazeEngineCoreContext.memoryListeners.First() == listener)
			blazeEngineCoreContext.memoryListeners.EraseFirst();
		else
		{
			auto it = blazeEngineCoreContext.memoryListeners.FirstIterator();
			auto next = ++it;
			auto end = blazeEngineCoreContext.memoryListeners.BehindIterator();

			while (next != end && *next != listener)
			{
				it = next;
				++next;
			}

			blazeEngineCoreContext.memoryListeners.EraseAfter(it);
		}
	}
	void ChangeListener(MemoryListener* old, MemoryListener* ptr)
	{
		std::lock_guard lk{ blazeEngineCoreContext.contextMutex };
		auto it = std::find(blazeEngineCoreContext.memoryListeners.FirstIterator(), blazeEngineCoreContext.memoryListeners.BehindIterator(), old);
		*it = ptr;
	}

	template<size_t size>
	void GetLocation(char(&arr)[size])
	{
		Debug::Callstack callstack = { 1, 5 };
		uintMem sizeLeft = size - 1;

		char* ptr = arr;
		for (const Debug::StackFrame& frame : callstack)
		{
			StringView fileName = frame.GetFilePath();
			fileName = fileName.SubString(fileName.FindReverse("/"), fileName.Count());
			String fileLine = Format("{}", frame.GetFileLine());

			uintMem sizeNeeded = fileName.Count() - 1 + fileLine.Count();

			if (sizeNeeded > sizeLeft)
				break;

			memcpy(ptr, fileName.Ptr(), fileName.Count() - 1);
			ptr += fileName.Count() - 1;

			ptr[0] = ' ';
			ptr += 1;

			memcpy(ptr, fileLine.Ptr(), fileLine.Count());
			ptr += fileLine.Count();

			ptr[0] = '\n';
			ptr += 1;

			sizeLeft -= sizeNeeded;
		}

		*ptr = '\0';
	}

	void* Allocate(uintMem size)
	{
		void* ptr = malloc(size + sizeof(AllocationHeader));

		if (ptr == nullptr)
		{
			BLAZE_LOG_ERROR("Malloc failed with {} bytes", size + sizeof(AllocationHeader));
			return nullptr;
		}

		AllocationHeader* header = (AllocationHeader*)ptr;
		header->size = size;

		MemoryEvent event{ MemoryEventType::Allocation, ptr, size };
		for (auto& l : blazeEngineCoreContext.memoryListeners)
			l->AddMemoryEvent(event);

		void* outPtr = (char*)ptr + sizeof(AllocationHeader);

		if (trackAllocations)
		{
			std::lock_guard lk{ blazeEngineCoreContext.contextMutex };

			auto it = blazeEngineCoreContext.memoryAllocations.AddBack();
			it->size = size;
			it->ptr = ptr;
			GetLocation(it->location);

			blazeEngineCoreContext.allocationMap.Insert(outPtr, it);
		}

		return outPtr;
	}
	void Free(void* ptr)
	{
		if (ptr == nullptr)
			return;

		if (trackAllocations)
		{
			std::lock_guard lk{ blazeEngineCoreContext.contextMutex };

			auto allocationIt = blazeEngineCoreContext.allocationMap.Find(ptr);
			if (!allocationIt.IsNull())
			{
				blazeEngineCoreContext.memoryAllocations.Erase(allocationIt->value);
				blazeEngineCoreContext.allocationMap.Erase(allocationIt);
			}
		}

		ptr = (char*)ptr - sizeof(AllocationHeader);
		AllocationHeader* header = (AllocationHeader*)ptr;

		MemoryEvent event{ MemoryEventType::Deallocation, ptr, header->size };
		for (auto& l : blazeEngineCoreContext.memoryListeners)
			l->AddMemoryEvent(event);

		free(ptr);
	}
	void* Reallocate(uintMem size, void* old)
	{
		Free(old);
		return Allocate(size);
	}
}