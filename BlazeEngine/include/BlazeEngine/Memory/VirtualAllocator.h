#pragma once
#include "BlazeEngine/Memory/Allocator.h"

namespace Blaze
{
	class BLAZE_API VirtualAllocator;

	template<typename T>
	concept IsVirtualAllocator = std::derived_from<T, VirtualAllocator>&& std::constructible_from<T, T&&>;
	template<typename T>
	concept VirtualAllocatorType = IsVirtualAllocator<T>;

	class BLAZE_API VirtualAllocator : public AllocatorBase
	{
	public:		
		VirtualAllocator();		
		VirtualAllocator(const VirtualAllocator&) = delete;
		VirtualAllocator(VirtualAllocator&&) noexcept;
		template<typename T> requires IsVirtualAllocator<T>
		VirtualAllocator(T&& implementation);		

		virtual void* Allocate(uint bytes);
		virtual void Free(void* ptr);
		virtual void* Reallocate(uint bytes, void* old);

		VirtualAllocator& operator=(const VirtualAllocator&) = delete;
		VirtualAllocator& operator=(VirtualAllocator&&) noexcept = delete;
	private:		
		VirtualAllocator* implementation;
	};
	
	template<typename T> requires IsVirtualAllocator<T>
	VirtualAllocator::VirtualAllocator(T&& other)
		: implementation(DefaultAllocator::Create(std::move(implementation)))
	{		
	}	
}