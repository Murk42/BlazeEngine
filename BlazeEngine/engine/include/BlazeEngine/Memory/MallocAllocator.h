#pragma once

namespace Blaze
{	
	class BLAZE_API MallocAllocator : public AllocatorBase
	{
	public:				
		static void* Allocate(uint byteCount);
		static void Free(void* ptr);
	};
}