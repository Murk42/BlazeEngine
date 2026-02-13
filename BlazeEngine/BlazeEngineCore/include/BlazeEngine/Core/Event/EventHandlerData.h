#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Type/FunctionTraits.h"
#include "BlazeEngine/Core/Event/EventHandler.h"

namespace Blaze
{
	template<typename T>
	class BLAZE_API EventHandlerData
	{
	public:
		constexpr EventHandlerData();
		constexpr EventHandlerData(const EventHandlerData& other);

		constexpr EventHandlerData(EventHandler<T>& handlerObject);
		constexpr EventHandlerData(void(*function)(const T&));
		constexpr EventHandlerData(void(*function)(const T&, void*), void* userData);
		template<auto MemberFunctionPointer> requires SameAs<MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunctionPointer)>, TypeGroup<const T&>>
		constexpr EventHandlerData(MemberFunctionPointerTypeClassType<decltype(MemberFunctionPointer)>& handlerObject);

		constexpr EventHandlerData& Set(EventHandler<T>& handlerObject);
		constexpr EventHandlerData& Set(void(*function)(const T&));
		constexpr EventHandlerData& Set(void(*function)(const T&, void*), void* userData);
		template<auto MemberFunctionPointer> requires SameAs<MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunctionPointer)>, TypeGroup<const T&>>
		constexpr EventHandlerData& Set(MemberFunctionPointerTypeClassType<decltype(MemberFunctionPointer)>& handlerObject);
		template<typename MemberFunctionPointer>
		constexpr EventHandlerData& Set(MemberFunctionPointer memberFunctionPointer, MemberFunctionPointerTypeClassType<MemberFunctionPointer>& handlerObject);

		constexpr void Clear();
		constexpr bool Empty() const;

		constexpr void Invoke(const T& event) const;

		constexpr bool operator==(const EventHandlerData& other) const;
		constexpr bool operator!=(const EventHandlerData& other) const;

		constexpr EventHandlerData& operator=(const EventHandlerData& other);
	private:
		static void StaticFunctionInvokingFunction(const T& event, void* function);
		template<auto MemberFunctionPointer> requires SameAs<MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunctionPointer)>, TypeGroup<const T&>>
		static void MemberFunctionInvokingFunction(const T& event, void* object);

		void(*invokingFunction)(const T&, void*);
		void* data;
	};
}

#include "BlazeEngine/Core/Event/EventHandlerDataImpl.h"