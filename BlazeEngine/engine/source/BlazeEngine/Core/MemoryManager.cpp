#include "BlazeEngine/Core/MemoryManager.h"
#include "BlazeEngine/Console/Console.h"
#include "BlazeEngine/DataStructures/String.h"
using namespace Blaze;

#include <memory>
#include <mutex>

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

std::mutex* m;
MemoryReport* report = nullptr;
AllocationData* data = nullptr;
size_t allocCount = 0;
size_t freeCount = 0;

void* operator new(size_t size) {
	if (report == nullptr)
	{
		static std::mutex _m;
		m = &_m;
		std::lock_guard<std::mutex> lk(*m);

		report = (MemoryReport*)malloc(sizeof(MemoryReport));
		data = report->data;
		report->count = 0;

		data = report->data;
	}
	
	return Memory::Allocate(size);
}	
void operator delete(void* ptr) {
	Memory::Free(ptr);
}

extern "C" BLAZE_API MemoryReport* GetMemoryReport()
{
	return report;
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
			header->dataIndex = report->count;

			{
				std::lock_guard<std::mutex> lk(*m);
				data[report->count].type = 0;
				data[report->count].ptr = ptr;
				data[report->count].size = size;
				report->count++;

				if (report->count >= 1024 * 16)
					throw;

				allocCount++;
			}

			
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

			{
				std::lock_guard<std::mutex> lk(*m);

				data[report->count].type = 1;
				data[report->count].ptr = ptr;
				data[report->count].size = data[header->dataIndex].size;
				report->count++;

				if (report->count >= 1024 * 16)
					throw;

				freeCount++;
			}
			
			free(ptr);
		}
	}
}