#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"
#include "BlazeEngineCore/Types/TypeTraits.h"
#include "BlazeEngineCore/Event/EventHandler.h"

namespace Blaze
{	
	template<typename T>
	class BLAZE_CORE_API EventHandlerData
	{
	public:
		constexpr EventHandlerData();
		constexpr EventHandlerData(const EventHandlerData& other);

		constexpr EventHandlerData(EventHandler<T>& handlerObject);
		constexpr EventHandlerData(void(*function)(const T&));
		constexpr EventHandlerData(void(*function)(const T&, void*), void* userData);
		template<auto MemberFunctionPointer> requires std::same_as<MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunctionPointer)>, TemplateGroup<const T&>>
		constexpr EventHandlerData(MemberFunctionPointerTypeClassType<decltype(MemberFunctionPointer)>& handlerObject);		

		constexpr void Set(EventHandler<T>& handlerObject);
		constexpr void Set(void(*function)(const T&));
		constexpr void Set(void(*function)(const T&, void*), void* userData);
		template<auto MemberFunctionPointer> requires std::same_as<MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunctionPointer)>, TemplateGroup<const T&>>
		constexpr void Set(MemberFunctionPointerTypeClassType<decltype(MemberFunctionPointer)>& handlerObject);		

		constexpr void Clear();
		constexpr bool Empty() const;

		constexpr void Invoke(const T& event) const;

		constexpr bool operator==(const EventHandlerData& other) const;
		constexpr bool operator!=(const EventHandlerData& other) const;

		constexpr EventHandlerData& operator=(const EventHandlerData& other);
	private:		
		static void StaticFunctionInvokingFunction(const T& event, void* function);
		template<auto MemberFunctionPointer> requires std::same_as<MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunctionPointer)>, TemplateGroup<const T&>>
		static void MemberFunctionInvokingFunction(const T& event, void* object);

		void(*invokingFunction)(const T&, void*);
		void* data;
	};	
}