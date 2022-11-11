#pragma once
#include "BlazeEngine/Event/Events.h"
#include "BlazeEngine/DataStructures/List.h"

namespace Blaze
{
	template<typename T>
	class EventDispatcher;

	template<typename T>
	class EventHandler
	{
		bool supress = false;
		List<T> events;
		EventDispatcher<T>* dispatcher = nullptr;
	public:
		EventHandler()
		{
		}
		~EventHandler()
		{

		}

		void SupressEvents()
		{
			supress = true;
		}
		void ProcessSupressedEvents()
		{
			for (auto& event : events)
				OnEvent(event);
			events.Clear();
		}

		void AddEvent(T event)
		{
			if (supress)
				events.AddBack(event);
			else
				OnEvent(event);
		}

		virtual void OnEvent(T event) = 0;

		template <typename T>
		friend class EventDispatcher;
	};
}