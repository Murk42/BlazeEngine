#pragma once
#include "BlazeEngine/Event/Events.h"
#include "BlazeEngine/Event/EventHandler.h"
#include "BlazeEngine/DataStructures/Array.h"
#include <utility>

namespace Blaze
{
	template<typename T>
	class EventHandler;

	template<typename T>
	class EventDispatcher
	{
		Array<EventHandler<T>*> handlers;		
	public:
		~EventDispatcher()
		{
			for (auto& handler : handlers)
				handler->dispatcher = nullptr;
		}
		
		Result AddHandler(EventHandler<T>& handler)
		{
			handlers.Resize(handlers.Count() + 1);
			handlers.Last() = &handler;
			handler.dispatcher = this;			

			return Result();
		}
		Result RemoveHandler(EventHandler<T>& handler)
		{
			auto it = std::find(handlers.begin(), handlers.end(), &handler);
			if (it != handlers.end())
			{
				(*it)->dispatcher = nullptr;
				*it = handlers.Last();
				handlers.Resize(handlers.Count() - 1);				
			}

			return Result();
		}

		Result Call(T event)
		{
			return ADD_STACK_FRAME(LoggerListener::Listen([&]() {

				for (auto& handler : handlers)
				{
					if (handler->listening)
						handler->OnEvent(event);

					if (handler->supress)
						break;
				}

				}));						
		}

		EventDispatcher& operator+=(EventHandler<T>& handler)
		{
			AddHandler(handler);
			return *this;
		}
		EventDispatcher& operator-=(EventHandler<T>& handler)
		{
			RemoveHandler(handler);
			return *this;
		}
	};

	//template<typename T>
	//EventHandler<T>::~EventHandler()
	//{
	//	//if (dispatcher)
	//	//	dispatcher->RemoveHandler(*this);
	//}		
}