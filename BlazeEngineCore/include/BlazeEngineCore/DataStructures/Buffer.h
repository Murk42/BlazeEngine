#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"

namespace Blaze
{
	class BLAZE_CORE_API Buffer
	{
	public:
		Buffer();
		Buffer(const Buffer&) = delete;
		Buffer(Buffer&&) noexcept;
		Buffer(uintMem size);		
		~Buffer();
				
		inline bool Empty() const { return ptr == nullptr; }
		inline void* Ptr() const { return ptr; }

		void Clear();

		void* Allocate(uintMem size);		

		operator void* () const { return ptr; }

		Buffer& operator=(const Buffer&) = delete;
		Buffer& operator=(Buffer&&) noexcept;		
	private:
		void* ptr;
	};
}