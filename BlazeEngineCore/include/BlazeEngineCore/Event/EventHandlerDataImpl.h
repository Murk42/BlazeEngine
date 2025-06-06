#pragma once
#include "BlazeEngineCore/Event/EventHandlerData.h"
#include "BlazeEngineCore/Debug/Logger.h"


namespace Blaze
{
	template<typename T>
	inline constexpr EventHandlerData<T>::EventHandlerData()
		: data(nullptr), invokingFunction(nullptr)
	{
	}
	template<typename T>
	inline constexpr EventHandlerData<T>::EventHandlerData(const EventHandlerData& other)
		: data(other.data), invokingFunction(other.invokingFunction)
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
	inline constexpr EventHandlerData<T>::EventHandlerData(void(*function)(const T&, void*), void* userData)
	{
		Set(function, userData);
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
		data = &handlerObject;
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
		data = function;
	}
	template<typename T>
	inline constexpr void EventHandlerData<T>::Set(void(*function)(const T&, void*), void* userData)
	{
		if (function == nullptr)
		{
			BLAZE_ENGINE_CORE_ERROR("Trying to set a static function but its pointer is nullptr");
			return;
		}

		invokingFunction = function;
		data = userData;
	}
	template<typename T>
	template<auto MemberFunctionPointer> requires std::same_as<MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunctionPointer)>, TemplateGroup<const T&>>
	inline constexpr void EventHandlerData<T>::Set(MemberFunctionPointerTypeClassType<decltype(MemberFunctionPointer)>& handlerObject)
	{
		invokingFunction = MemberFunctionInvokingFunction<MemberFunctionPointer>;
		data = &handlerObject;
	}	
	template<typename T>
	inline constexpr void EventHandlerData<T>::Clear()
	{
		invokingFunction = nullptr;
		data = nullptr;
	}
	template<typename T>
	inline constexpr bool EventHandlerData<T>::Empty() const
	{
		return data == nullptr;
	}
	template<typename T>
	inline constexpr void EventHandlerData<T>::Invoke(const T& event) const
	{
		if (invokingFunction != nullptr)
			invokingFunction(event, data);
	}
	template<typename T>
	inline constexpr bool EventHandlerData<T>::operator==(const EventHandlerData& other) const
	{
		return data == other.data && invokingFunction == other.invokingFunction;
	}
	template<typename T>
	inline constexpr bool EventHandlerData<T>::operator!=(const EventHandlerData& other) const
	{
		return data != other.data || invokingFunction != other.invokingFunction;
	}
	template<typename T>
	inline constexpr EventHandlerData<T>& EventHandlerData<T>::operator=(const EventHandlerData& other)
	{
		data = other.data;
		invokingFunction = other.invokingFunction;
		return *this;
	}
	template<typename T>
	inline void EventHandlerData<T>::StaticFunctionInvokingFunction(const T& event, void* function)
	{
		((void(*)(const T&))function)(event);
	}	
	template<typename T>
	template<auto MemberFunctionPointer> requires std::same_as<MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunctionPointer)>, TemplateGroup<const T&>>
	inline void EventHandlerData<T>::MemberFunctionInvokingFunction(const T& event, void* object)
	{		
		using Class = MemberFunctionPointerTypeClassType<decltype(MemberFunctionPointer)>;
		((*(Class*)object).*MemberFunctionPointer)(event);
	}	
}