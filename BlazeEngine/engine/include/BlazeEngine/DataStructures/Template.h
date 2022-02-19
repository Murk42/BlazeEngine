#pragma once
#include "BlazeEngine/DataStructures/Common.h"

namespace Blaze
{
	template<typename T>
	class Template
	{		
	public:
		using Type = T;
	private:
		template<typename>
		struct _Same
		{
			static constexpr bool value = false;
		};
		template<>
		struct _Same<T>
		{
			static constexpr bool value = true;
		};
	public:
		
		template<typename T2>
		static constexpr bool Same = _Same<T2>::value;
	};
}