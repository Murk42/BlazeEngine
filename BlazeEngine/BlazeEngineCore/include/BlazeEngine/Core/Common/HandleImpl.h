#pragma once

namespace Blaze
{
	template<typename T, AllocatorType Allocator>
	inline Handle<T, Allocator>::Handle()
		: ptr(nullptr), allocator()
	{
	}
	template<typename T, AllocatorType Allocator>
	inline Handle<T, Allocator>::Handle(Handle&& other) noexcept
		: ptr(std::exchange(other.ptr, nullptr)), allocator(std::move(other.allocator))
	{
	}
	template<typename T, AllocatorType Allocator>
	inline Handle<T, Allocator>::~Handle()
	{
		Clear();
	}
	template<typename T, AllocatorType Allocator>
	inline bool Handle<T, Allocator>::Empty() const
	{
		return ptr == nullptr;
	}
	template<typename T, AllocatorType Allocator>
	inline void Handle<T, Allocator>::Clear()
	{
		if (ptr)
		{
			ptr->~T();
			allocator.Free(ptr);
			ptr = nullptr;
		}
	}
	template<typename T, AllocatorType Allocator>
	inline T* Handle<T, Allocator>::Ptr()
	{
		return ptr;
	}
	template<typename T, AllocatorType Allocator>
	inline const T* Handle<T, Allocator>::Ptr() const
	{
		return ptr;
	}
	template<typename T, AllocatorType Allocator>
	inline Handle<T, Allocator>::operator T* ()
	{
		return ptr;
	}
	template<typename T, AllocatorType Allocator>
	inline Handle<T, Allocator>::operator const T* () const
	{
		return ptr;
	}
	template<typename T, AllocatorType Allocator>
	inline T* Handle<T, Allocator>::operator->() const
	{
		if (ptr == nullptr)
			BLAZE_LOG_FATAL("Trying to access empty handle");

		return ptr;
	}
	template<typename T, AllocatorType Allocator>
	inline T& Handle<T, Allocator>::operator*() const
	{
		if (ptr == nullptr)
			BLAZE_LOG_FATAL("Trying to access empty handle");

		return *ptr;
	}
	template<typename T, AllocatorType Allocator>
	inline bool Handle<T, Allocator>::operator==(const Handle& other) const
	{
		return ptr == other.ptr;
	}
	template<typename T, AllocatorType Allocator>
	inline bool Handle<T, Allocator>::operator!=(const Handle& other) const
	{
		return ptr != other.ptr;
	}
	template<typename T, AllocatorType Allocator>
	inline auto Handle<T, Allocator>::operator=(Handle&& other) noexcept -> Handle&
	{
		ptr = std::exchange(other.ptr, nullptr);
		allocator = std::move(other.allocator);
		return *this;
	}
	template<typename T, AllocatorType Allocator>
	template<typename ...Args> requires IsConstructibleFrom<T, Args...>
	inline auto Handle<T, Allocator>::Create(Args && ...args) -> Handle
	{
		Handle<T, Allocator> handle;
		handle.ptr = static_cast<T*>(handle.allocator.Allocate(sizeof(T)));
		std::construct_at(handle.ptr, std::forward<Args>(args)...);
		return handle;
	}
	template<typename T, AllocatorType Allocator>
	template<IsDerivedFrom<T> D, typename ... Args> requires HasVirtualDestructor<D> && IsConstructibleFrom<D, Args...>
	inline auto Handle<T, Allocator>::CreateDerived(Args&& ... args) -> Handle
	{
		Handle<T, Allocator> handle;

		D* ptr = static_cast<D*>(handle.allocator.Allocate(sizeof(D)));
		std::construct_at(ptr, std::forward<Args>(args)...);

		handle.ptr = static_cast<T*>(ptr);
		return handle;
	}
}