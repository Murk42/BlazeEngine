#pragma once
#include "BlazeEngineCore/Event/EventHandlerData.h"
#include "BlazeEngineCore/DataStructures/Array.h"
#include "BlazeEngineCore/DataStructures/ArrayView.h"
#include "BlazeEngineCore/Memory/Allocator.h"

namespace Blaze
{		
	template<typename T>
	class BLAZE_CORE_API EventDispatcherInterface
	{
	public:
		virtual void AddHandler(const EventHandlerData<T>& handlerData) = 0;
		virtual void RemoveHandler(const EventHandlerData<T>& handlerData) = 0;
		virtual void SwapHandlers(const EventHandlerData<T>& oldHandlerData, const EventHandlerData<T>& newHandlerData) = 0;
		virtual bool HasHandler(const EventHandlerData<T>& handlerData) = 0;		
	};
	
	template<typename T>
	class BLAZE_CORE_API EventDispatcherBase : public EventDispatcherInterface<T>
	{
	public:
		virtual void Call(const T& event) const = 0;
	};

	template<typename T, AllocatorType Allocator = DefaultAllocator>
	class BLAZE_CORE_API EventDispatcher : public EventDispatcherBase<T>
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
		template<auto MemberFunctionPointer> requires std::same_as<MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunctionPointer)>, TemplateGroup<const T&>>
		void AddHandler(MemberFunctionPointerTypeClassType<decltype(MemberFunctionPointer)>& handlerObject);
		//This function is meant to be used in the following way:
		//eventDispatcher.RemoveHandler<&ClassA::MemberFunction>(instanceOfClassA);
		template<auto MemberFunctionPointer> requires std::same_as<MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunctionPointer)>, TemplateGroup<const T&>>
		void RemoveHandler(MemberFunctionPointerTypeClassType<decltype(MemberFunctionPointer)>& handlerObject);
		//This function is meant to be used in the following way:
		//eventDispatcher.SwapHandlers<&ClassA::MemberFunction>(instanceOfClassA, otherInstanceOfClassA);
		template<auto MemberFunctionPointer> requires std::same_as<MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunctionPointer)>, TemplateGroup<const T&>>
		void SwapHandlers(MemberFunctionPointerTypeClassType<decltype(MemberFunctionPointer)>& oldHandlerObject, MemberFunctionPointerTypeClassType<decltype(MemberFunctionPointer)>& newHandlerObject);

		void Call(const T& event) const override;

		ArrayView<EventHandlerData<T>> GetEventHandlerData() const;

		EventDispatcher& operator=(const EventDispatcher& other) = delete;
		EventDispatcher& operator=(EventDispatcher&& other) noexcept;
	private:		
		Array<EventHandlerData<T>, Allocator> handlers;		
	};	
};