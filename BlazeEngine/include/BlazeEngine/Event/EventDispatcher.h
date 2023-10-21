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
		std::mutex mutex;
		Array<EventHandler<T>*> handlers;		
	};

	class BLAZE_API std::mutex;

	template<typename T>
	inline EventDispatcher<T>::EventDispatcher()
	{

	}
	template<typename T>
	inline EventDispatcher<T>::EventDispatcher(EventDispatcher&& other) noexcept
		: handlers(std::move(other.handlers))
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

	template<typename T>
	inline EventDispatcher<T>& EventDispatcher<T>::operator=(EventDispatcher&& other) noexcept
	{
		handlers = std::move(other.handlers);
		for (EventHandler<T>*& handler : handlers)		
			handler->dispatcher = this;
		
		return *this;
	}
}