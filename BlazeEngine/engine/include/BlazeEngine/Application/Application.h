#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include <functional>

#include <iostream>

namespace Blaze
{
	struct BlazeApplicationClassInfo
	{
		void(*Constructor)(void*);
		void(*Destructor)(void*);
		void(*Setup)(void*);
		size_t size = 0;		
	};			

	BLAZE_API void SetBlazeApplicationClassInfo(BlazeApplicationClassInfo sceneInfo);		

	BLAZE_API void Stop();

	template<typename T>
	class BlazeApplication
	{
		static void Constructor(void* ptr) { new (ptr) T(); }
		static void Destructor(void* ptr) { ((T*)ptr)->~T(); }
		static void _Setup(void* ptr) 
		{
			using SetupType = decltype(&T::Setup);
			static constexpr bool SetupDefined = !std::is_same_v<SetupType, int*	>;
			static_assert(SetupDefined, "Setup function must be defined");
			if constexpr(SetupDefined)
				((T*)ptr)->Setup(); 
		}		

		struct Init {
			Init()
			{
				std::cout << "A";
				using SetupType = decltype(&T::Setup);
				static constexpr bool SetupDefined = !std::is_same_v<SetupType, int*>;
				static_assert(SetupDefined, "Setup function must be defined");
				if constexpr (SetupDefined)
					SetBlazeApplicationClassInfo({ Constructor, Destructor, _Setup, sizeof(T) });
			}
		} static init;	

		int Setup;		
	public:			
	};	

	template<typename T>
	typename BlazeApplication<T>::Init init;
}