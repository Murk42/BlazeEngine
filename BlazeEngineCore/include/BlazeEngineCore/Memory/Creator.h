//#pragma once
//
//namespace Blaze
//{
//#ifdef BLAZE_COMPILER_MSVC
//#define BLAZE_CREATOR_ATTRIBUTE [[msvc::no_unique_address]]
//#else
//#define BLAZE_CREATOR_ATTRIBUTE [[no_unique_address]]
//#endif
//
//	template<AllocatorType Allocator = DefaultAllocator>
//	class BLAZE_CORE_API DefaultCreator
//	{
//	public:		
//		using AllocatorType = Allocator;		
//
//		BLAZE_ALLOCATOR_ATTRIBUTE inline static Allocator allocator;
//		
//		template<typename T, typename ... Args> requires std::constructible_from<T, Args...>
//		static T* Create(Args&& ... args);
//		template<typename T, typename ... Args> requires std::constructible_from<T, Args...>
//		static T* CreateArray(uintMem count, const Args& ... args);
//		template<typename T>
//		static void Destroy(T* ptr);
//		template<typename T>
//		static void DestroyArray(uintMem count, T* ptr);
//	};	
//
//	template<typename C, typename T, typename ... Args>
//	concept IsCreator = requires (const C& creator, uintMem count, Args&& ... args) {
//		{ creator.Create(std::forward<Args>(args)...) } -> std::same_as<T*>;
//		{ creator.CreateArray(count, args...) } -> std::same_as<T*>;
//		{ creator.Destroy(&creator) };
//		{ creator.DestroyArray(count, &creator) };
//	};
//
//	template<typename C, typename T, typename ... Args>
//	concept CreatorType = IsCreator<C, T, Args...>;
//
//	template<typename T, typename ... Args>
//	concept IsStaticCreator = !requires(const T & creator, Args&& ... args, uintMem count)
//	{
//		{ creator.Create(std::forward<Args>(args)...) } -> std::same_as<T*>;
//		{ creator.CreateArray(count, args...) } -> std::same_as<T*>;
//		{ creator.Destroy(&creator) };
//		{ creator.DestroyArray(count, &creator) };
//	} && IsCreator<T>;
//	template<typename T, typename ... Args>
//	concept IsDynamicCreator = requires(const T & creator, Args&& ... args)
//	{
//		{ creator.Create(std::forward<Args>(args)...) } -> std::same_as<T*>;
//		{ creator.CreateArray(count, args...) } -> std::same_as<T*>;
//		{ creator.Destroy(&creator) };
//		{ creator.DestroyArray(count, &creator) };
//	}&& IsCreator<T>;	
//
//	template<AllocatorType Allocator>
//	template<typename T, typename ...Args> requires std::constructible_from<T, Args...>
//	inline T* DefaultCreator<Allocator>::Create(Args && ...args)
//	{
//		T* ptr = (T*)allocator.Allocate(sizeof(T));
//		std::construct_at(ptr, std::forward<Args>(args)...);
//		return ptr;
//	}
//	template<AllocatorType Allocator>
//	template<typename T, typename ...Args> requires std::constructible_from<T, Args...>
//	inline T* DefaultCreator<Allocator>::CreateArray(uintMem count, const Args& ...args)
//	{
//		T* ptr = (T*)allocator.Allocate(sizeof(T) * count);
//
//		for (uintMem i = 0; i != count; ++i)
//			std::construct_at(ptr + i, args...);
//
//		return ptr;
//	}
//	template<AllocatorType Allocator>
//	template<typename T>
//	inline void DefaultCreator<Allocator>::Destroy(T* ptr)
//	{
//		std::destroy_at(ptr);
//		allocator.Free(ptr);
//	}
//	template<AllocatorType Allocator>
//	template<typename T>
//	inline void DefaultCreator<Allocator>::DestroyArray(uintMem count, T* ptr)
//	{
//		std::destroy_n(ptr, count);
//		allocator.Free(ptr);
//	}
//}