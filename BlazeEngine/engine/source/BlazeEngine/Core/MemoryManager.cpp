#include "BlazeEngine/Core/MemoryManager.h"
#include "BlazeEngine/Console/Console.h"
#include "BlazeEngine/DataStructures/String.h"
using namespace Blaze;

#include <memory>
#include <cstdio>

#include "BlazeEngine/File/File.h"

struct AllocationData
{
	int type;
	size_t size;
	void* ptr;
};
struct MemoryReport
{
	size_t count;
	AllocationData data[1024 * 16];
};
struct AllocationHeader
{
	size_t dataIndex;
};

//File file;
size_t allocCount = 0;
size_t freeCount = 0;

void* operator new(size_t size)
{
	//if (report == nullptr)
	//{		
	//	//std::lock_guard<std::mutex> lk(m);
	//
	//	report = (MemoryReport*)malloc(sizeof(MemoryReport));
	//	new (report) MemoryReport();
	//
	//	data = report->data;
	//	report->count = 0;
	//
	//	data = report->data;
	//}
	//if (!file.IsOpen())
	//	file.Open("memory.txt", FileOpenMode::Write, FileOpenFlags::Truncate);
	
	return Memory::Allocate(size);
}	
void operator delete(void* ptr) {
	Memory::Free(ptr);
}

extern "C" BLAZE_API void SetMemoryReport(MemoryReport* newReport)
{
	//std::lock_guard<std::mutex> lk(m);
	//std::lock_guard<std::mutex> lk2(newReport->m);
	//newReport->count = report->count;
	//memcpy(newReport->data, report->data, sizeof(AllocationData) * report->count);
	//report->~MemoryReport();
	//free(report);
	//report = newReport;
	//data = report->data;
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
	
	void InitializeMemory()
	{
	}
	void TerminateMemory()
	{
	}
	
	namespace Memory
	{
		void* Allocate(size_t size)
		{
			void* ptr = malloc(size + sizeof(AllocationHeader));

			AllocationHeader* header = (AllocationHeader*)ptr;
			//header->dataIndex = report->count;
			
			allocCount++;
			
			//char buffer[128];
			//uint messageSize = sprintf_s(buffer, 128, "%u %p %u", allocCount, ptr, size);
			//file.Write({ buffer, messageSize });			

			
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

			//{
			//	std::lock_guard<std::mutex> lk(report->m);
			//
			//	data[report->count].type = 1;
			//	data[report->count].ptr = ptr;
			//	data[report->count].size = data[header->dataIndex].size;
			//	report->count++;
			//
			//	if (report->count >= 1024 * 16)
			//		throw;
			//
				freeCount++;
			//
			//	if (allocCount + freeCount != report->count)
			//		throw;
			//}
			
			free(ptr);
		}
	}
}