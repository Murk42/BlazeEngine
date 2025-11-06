#pragma once
#include "BlazeEngine/Core/Event/EventHandlerData.h"
#include "BlazeEngine/Core/Container/Array.h"
#include "BlazeEngine/Core/Container/ArrayView.h"
#include "BlazeEngine/Core/Memory/Allocator.h"

namespace Blaze
{		
	template<typename T>
	class BLAZE_API EventDispatcherInterface
	{
	public:
		virtual void AddHandler(const EventHandlerData<T>& handlerData) = 0;
		virtual void RemoveHandler(const EventHandlerData<T>& handlerData) = 0;
		virtual void SwapHandlers(const EventHandlerData<T>& oldHandlerData, const EventHandlerData<T>& newHandlerData) = 0;
		virtual bool HasHandler(const EventHandlerData<T>& handlerData) = 0;		
	};
	
	template<typename T>
	class BLAZE_API EventDispatcherBase : public EventDispatcherInterface<T>
	{
	public:
		virtual ~EventDispatcherBase();

		virtual void Call(const T& event) const = 0;
	};

	template<typename T, AllocatorType Allocator = DefaultAllocator>
	class BLAZE_API EventDispatcher : public EventDispatcherBase<T>
	{
	public:
		using EventType = T;

		EventDispatcher();
		EventDispatcher(const EventDispatcher& other) = delete;
		EventDispatcher(EventDispatcher&& other) noexcept;
		~EventDispatcher();
		
		void AddHandler(const EventHandlerData<T>& handlerData) override;		
		void RemoveHandler(const EventHandlerData<T>& handlerData) override;
		void SwapHandlers(const EventHandlerData<T>& oldHandlerData, const EventHandlerData<T>& newHandlerData) override;
		bool HasHandler(const EventHandlerData<T>& handlerData) override;

		//This function is meant to be used in the following way:
		//eventDispatcher.AddHandler<&ClassA::MemberFunction>(instanceOfClassA);
		template<auto MemberFunctionPointer> requires SameAs<MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunctionPointer)>, TypeGroup<const T&>>
		void AddHandler(MemberFunctionPointerTypeClassType<decltype(MemberFunctionPointer)>& handlerObject);
		//This function is meant to be used in the following way:
		//eventDispatcher.RemoveHandler<&ClassA::MemberFunction>(instanceOfClassA);
		template<auto MemberFunctionPointer> requires SameAs<MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunctionPointer)>, TypeGroup<const T&>>
		void RemoveHandler(MemberFunctionPointerTypeClassType<decltype(MemberFunctionPointer)>& handlerObject);
		//This function is meant to be used in the following way:
		//eventDispatcher.SwapHandlers<&ClassA::MemberFunction>(instanceOfClassA, otherInstanceOfClassA);
		template<auto MemberFunctionPointer> requires SameAs<MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunctionPointer)>, TypeGroup<const T&>>
		void SwapHandlers(MemberFunctionPointerTypeClassType<decltype(MemberFunctionPointer)>& oldHandlerObject, MemberFunctionPointerTypeClassType<decltype(MemberFunctionPointer)>& newHandlerObject);

		void Call(const T& event) const override;

		ArrayView<EventHandlerData<T>> GetEventHandlerData() const;

		EventDispatcher& operator=(const EventDispatcher& other) = delete;
		EventDispatcher& operator=(EventDispatcher&& other) noexcept;
	private:			
		Array<EventHandlerData<T>, Allocator> handlers;
	};	
}

#include "BlazeEngine/Core/Event/EventDispatcherImpl.h"