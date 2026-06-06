#pragma once
#include <BlazeEngine/Core/Math/Vector.h>

namespace Blaze
{
	namespace Internal
	{
		template<typename T>
		struct IsVectorType
		{
			static constexpr bool value = false;
		};
		template<typename T, uintMem Size>
		struct IsVectorType<Vector<T, Size>>
		{
			static constexpr bool value = true;
		};
	}
	
	template<typename T>
	concept VectorType = Internal::IsVectorType<T>::value;
}