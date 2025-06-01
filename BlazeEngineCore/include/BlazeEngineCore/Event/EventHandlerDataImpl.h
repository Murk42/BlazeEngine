#pragma once
#include "BlazeEngineCore/Event/EventHandlerData.h"
#include "BlazeEngineCore/Debug/Logger.h"


namespace Blaze
{
	template<typename T>
	inline constexpr EventHandlerData<T>::EventHandlerData()
		: ptr(nullptr), invokingFunction(nullptr)
	{
	}
	template<typename T>
	inline constexpr EventHandlerData<T>::EventHandlerData(const EventHandlerData& other)
		: ptr(other.ptr), invokingFunction(other.invokingFunction)
	{
	}
	template<typename T>
	inline constexpr EventHandlerData<T>::EventHandlerData(EventHandler<T>& handlerObject)
	{
		Set(handlerObject);
	}
	template<typename T>
	inline constexpr EventHandlerData<T>::EventHandlerData(void(*function)(const T&))
	{
		Set(function);
	}
	template<typename T>
	template<auto MemberFunctionPointer> requires std::same_as<MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunctionPointer)>, TemplateGroup<const T&>>
	inline constexpr EventHandlerData<T>::EventHandlerData(MemberFunctionPointerTypeClassType<decltype(MemberFunctionPointer)>& handlerObject)
	{
		Set(handlerObject);
	}
	template<typename T>
	inline constexpr void EventHandlerData<T>::Set(EventHandler<T>& handlerObject)
	{
		invokingFunction = MemberFunctionInvokingFunction<&EventHandler<T>::OnEvent>;
		ptr = &handlerObject;
	}
	template<typename T>
	inline constexpr void EventHandlerData<T>::Set(void(*function)(const T&))
	{				
		if (function == nullptr)
		{
			BLAZE_ENGINE_CORE_ERROR("Trying to set a static function but its pointer is nullptr");
			return;
		}

		invokingFunction = StaticFunctionInvokingFunction;
		ptr = function;
	}
	template<typename T>
	template<auto MemberFunctionPointer> requires std::same_as<MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunctionPointer)>, TemplateGroup<const T&>>
	inline constexpr void EventHandlerData<T>::Set(MemberFunctionPointerTypeClassType<decltype(MemberFunctionPointer)>& handlerObject)
	{
		invokingFunction = MemberFunctionInvokingFunction<MemberFunctionPointer>;
		ptr = &handlerObject;
	}	
	template<typename T>
	inline void EventHandlerData<T>::Clear()
	{
		invokingFunction = nullptr;
		ptr = nullptr;
	}
	template<typename T>
	inline bool EventHandlerData<T>::IsEmpty() const
	{
		return ptr == nullptr;
	}
	template<typename T>
	inline void EventHandlerData<T>::Invoke(const T& event) const
	{
		if (ptr == nullptr)
		{
			BLAZE_ENGINE_CORE_ERROR("Trying to invoke a event handler but the ptr is nullptr");
			return;
		}

		invokingFunction(ptr, event);
	}
	template<typename T>
	inline constexpr bool EventHandlerData<T>::operator==(const EventHandlerData& other) const
	{
		return ptr == other.ptr && invokingFunction == other.invokingFunction;
	}
	template<typename T>
	inline constexpr bool EventHandlerData<T>::operator!=(const EventHandlerData& other) const
	{
		return ptr != other.ptr || invokingFunction != other.invokingFunction;
	}
	template<typename T>
	inline constexpr EventHandlerData<T>& EventHandlerData<T>::operator=(const EventHandlerData& other)
	{
		ptr = other.ptr;
		invokingFunction = other.invokingFunction;
		return *this;
	}	
	template<typename T>
	inline void EventHandlerData<T>::StaticFunctionInvokingFunction(void* function, const T& event)
	{
		((void(*)(const T&))function)(event);
	}
	template<typename T>
	template<auto MemberFunctionPointer> requires std::same_as<MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunctionPointer)>, TemplateGroup<const T&>>
	inline void EventHandlerData<T>::MemberFunctionInvokingFunction(void* object, const T& event)
	{		
		using Class = MemberFunctionPointerTypeClassType<decltype(MemberFunctionPointer)>;
		((*(Class*)object).*MemberFunctionPointer)(event);
	}	
}