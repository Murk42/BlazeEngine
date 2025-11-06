#pragma once
#include "BlazeEngine/Core/Type/TypeGroup.h"

namespace Blaze::Internal
{
	template<typename T>
	struct MemberFunctionPointerParameters;

	template<typename R, typename C, typename ... A>
	struct MemberFunctionPointerParameters<R(C::*)(A...)>
	{
		using ReturnType = R;
		using ClassType = C;
		using ArgumentTypes = TypeGroup<A...>;
	};
}