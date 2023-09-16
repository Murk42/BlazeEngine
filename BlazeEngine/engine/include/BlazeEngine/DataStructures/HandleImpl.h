#pragma once

namespace Blaze
{
	template<typename T, AllocatorType Allocator>
	inline Handle<T, Allocator>::Handle()
		: ptr(nullptr)
	{
	}
	template<typename T, AllocatorType Allocator>
	template<typename T2> requires IsConvertibleToOtherHandleType<T, T2>
	inline Handle<T, Allocator>::Handle(Handle<T2, Allocator>&& other) noexcept
		: allocator(std::move(other.allocator))
	{
		ptr = (T*)other.ptr;
		other.ptr = nullptr;
	}
	template<typename T, AllocatorType Allocator>
	inline Handle<T, Allocator>::~Handle()
	{
		Clear();		
	}
	template<typename T, AllocatorType Allocator>
	inline void Handle<T, Allocator>::Clear()
	{
		allocator.Destroy(ptr);		
	}
	template<typename T, AllocatorType Allocator>
	inline T* Handle<T, Allocator>::Ptr() const
	{
		return ptr;
	}
	template<typename T, AllocatorType Allocator>
	inline T* Handle<T, Allocator>::operator->() const
	{
		return ptr;
	}
	template<typename T, AllocatorType Allocator>
	inline T& Handle<T, Allocator>::operator*() const
	{
		return *ptr;
	}

	template<typename T, AllocatorType Allocator>
	inline Handle<T, Allocator>::operator T* () const
	{
		return ptr;
	}	

	template<typename T, AllocatorType Allocator>
	template<typename T2> requires IsConvertibleToOtherHandleType<T, T2>
	inline Handle<T, Allocator>& Handle<T, Allocator>::operator=(Handle<T2, Allocator>&& other) noexcept
	{
		Clear();

		ptr = (T*)other.ptr;
		allocator.operator=(std::move(other.allocator));
		other.ptr = nullptr;

		return *this;
	}

	template<typename T, AllocatorType Allocator>
	template<typename ...Args>requires std::constructible_from<T, Args...>
	inline Handle<T, Allocator> Handle<T, Allocator>::CreateNew(Args&& ... args)
	{
		Handle handle;
		handle.ptr = handle.allocator.Create<T, Args...>(std::forward<Args>(args)...);
		return handle;				
	}
}