#pragma once
#include "BlazeEngine/Event/Events.h"

namespace Blaze
{
	template<typename T>
	class EventDispatcher;

	template<typename T>
	class EventHandler
	{
		bool supress = false;
		std::vector<T> events;
		EventDispatcher<T>* dispatcher;
	public:
		~EventHandler();

		void SupressEvents()
		{
			supress = true;
		}
		void ProcessSupressedEvents()
		{
			for (auto& event : events)
				OnEvent(event);
			events.clear();
		}

		void AddEvent(T event)
		{
			if (supress)
				events.emplace_back(event);
			else
				OnEvent(event);
		}

		virtual void OnEvent(T event) = 0;

		template <typename T>
		friend class EventDispatcher;
	};
}