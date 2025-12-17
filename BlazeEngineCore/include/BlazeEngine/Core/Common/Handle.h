#pragma once
#include "BlazeEngine/Core/Debug/Logger.h"

namespace Blaze
{			
	template<typename T, AllocatorType Allocator = DefaultAllocator>  
	class Handle
	{
	public:
		using ValueType = T;
		using AllocatorType = Allocator;
		
		Handle();
		Handle(Handle&& other) noexcept;
		~Handle();
	
		bool Empty() const;
		void Clear();

		T* Ptr();
		const T* Ptr() const;

		operator T* ();
		operator const T* () const;
		
		T* operator->() const;
		T& operator*() const;
	
		bool operator==(const Handle& other) const;		
		bool operator!=(const Handle& other) const;
	
		Handle& operator=(Handle&& other) noexcept; 
	
		template<typename ... Args> requires IsConstructibleFrom<T, Args...>
		static Handle Create(Args&& ... args);
		template<IsDerivedFrom<T> D, typename ... Args> requires HasVirtualDestructor<D> && IsConstructibleFrom<D, Args...>
		static Handle CreateDerived(Args&& ... args);
	private:
		T* ptr;
		BLAZE_ALLOCATOR_ATTRIBUTE Allocator allocator;
	};			
}

#include "BlazeEngine/Core/Common/HandleImpl.h"