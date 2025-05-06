#pragma once
#include "BlazeEngineCore/Types/TemplateGroup.h"

namespace Blaze
{
	namespace TypeTraitsImpl
	{
		template<typename T>
		struct MemberFunctionPointerParameters
		{
		};

		template<typename R, typename C, typename ... A>
		struct MemberFunctionPointerParameters<R(C::*)(A...)>
		{
			using ReturnType = R;
			using ClassType = C;
			using ArgumentTypes = TemplateGroup<A...>;
		};
	}
}