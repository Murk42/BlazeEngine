#pragma once

namespace Blaze
{			
	//template<typename T, AllocatorType Allocator = DefaultAllocator>  
	//class Handle
	//{
	//public:
	//	using ValueType = T;
	//	using AllocatorType = Allocator;
	//	
	//	Handle();
	//	/*
	//		Creates a handle from a pointer which must be pointing to valid memory allocated using the same allocator as the template argument allocator type
	//
	//		\param ptr    pointer to memory that must be pointing to valid memory allocated using the same allocator as the template argument allocator type
	//	*/
	//	Handle(T* ptr);						
	//	Handle(Handle&& other) noexcept;
	//	~Handle();
	//
	//	bool Empty() const;
	//	void Clear();
	//
	//	T* Ptr() const;
	//	
	//	T* operator->() const;
	//	T& operator*() const;
	//
	//	operator T* () const;
	//
	//	bool operator==(T* ptr) const;		
	//	bool operator!=(T* ptr) const;
	//	
	//	T& operator[](uintMem index);
	//	const T& operator[](uintMem index) const;
	//
	//	Handle& operator=(Handle&& other) noexcept;
	//
	//	template<typename ... Args> requires IsConstructibleFrom<T, Args...>
	//	static Handle CreateNew(Args&& ... args);
	//
	//	template<typename, ::Blaze::AllocatorType>
	//	friend class ::Blaze::Handle;
	//private:
	//	T* ptr;
	//	BLAZE_ALLOCATOR_ATTRIBUTE Allocator allocator;
	//};			
}