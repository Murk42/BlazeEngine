#pragma once
#include <type_traits>
#include <concepts>
#include "BlazeEngineCore/Types/TypeTraitsImpl.h"

namespace Blaze
{	
	template<typename MemberFunctionPointerType>
	using MemberFunctionPointerTypeReturnType = typename TypeTraitsImpl::MemberFunctionPointerParameters<MemberFunctionPointerType>::template ReturnType;	
	template<typename MemberFunctionPointerType>
	using MemberFunctionPointerTypeClassType = typename TypeTraitsImpl::MemberFunctionPointerParameters<MemberFunctionPointerType>::template ClassType;
	template<typename MemberFunctionPointerType>
	using MemberFunctionPointerTypeArgumentTypes = typename TypeTraitsImpl::MemberFunctionPointerParameters<MemberFunctionPointerType>::template ArgumentTypes;
}