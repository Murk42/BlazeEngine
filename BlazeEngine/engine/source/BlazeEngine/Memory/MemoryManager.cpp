#include "BlazeEngine/Memory/MemoryManager.h"
#include "BlazeEngine/Memory/MallocAllocator.h"
#include "BlazeEngine/Memory/MemoryListener.h"
#include "BlazeEngine/DataStructures/List.h"
#include "source/BlazeEngine/Internal/EngineData.h"
using namespace Blaze;

#include <memory>
#include <cstdio>

#include "BlazeEngine/File/File.h"

struct AllocationHeader
{
	size_t size;
};

static File file;
static bool logMemory = false;
static size_t allocCount = 0;
static size_t freeCount = 0;
static size_t memoryLeft;
static List<MemoryListener*, MallocAllocator<ListNode<MemoryListener*>>> listeners;
static bool activeListeners;

void* operator new(size_t size)
{
	return Memory::Allocate(size);
}
void operator delete(void* ptr) {
	Memory::Free(ptr);
}

namespace Blaze
{
	char* itoa(int64_t value, char* result, int base) {
		// check that the base if valid
		if (base < 2 || base > 36) { *result = '\0'; return result; }

		char* ptr = result, * ptr1 = result, tmp_char;
		int64_t tmp_value;

		do {
			tmp_value = value;
			value /= base;
			*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + (tmp_value - value * base)];
		} while (value);

		return ptr;
	}
	
	void SaveMemoryLog()
	{
		logMemory = false;
		file.Close();
	}
	void InitializeMemory()
	{
		if (file.Open("memory.txt", FileOpenMode::Write, FileOpenFlags::Create | FileOpenFlags::Truncate))
			exit(1);
		logMemory = true;

		
		atexit(SaveMemoryLog);
		activeListeners = true;
	}	
	void TerminateMemory()
	{
		activeListeners = true;
	}
	
	namespace Memory
	{
		void AddListener(MemoryListener* listener)
		{
			listeners.AddBack(listener);
		}
		void RemoveListener(MemoryListener* listener)
		{
			if (*listeners.begin() == listener)
				listeners.PopFirst();
			else
			{
				auto it = listeners.begin();
				auto next = ++it;
				while (next != listeners.end() && *next != listener)
				{
					it = next;
					++next;
				}

				listeners.EraseAfter(it);
			}
		}
		void ChangeListener(MemoryListener* old, MemoryListener* ptr)
		{
			//auto it = std::find(listeners.begin(), listeners.end(), old);
			//*it = ptr;
		}

		void* Allocate(size_t size)
		{
			void* ptr = malloc(size + sizeof(AllocationHeader));

			AllocationHeader* header = (AllocationHeader*)ptr;
			header->size = size;
			
			memoryLeft += size;


			if (activeListeners)
			{
				MemoryEvent event{ MemoryEventType::Allocation, ptr, size };
				for (auto& l : listeners)
					l->AddMemoryEvent(event);
			}

			if (logMemory)
			{
				if (engineData != nullptr && engineData->finishedInit)
				{
					char buffer[128];
					uint messageSize = sprintf_s(buffer, 128, "CA %5u %p %5u %7u\n", allocCount, ptr, size, memoryLeft);
					file.Write({ buffer, messageSize });
				}
				else
				{
					char buffer[128];
					uint messageSize = sprintf_s(buffer, 128, "EA %5u %p %5u %7u\n", allocCount, ptr, size, memoryLeft);
					file.Write({ buffer, messageSize });
				}

				//file.Close();
				//file.Open("memory.txt", FileOpenMode::Write, FileOpenFlags::Create | FileOpenFlags::Truncate);
			}

			allocCount++;
			
			return (char*)ptr + sizeof(AllocationHeader);
		}
		void* Allocate(size_t size, const char* fileName, int line)
		{			
			return malloc(size);
		}
		void* Allocate(size_t size, const char* fileName, int line, const char* usage)
		{
			return malloc(size);
		}
		void Free(void* ptr)
		{
			if (ptr == nullptr)
				return;
			
			ptr = (char*)ptr - sizeof(AllocationHeader);
			AllocationHeader* header = (AllocationHeader*)ptr;

			memoryLeft -= header->size;

			if (activeListeners)
			{
				MemoryEvent event{ MemoryEventType::Deallocation, ptr, header->size };
				for (auto& l : listeners)
					l->AddMemoryEvent(event);
			}

			if (logMemory)
			{
				if (engineData != nullptr && engineData->finishedInit)
				{
					char buffer[128];
					uint messageSize = sprintf_s(buffer, 128, "CF %5u %p %5u %7u\n", freeCount, ptr, header->size, memoryLeft);
					file.Write({ buffer, messageSize });
				}
				else
				{
					char buffer[128];
					uint messageSize = sprintf_s(buffer, 128, "EF %5u %p %5u %7u\n", freeCount, ptr, header->size, memoryLeft);
					file.Write({ buffer, messageSize });
				}
				//file.Close();
				//file.Open("memory.txt", FileOpenMode::Write, FileOpenFlags::Create | FileOpenFlags::Truncate);
			}
			
			freeCount++;
			
			free(ptr);
		}
	}
}