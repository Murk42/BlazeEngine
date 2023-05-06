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
		bool listening = true;		
		EventDispatcher<T>* dispatcher = nullptr;
	public:
		EventHandler()
		{
		}
		EventHandler(bool supress, bool listening)
			: supress(supress), listening(listening)
		{

		}
		~EventHandler()
		{
			if (dispatcher != nullptr)
				dispatcher->RemoveHandler(*this);
		}

		void StartSupressingEvents()
		{
			supress = true;
		}
		void StopSupressingEvents()
		{
			supress = false;
		}
		void StartListeningForEvents()
		{
			listening = true;
		}
		void StopListeningForEvents()
		{
			listening = false;
		}				

		bool IsSupressing() const { return supress; };
		bool IsListening() const { return listening; }

		virtual void OnEvent(T event) = 0;

		template <typename T>
		friend class EventDispatcher;
	};
}