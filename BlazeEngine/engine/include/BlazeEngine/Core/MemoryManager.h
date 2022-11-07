#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include <malloc.h>

//void* operator new  (size_t count)
//{
//	return nullptr;
//}

//BLAZE_API void* operator new[](std::size_t count);
//
//BLAZE_API void* operator new  (std::size_t count, std::align_val_t al);
//
//BLAZE_API void* operator new[](std::size_t count, std::align_val_t al);


namespace Blaze
{
	namespace Memory
	{		
		BLAZE_API void* Allocate(size_t size);
		BLAZE_API void Free(void* ptr);

		namespace Core
		{
			struct MemoryManagerContext
			{
			};

			BLAZE_API void SetupMemoryManagerContext(MemoryManagerContext context);
		}
	}
}