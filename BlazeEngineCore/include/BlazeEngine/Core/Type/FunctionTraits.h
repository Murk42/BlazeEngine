#pragma once
#include "BlazeEngine/Core/Type/FunctionTraitsImpl.h"

namespace Blaze
{
	template<typename MemberFunctionPointerType>
	using MemberFunctionPointerTypeReturnType = typename Internal::MemberFunctionPointerParameters<MemberFunctionPointerType>::template ReturnType;
	template<typename MemberFunctionPointerType>
	using MemberFunctionPointerTypeClassType = typename Internal::MemberFunctionPointerParameters<MemberFunctionPointerType>::template ClassType;
	template<typename MemberFunctionPointerType>
	using MemberFunctionPointerTypeArgumentTypes = typename Internal::MemberFunctionPointerParameters<MemberFunctionPointerType>::template ArgumentTypes;

	template<typename FunctionPointerType>
	using FunctionPointerTypeReturnType = typename Internal::FunctionParameters<FunctionPointerType>::template ReturnType;
	template<typename FunctionPointerType>
	using FunctionPointerTypeArgumentTypes = typename Internal::FunctionParameters<FunctionPointerType>::template ArgumentTypes;
}