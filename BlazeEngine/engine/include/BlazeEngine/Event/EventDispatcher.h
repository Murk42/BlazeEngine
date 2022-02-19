#pragma once
#include "BlazeEngine/Event/Events.h"

namespace Blaze
{
	template<typename T>
	class EventHandler;

	template<typename T>
	class EventDispatcher
	{
		std::vector<EventHandler<T>*> handlers;
	public:
		void Call(T parameters)
		{
			for (auto& handler : handlers)
				handler->OnEvent(parameters);
			//include EventHandler if compiler throws an error here
		}

		void AddHandler(EventHandler<T>& handler)
		{
			handlers.emplace_back(&handler);
		}
		void RemoveHandler(EventHandler<T>& handler)
		{
			auto it = handlers.find(&handler);
			if (it != handlers.end())
				handlers.erase(it);
		}
	};
}