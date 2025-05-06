#pragma once

namespace Blaze
{
	/*
	template<typename T, AllocatorType Allocator>
	inline Handle<T, Allocator>::Handle()
		: ptr(nullptr)
	{
	}
	template<typename T, AllocatorType Allocator>
	inline Handle<T, Allocator>::Handle(T* ptr)
		: ptr(ptr)
	{
	}
	template<typename T, AllocatorType Allocator>	
	inline Handle<T, Allocator>::Handle(Handle&& other) noexcept
		: allocator(std::move(other.allocator))
	{
		ptr = other.ptr;
		other.ptr = nullptr;
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
		allocator.Free(ptr);		
	}
	template<typename T, AllocatorType Allocator>
	inline T* Handle<T, Allocator>::Ptr() const
	{
		return ptr;
	}
	template<typename T, AllocatorType Allocator>
	inline T* Handle<T, Allocator>::operator->() const
	{
		if (ptr == nullptr)
		{
			BLAZE_ENGINE_CORE_FATAL("Dereferencing a pointer that is nullptr");
			return nullptr;
		}

		return ptr;
	}
	template<typename T, AllocatorType Allocator>
	inline T& Handle<T, Allocator>::operator*() const
	{
		if (ptr == nullptr)
		{
			BLAZE_ENGINE_CORE_FATAL("Dereferencing a pointer that is nullptr");
			return *(T*)nullptr;
		}

		return *ptr;
	}

	template<typename T, AllocatorType Allocator>
	inline Handle<T, Allocator>::operator T* () const
	{
		return ptr;
	}	

	template<typename T, AllocatorType Allocator>
	inline bool Handle<T, Allocator>::operator==(T* ptr) const
	{
		return ptr == this->ptr;
	}

	template<typename T, AllocatorType Allocator>
	inline bool Handle<T, Allocator>::operator!=(T* ptr) const
	{
		return ptr != this->ptr;
	}

	template<typename T, AllocatorType Allocator>
	inline T& Handle<T, Allocator>::operator[](uintMem index)
	{
		if (ptr == nullptr)
		{
			BLAZE_ENGINE_CORE_FATAL("Dereferencing a pointer that is nullptr");
			return *(T*)nullptr;
		}

		return ptr[index];
	}

	template<typename T, AllocatorType Allocator>
	inline const T& Handle<T, Allocator>::operator[](uintMem index) const
	{
		if (ptr == nullptr)
		{
			BLAZE_ENGINE_CORE_FATAL("Dereferencing a pointer that is nullptr");
			return *(T*)nullptr;
		}
		return ptr[index];
	}

	template<typename T, AllocatorType Allocator>	
	inline Handle<T, Allocator>& Handle<T, Allocator>::operator=(Handle&& other) noexcept
	{
		Clear();

		ptr = other.ptr;
		allocator.operator=(std::move(other.allocator));
		other.ptr = nullptr;

		return *this;
	}

	template<typename T, AllocatorType Allocator>
	template<typename ...Args> requires std::constructible_from<T, Args...>
	inline Handle<T, Allocator> Handle<T, Allocator>::CreateNew(Args&& ... args)
	{
		Handle handle;
		handle.ptr = (T*)handle.allocator.New(sizeof(T));
		std::construct_at(handle.ptr, std::forward<Args>(args)...);
		return handle;				
	}
	*/
}