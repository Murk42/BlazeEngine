#pragma once

#include "BlazeEngine/Event/EventHandler.h"
#include "BlazeEngine/Event/EventDispatcher.h"
#include "BlazeEngine/Event/Events.h"

namespace Blaze
{
	namespace EventManager
	{
		template<typename T> 
		void AddEventHandler(Blaze::EventHandler<T>&);
						
		template<> BLAZE_API void AddEventHandler(Blaze::EventHandler<Event::KeyPressed>& handler);
		template<> BLAZE_API void AddEventHandler(Blaze::EventHandler<Event::KeyReleased>& handler);
		template<> BLAZE_API void AddEventHandler(Blaze::EventHandler<Event::MousePressed>& handler);
		template<> BLAZE_API void AddEventHandler(Blaze::EventHandler<Event::MouseReleased>& handler);
		template<> BLAZE_API void AddEventHandler(Blaze::EventHandler<Event::MouseMotion>& handler);
		template<> BLAZE_API void AddEventHandler(Blaze::EventHandler<Event::MouseScroll>& handler);
		template<> BLAZE_API void AddEventHandler(Blaze::EventHandler<Event::MouseEnter>& handler);
		template<> BLAZE_API void AddEventHandler(Blaze::EventHandler<Event::MouseLeave>& handler);
		template<> BLAZE_API void AddEventHandler(Blaze::EventHandler<Event::WindowResized>& handler);
		template<> BLAZE_API void AddEventHandler(Blaze::EventHandler<Event::WindowMoved>& handler);
		template<> BLAZE_API void AddEventHandler(Blaze::EventHandler<Event::WindowMinimized>& handler);
		template<> BLAZE_API void AddEventHandler(Blaze::EventHandler<Event::WindowMaximized>& handler);
		template<> BLAZE_API void AddEventHandler(Blaze::EventHandler<Event::WindowFocusGained>& handler);
		template<> BLAZE_API void AddEventHandler(Blaze::EventHandler<Event::WindowFocusLost>& handler);
		template<> BLAZE_API void AddEventHandler(Blaze::EventHandler<Event::WindowClosed>& handler);
		template<> BLAZE_API void AddEventHandler(Blaze::EventHandler<Event::TextInput>& handler);
		template<> BLAZE_API void AddEventHandler(Blaze::EventHandler<Event::ViewportChanged>& handler);

		template<typename T>
		void RemoveEventHandler(Blaze::EventHandler<T>&);

		template<> BLAZE_API void RemoveEventHandler(Blaze::EventHandler<Event::KeyPressed>& handler);
		template<> BLAZE_API void RemoveEventHandler(Blaze::EventHandler<Event::KeyReleased>& handler);
		template<> BLAZE_API void RemoveEventHandler(Blaze::EventHandler<Event::MousePressed>& handler);
		template<> BLAZE_API void RemoveEventHandler(Blaze::EventHandler<Event::MouseReleased>& handler);
		template<> BLAZE_API void RemoveEventHandler(Blaze::EventHandler<Event::MouseMotion>& handler);
		template<> BLAZE_API void RemoveEventHandler(Blaze::EventHandler<Event::MouseScroll>& handler);
		template<> BLAZE_API void RemoveEventHandler(Blaze::EventHandler<Event::MouseEnter>& handler);
		template<> BLAZE_API void RemoveEventHandler(Blaze::EventHandler<Event::MouseLeave>& handler);
		template<> BLAZE_API void RemoveEventHandler(Blaze::EventHandler<Event::WindowResized>& handler);
		template<> BLAZE_API void RemoveEventHandler(Blaze::EventHandler<Event::WindowMoved>& handler);
		template<> BLAZE_API void RemoveEventHandler(Blaze::EventHandler<Event::WindowMinimized>& handler);
		template<> BLAZE_API void RemoveEventHandler(Blaze::EventHandler<Event::WindowMaximized>& handler);
		template<> BLAZE_API void RemoveEventHandler(Blaze::EventHandler<Event::WindowFocusGained>& handler);
		template<> BLAZE_API void RemoveEventHandler(Blaze::EventHandler<Event::WindowFocusLost>& handler);
		template<> BLAZE_API void RemoveEventHandler(Blaze::EventHandler<Event::WindowClosed>& handler);
		template<> BLAZE_API void RemoveEventHandler(Blaze::EventHandler<Event::TextInput>& handler);
		template<> BLAZE_API void RemoveEventHandler(Blaze::EventHandler<Event::ViewportChanged>& handler);
	}
}