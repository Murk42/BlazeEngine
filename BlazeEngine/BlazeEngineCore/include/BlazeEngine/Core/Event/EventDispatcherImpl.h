#pragma once
#include "EventDispatcher.h"
#include "BlazeEngine/Core/Debug/Logger.h"

namespace Blaze
{
	template<typename T>
	inline EventDispatcherBase<T>::~EventDispatcherBase()
	{
	}

	template<typename T, AllocatorType Allocator>
	inline EventDispatcher<T, Allocator>::EventDispatcher()
	{
	}
	template<typename T, AllocatorType Allocator>
	inline EventDispatcher<T, Allocator>::EventDispatcher(EventDispatcher&& other) noexcept
		: handlers(std::move(other.handlers))
	{
	}
	template<typename T, AllocatorType Allocator>
	inline EventDispatcher<T, Allocator>::~EventDispatcher()
	{
	}
	template<typename T, AllocatorType Allocator>
	inline void EventDispatcher<T, Allocator>::AddHandler(const EventHandlerData<T>& handlerData)
	{
		if (handlerData.Empty())
		{
			BLAZE_LOG_ERROR("Trying to add empty handler data");
			return;
		}

		handlers.AddBack(handlerData);
	}
	template<typename T, AllocatorType Allocator>
	inline void EventDispatcher<T, Allocator>::RemoveHandler(const EventHandlerData<T>& handlerData)
	{
		for (uintMem i = 0; i < handlers.Count(); ++i)
			if (handlers[i] == handlerData)
			{
				handlers.EraseAt(i);
				return;
			}
	}
	template<typename T, AllocatorType Allocator>
	inline void EventDispatcher<T, Allocator>::SwapHandlers(const EventHandlerData<T>& oldHandlerData, const EventHandlerData<T>& newHandlerData)
	{
		for (uintMem i = 0; i < handlers.Count(); ++i)
			if (handlers[i] == oldHandlerData)
			{
				handlers[i] = newHandlerData;
				return;
			}
	}
	template<typename T, AllocatorType Allocator>
	inline bool EventDispatcher<T, Allocator>::HasHandler(const EventHandlerData<T>& handlerData)
	{
		for (auto& handler : handlers)
			if (handler == handlerData)
				return true;

		return false;
	}
	template<typename T, AllocatorType Allocator>
	template<auto MemberFunctionPointer> requires SameAs<MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunctionPointer)>, TypeGroup<const T&>>
	inline void EventDispatcher<T, Allocator>::AddHandler(MemberFunctionPointerTypeClassType<decltype(MemberFunctionPointer)>& handlerObject)
	{
		EventHandlerData<T> data;
		data.Set<MemberFunctionPointer>(handlerObject);
		AddHandler(data);
	}
	template<typename T, AllocatorType Allocator>
	template<auto MemberFunctionPointer> requires SameAs<MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunctionPointer)>, TypeGroup<const T&>>
	inline void EventDispatcher<T, Allocator>::RemoveHandler(MemberFunctionPointerTypeClassType<decltype(MemberFunctionPointer)>& handlerObject)
	{
		EventHandlerData<T> data;
		data.Set<MemberFunctionPointer>(handlerObject);
		RemoveHandler(data);
	}
	template<typename T, AllocatorType Allocator>
	template<auto MemberFunctionPointer> requires SameAs<MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunctionPointer)>, TypeGroup<const T&>>
	inline void EventDispatcher<T, Allocator>::SwapHandlers(MemberFunctionPointerTypeClassType<decltype(MemberFunctionPointer)>& oldHandlerObject, MemberFunctionPointerTypeClassType<decltype(MemberFunctionPointer)>& newHandlerObject)
	{
		EventHandlerData<T> oldData, newData;
		oldData.Set<MemberFunctionPointer>(oldHandlerObject);
		newData.Set<MemberFunctionPointer>(newHandlerObject);
		SwapHandlers(oldData, newData);
	}
	template<typename T, AllocatorType Allocator>
	inline void EventDispatcher<T, Allocator>::Call(const T& event) const
	{
		for (auto& handler : handlers)
		{
			try
			{
				handler.Invoke(event);
			}
			catch (const std::exception& ex)
			{
				BLAZE_LOG_ERROR("An exception was thrown while handling an event: \"{}\"", ex.what());
			}
			catch (...)
			{
				BLAZE_LOG_ERROR("An unknown exception was thrown while handling an event.");
			}
		}
	}
	template<typename T, AllocatorType Allocator>
	inline ArrayView<EventHandlerData<T>> EventDispatcher<T, Allocator>::GetEventHandlerData() const
	{
		return handlers;
	}
	template<typename T, AllocatorType Allocator>
	inline EventDispatcher<T, Allocator>& EventDispatcher<T, Allocator>::operator=(EventDispatcher&& other) noexcept
	{
		handlers = std::move(other.handlers);

		return *this;
	}
}