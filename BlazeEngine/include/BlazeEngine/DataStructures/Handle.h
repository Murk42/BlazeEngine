#pragma once

namespace Blaze
{
	template<typename T, typename T2>
	concept IsConvertibleToOtherHandleType = std::derived_from<T, T2> || std::derived_from<T2, T>;
	
	template<typename, AllocatorType>
	class Handle;

	template<typename T, AllocatorType Allocator = DefaultAllocator>  
	class Handle
	{
	public:
		using ValueType = T;
		using AllocatorType = Allocator;
		
		Handle();
		//This is dangerous because we dont know how the pointer was allocated
		//Handle(T* ptr);				
		template<typename T2> requires IsConvertibleToOtherHandleType<T, T2>
		Handle(Handle<T2, Allocator>&& other) noexcept;
		~Handle();

		void Clear();

		T* Ptr() const;
		
		T* operator->() const;
		T& operator*() const;

		operator T* () const;

		template<typename T2> requires IsConvertibleToOtherHandleType<T, T2>
		Handle& operator=(Handle<T2, Allocator>&& other) noexcept;

		template<typename ... Args> requires std::constructible_from<T, Args...>
		static Handle CreateNew(Args&& ... args);

		template<typename, ::Blaze::AllocatorType>
		friend class ::Blaze::Handle;
	private:
		T* ptr;
		BLAZE_ALLOCATOR_ATTRIBUTE Allocator allocator;
	};			
}