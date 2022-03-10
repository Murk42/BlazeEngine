#pragma once
#include "BlazeEngine/Event/Events.h"
#include "BlazeEngine/Event/EventHandler.h"

namespace Blaze
{
	template<typename T>
	class EventHandler;

	template<typename T>
	class EventDispatcher
	{
		std::vector<EventHandler<T>*> handlers;

	public:
		~EventDispatcher()
		{
			for (auto& handler : handlers)
				handler->dispatcher = nullptr;
		}
		
		void AddHandler(EventHandler<T>& handler)
		{
			handler.dispatcher = this;
			handlers.emplace_back(&handler);
		}
		void RemoveHandler(EventHandler<T>& handler)
		{
			auto it = std::find(handlers.begin(), handlers.end(), &handler);
			if (it != handlers.end())
			{
				(* it)->dispatcher = nullptr;
				handlers.erase(it);
			}
		}

		void Call(T parameters)
		{
			for (auto& handler : handlers)
				handler->AddEvent(parameters);
			//include EventHandler if compiler throws an error here
		}
	};

	template<typename T>
	inline EventHandler<T>::~EventHandler()
	{		
		if (dispatcher)
			dispatcher->RemoveHandler(*this);			
	}		
}