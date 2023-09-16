#pragma once
#include "BlazeEngine/Event/EventHandler.h"

namespace Blaze
{
	template<typename T>
	class EventHandler;

	template<typename T>
	class EventDispatcher
	{
	public:
		~EventDispatcher();
		
		Result AddHandler(EventHandler<T>& handler);
		Result RemoveHandler(EventHandler<T>& handler);
		
		virtual Result Call(T event); 				
	private:
		std::mutex mutex;
		Array<EventHandler<T>*> handlers;		
	};

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
			return BLAZE_ERROR_RESULT("Blaze Engine", "Trying to add a evnet handler to a dispatcher but it is already subsribed to a dispatcher");

		handlers.Resize(handlers.Count() + 1);
		handlers.Last() = &handler;
		handler.dispatcher = this;

		return Result();
	}

	template<typename T>
	inline Result EventDispatcher<T>::RemoveHandler(EventHandler<T>& handler)
	{
		std::lock_guard<std::mutex> lk { mutex };

		auto it = std::find(begin(handlers), end(handlers), &handler);
		if (!it.IsNull())
		{
			(*it)->dispatcher = nullptr;
			*it = handlers.Last();
			handlers.Resize(handlers.Count() - 1);
		}

		return Result();
	}

	template<typename T>
	inline Result EventDispatcher<T>::Call(T event)
	{
		std::lock_guard<std::mutex> lk { mutex };

		Debug::LoggerListener listener;

		for (auto& handler : handlers)
		{
			if (handler->listening)
				handler->OnEvent(event);

			if (handler->supress)
				break;
		}

		return listener.GetResult();
	}	
}