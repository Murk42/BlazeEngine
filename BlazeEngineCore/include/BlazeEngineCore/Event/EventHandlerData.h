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
		template<auto MemberFunctionPointer> requires std::same_as<MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunctionPointer)>, TemplateGroup<const T&>>
		constexpr EventHandlerData(MemberFunctionPointerTypeClassType<decltype(MemberFunctionPointer)>& handlerObject);		

		constexpr void Set(EventHandler<T>& handlerObject);
		constexpr void Set(void(*function)(const T&));
		template<auto MemberFunctionPointer> requires std::same_as<MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunctionPointer)>, TemplateGroup<const T&>>
		constexpr void Set(MemberFunctionPointerTypeClassType<decltype(MemberFunctionPointer)>& handlerObject);		

		inline void Clear();
		inline bool IsEmpty() const;

		inline void Invoke(const T& event) const;

		constexpr bool operator==(const EventHandlerData& other) const;
		constexpr bool operator!=(const EventHandlerData& other) const;

		constexpr EventHandlerData& operator=(const EventHandlerData& other);
	private:		
		static void StaticFunctionInvokingFunction(void* function, const T& event);
		template<auto MemberFunctionPointer> requires std::same_as<MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunctionPointer)>, TemplateGroup<const T&>>
		static void MemberFunctionInvokingFunction(void* object, const T& event);		

		void(*invokingFunction)(void*, const T&);
		void* ptr;
	};	
}