#pragma once
#include "BlazeEngine/Event/EventHandler.h"

namespace Blaze
{
	template<typename T>
	class EventHandler;

	template<typename T>
	class BLAZE_API EventDispatcher
	{
	public:
		EventDispatcher();
		EventDispatcher(const EventDispatcher& other) = delete;
		EventDispatcher(EventDispatcher&& other) noexcept;
		~EventDispatcher();
		
		Result AddHandler(EventHandler<T>& handler);
		Result RemoveHandler(EventHandler<T>& handler);
		
		virtual Result Call(T event); 				

		EventDispatcher& operator=(const EventDispatcher& other) = delete;
		EventDispatcher& operator=(EventDispatcher&& other) noexcept;
	private:
		struct HandlerCallData
		{			
			DualList<EventHandler<T>*>::Iterator handlerIt;
			bool toBeDeleted;
		};
		std::mutex mutex;
		
		DualList<HandlerCallData> callsData;
		DualList<EventHandler<T>*> handlers;		

		void EraseHandlerFromCallData(EventHandler<T>* it);
	};

	class BLAZE_API std::mutex;

	template<typename T>
	inline EventDispatcher<T>::EventDispatcher()
	{

	}
	template<typename T>
	inline EventDispatcher<T>::EventDispatcher(EventDispatcher<T>&& other) noexcept		
		: callsData(std::move(other.callsData)), handlers(std::move(other.handlers))
	{						
		for (EventHandler<T>*& handler : handlers)					
			handler->dispatcher = this;		
	}
	template<typename T>
	inline EventDispatcher<T>::~EventDispatcher()
	{
		for (auto& handler : handlers)
		{
			handler->DispatcherDestroyed();
			handler->dispatcher = nullptr;
		}
	}

	template<typename T>
	inline Result EventDispatcher<T>::AddHandler(EventHandler<T>& handler)
	{
		std::lock_guard<std::mutex> lk { mutex };

		if (handler.dispatcher != this && handler.dispatcher != nullptr)
			return BLAZE_ERROR_RESULT("Blaze Engine", "Trying to add a evnet handler to a dispatcher but it is already subscribed to a dispatcher");

		handlers.AddBack(&handler);		
		return Result();
	}

	template<typename T>
	inline Result EventDispatcher<T>::RemoveHandler(EventHandler<T>& handler)
	{
		std::lock_guard<std::mutex> lk { mutex };

		for (HandlerCallData& callData : callsData)
			if (*callData.handlerIt == &handler)
			{				
				callData.toBeDeleted = true;
				return Result();
			}

		handlers.EraseOne([&](auto value) -> bool {
			if (value == &handler)
			{
				EraseHandlerFromCallData(value);
				return true;
			}
			return false;
			});

		return Result();
	}

	template<typename T>
	inline Result EventDispatcher<T>::Call(T event)
	{						
		std::unique_lock<std::mutex> lk{ mutex };
		
		auto callDataIt = callsData.AddBack();
		HandlerCallData& callData = *callDataIt;
		callData.handlerIt = handlers.FirstIterator();
		callData.toBeDeleted = false;

		while(!callData.handlerIt.IsNull())
		{						
			EventHandler<T>& handler = **callData.handlerIt;

			bool suppress = handler.suppress;

			if (handler.listening)
			{
				lk.unlock();
				handler.OnEvent(event);
				lk.lock();
			}			

			if (callData.toBeDeleted)
			{				
				auto it = callData.handlerIt;
				EraseHandlerFromCallData(*callData.handlerIt);				
				handlers.Erase(it);
				callData.toBeDeleted = false;
			}				
								
			if (suppress || callData.handlerIt.IsNull())
				break;

			++callData.handlerIt;			
		}

		callsData.Erase(callDataIt);

		return Result();
	}	

	template<typename T>
	inline EventDispatcher<T>& EventDispatcher<T>::operator=(EventDispatcher&& other) noexcept
	{
		std::unique_lock<std::mutex> lk{ mutex };

		handlers = std::move(other.handlers);
		callsData = std::move(other.callsData);

		for (EventHandler<T>*& handler : handlers)		
			handler->dispatcher = this;
		
		return *this;
	}

	template<typename T>
	inline void EventDispatcher<T>::EraseHandlerFromCallData(EventHandler<T>* handler)
	{		
		for (HandlerCallData& callData : callsData)
			if (*callData.handlerIt == handler)
				--callData.handlerIt;
	}
}