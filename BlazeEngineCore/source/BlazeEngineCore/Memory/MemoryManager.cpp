#include "pch.h"
#include "BlazeEngineCore/Utilities/Timing.h"
#include "BlazeEngineCore/Utilities/StringParsing.h"

namespace Blaze::Memory
{
	static constexpr bool trackAllocations = false;

	void AddListener(MemoryListener* listener)
	{
		//std::lock_guard lk{ blazeEngineCoreContext.contextMutex };
		blazeEngineCoreContext.memoryListeners.AddBack(listener);
	}
	void RemoveListener(MemoryListener* listener)
	{
		//std::lock_guard lk{ blazeEngineCoreContext.contextMutex };
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
		//std::lock_guard lk{ blazeEngineCoreContext.contextMutex };
		auto it = std::find(blazeEngineCoreContext.memoryListeners.FirstIterator(), blazeEngineCoreContext.memoryListeners.BehindIterator(), old);
		*it = ptr;
	}

	template<size_t size>
	void GetLocation(char(&arr)[size])
	{
		Debug::Callstack callstack(1, 5);
		uintMem sizeLeft = size - 1;
		auto it = callstack.begin();

		char* ptr = arr;
		while (it != callstack.end())
		{
			StringUTF8 fileName = it->FilePath().FileName();
			String fileLine = StringParsing::Convert(it->FileLine());

			uintMem sizeNeeded = fileName.BufferSize() - 1 + fileLine.Count();

			if (sizeNeeded > sizeLeft)
				break;

			memcpy(ptr, fileName.Buffer(), fileName.BufferSize() - 1);
			ptr += fileName.BufferSize() - 1;

			ptr[0] = ' ';
			ptr += 1;

			memcpy(ptr, fileLine.Ptr(), fileLine.Count());
			ptr += fileLine.Count();

			ptr[0] = '\n';
			ptr += 1;

			sizeLeft -= sizeNeeded;
			++it;
		}

		*ptr = '\0';
	}

	void* Allocate(uintMem size)
	{
		void* ptr = malloc(size + sizeof(AllocationHeader));

		if (ptr == nullptr)
		{
			Debug::Logger::LogError("Blaze Engine", "Malloc failed with " + StringParsing::Convert(size + sizeof(AllocationHeader)) + " bytes");
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
			//std::lock_guard lk{ blazeEngineCoreContext.contextMutex };

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
			//std::lock_guard lk{ blazeEngineCoreContext.contextMutex };

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