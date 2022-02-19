#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Event/EventDispatcher.h"
#include "BlazeEngine/Event/Events.h"

namespace Blaze
{
	namespace EventManager
	{
		template<typename T>
		void AddHandler(EventHandler<T>& handler);
				
		template<> BLAZE_API void AddHandler<Event::KeyPressed		 >(EventHandler<Event::KeyPressed		>& handler);
		template<> BLAZE_API void AddHandler<Event::KeyReleased		 >(EventHandler<Event::KeyReleased		>& handler);
		template<> BLAZE_API void AddHandler<Event::MousePressed	 >(EventHandler<Event::MousePressed		>& handler);	
		template<> BLAZE_API void AddHandler<Event::MouseReleased	 >(EventHandler<Event::MouseReleased	>& handler);
		template<> BLAZE_API void AddHandler<Event::MouseMotion		 >(EventHandler<Event::MouseMotion		>& handler);
		template<> BLAZE_API void AddHandler<Event::MouseScroll		 >(EventHandler<Event::MouseScroll		>& handler);
		template<> BLAZE_API void AddHandler<Event::MouseEnter		 >(EventHandler<Event::MouseEnter		>& handler);
		template<> BLAZE_API void AddHandler<Event::MouseLeave		 >(EventHandler<Event::MouseLeave		>& handler);
		template<> BLAZE_API void AddHandler<Event::WindowResized	 >(EventHandler<Event::WindowResized	>& handler);
		template<> BLAZE_API void AddHandler<Event::WindowMoved		 >(EventHandler<Event::WindowMoved		>& handler);
		template<> BLAZE_API void AddHandler<Event::WindowMinimized	 >(EventHandler<Event::WindowMinimized	>& handler);
		template<> BLAZE_API void AddHandler<Event::WindowMaximized	 >(EventHandler<Event::WindowMaximized	>& handler);
		template<> BLAZE_API void AddHandler<Event::WindowFocusGained>(EventHandler<Event::WindowFocusGained>& handler);
		template<> BLAZE_API void AddHandler<Event::WindowFocusLost	 >(EventHandler<Event::WindowFocusLost	>& handler);
		template<> BLAZE_API void AddHandler<Event::WindowClosed	 >(EventHandler<Event::WindowClosed		>& handler);	
		template<> BLAZE_API void AddHandler<Event::TextInput		 >(EventHandler<Event::TextInput		>& handler);
		template<> BLAZE_API void AddHandler<Event::ViewportChanged	 >(EventHandler<Event::ViewportChanged	>& handler);
	}
}