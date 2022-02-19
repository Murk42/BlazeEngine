#include "BlazeEngine/Core/MemoryManager.h"
#include "BlazeEngine/Console/Console.h"
#include "BlazeEngine/DataStructures/String.h"
using namespace Blaze;

#include <memory>

void* operator new(size_t size) noexcept
{        
    if (void* ptr = Memory::Allocate(size))
        return ptr;

    throw std::bad_alloc{}; 
}
void* operator new[](size_t size) noexcept
{  
    if (void* ptr = Memory::Allocate(size))
        return ptr;

    throw std::bad_alloc{};
}
void operator delete(void* ptr) noexcept
{       
    Memory::Free(ptr);
}
void operator delete[](void* ptr) noexcept
{       
    Memory::Free(ptr);
}

size_t allocCount = 0;

struct AllocationHeader
{
    size_t index;
    size_t size;
};

namespace Blaze
{
    namespace Memory
    {
        void* Allocate(size_t size)
        {
            void* ptr = malloc(size + sizeof(AllocationHeader));
            AllocationHeader* header = (AllocationHeader*)ptr;
            header->index = allocCount++;
            header->size = size;                        
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
            free(ptr);
        }
    }
}