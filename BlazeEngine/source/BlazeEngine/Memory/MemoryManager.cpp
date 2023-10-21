#include "pch.h"
#include "BlazeEngine/Memory/MemoryManager.h"
#include "BlazeEngine/Memory/MallocAllocator.h"
#include "BlazeEngine/Memory/MemoryListener.h"
#include "BlazeEngine/DataStructures/List.h"
#include "BlazeEngine/DataStructures/HybridArray.h"
using namespace Blaze;

#include "BlazeEngine/File/File.h"

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

static constexpr bool logAllocationsToFile = false;
static constexpr bool trackAllocations = false;

static std::mutex file_mutex;
static File memoryFile;

static std::mutex listeners_mutex;
static List<MemoryListener*, MallocAllocator> listeners;
static bool activeListeners;

static std::mutex allocation_mutex;
static DualList<MemoryAllocation, MallocAllocator> memoryAllocations;
static Map<void*, decltype(memoryAllocations)::Iterator, Hash<void*>, MallocAllocator> allocationMap;


namespace Blaze
{	
	void SaveMemoryLog()
	{		
		if (logAllocationsToFile)
		{
			std::lock_guard<std::mutex> lk { file_mutex};

			char buffer[128];
			uintMem messageSize = 0;

			messageSize = sprintf_s(buffer, 128, "%u unfreed allocations\n\n", (uint)memoryAllocations.Count());
			memoryFile.Write(buffer, messageSize);

			for (auto& allocation : memoryAllocations)
			{
				messageSize = sprintf_s(buffer, 128, "(%p) %u bytes \n%s\n", allocation.ptr, (uint)allocation.size, allocation.location);
				memoryFile.Write(buffer, messageSize);
			}


			memoryFile.Close();
		}
	}

	TimingResult InitializeMemory()
	{
		Timing timing{ "Memory manager" };

		if (logAllocationsToFile)
		{
			if (!std::filesystem::exists("logs"))
				std::filesystem::create_directories("logs");
			if (memoryFile.Open("logs/memory.txt", FileAccessPermission::Write))
				exit(1);
		}
		
		atexit(SaveMemoryLog);
		activeListeners = true;		

		return timing.GetTimingResult();
	}	
	void TerminateMemory()
	{
		activeListeners = true;
	}
		
	namespace Memory
	{
		void AddListener(MemoryListener* listener)
		{
			std::lock_guard<std::mutex> lk { listeners_mutex };
			listeners.AddBack(listener);
		}
		void RemoveListener(MemoryListener* listener)
		{			
			std::lock_guard<std::mutex> lk { listeners_mutex };
			if (listeners.First() == listener)
				listeners.EraseFirst();
			else
			{
				auto it = listeners.FirstIterator();
				auto next = ++it;
				auto end = listeners.BehindIterator();

				while (next != end && *next != listener)
				{
					it = next;
					++next;
				}

				listeners.EraseAfter(it);
			}
		}
		void ChangeListener(MemoryListener* old, MemoryListener* ptr)
		{
			std::lock_guard<std::mutex> lk { listeners_mutex };
			auto it = std::find(listeners.FirstIterator(), listeners.BehindIterator(), old);
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
				String fileName = it->FilePath().FileName();
				String fileLine = StringParsing::Convert(it->FileLine());
				
				uintMem sizeNeeded = fileName.Size() + fileLine.Size() + 2;

				if (sizeNeeded > sizeLeft)
					break;

				memcpy(ptr, fileName.Ptr(), fileName.Size());
				ptr += fileName.Size();

				ptr[0] = ' ';
				ptr += 1;
				
				memcpy(ptr, fileLine.Ptr(), fileLine.Size());
				ptr += fileLine.Size();

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

			if (activeListeners)
			{
				MemoryEvent event{ MemoryEventType::Allocation, ptr, size };
				for (auto& l : listeners)
					l->AddMemoryEvent(event);
			}			

			void* outPtr = (char*)ptr + sizeof(AllocationHeader);

			if (trackAllocations)
			{
				std::lock_guard<std::mutex> lk { allocation_mutex };

				auto it = memoryAllocations.AddBack();
				it->size = size;
				it->ptr = ptr;
				GetLocation(it->location);

				allocationMap.Insert(outPtr, it);
			}

			return outPtr;
		}		
		void Free(void* ptr)
		{			
			if (ptr == nullptr)
				return;

			if (trackAllocations)
			{
				std::lock_guard<std::mutex> lk { allocation_mutex };

				auto allocationIt = allocationMap.Find(ptr);
				if (!allocationIt.IsNull())
				{
					memoryAllocations.Erase(allocationIt->value);					
					allocationMap.Erase(allocationIt);
				}
			}

			ptr = (char*)ptr - sizeof(AllocationHeader);
			AllocationHeader* header = (AllocationHeader*)ptr;						

			if (activeListeners)
			{
				MemoryEvent event{ MemoryEventType::Deallocation, ptr, header->size };
				for (auto& l : listeners)
					l->AddMemoryEvent(event);
			}
			
			free(ptr);
		}
		BLAZE_API void* Reallocate(uintMem size, void* old)
		{
			Free(old);
			return Allocate(size);
		}
	}
}