#include "pch.h"
#include "BlazeEngine/Input/InputEvents.h"
#include "BlazeEngine/Input/Input.h"
#include "BlazeEngine/Input/ClientEventStack.h"
#include "BlazeEngine/Internal/WindowCreation.h"

#include "SDL2/SDL.h"

namespace Blaze
{	
	namespace Input
	{
		extern InputEventSystem inputEventSystem;

		Window* GetWindowFromSDLHandle(SDL_Window* window)
		{
			return (Window*)SDL_GetWindowData(window, "Blaze");
		}
	}

	ThreadRequestHandler<WindowCreationData, WindowHandle> windowCreationQueue;
	ThreadRequestHandler<WindowDestructionData, void> windowDestructionQueue;

	Key GetKeyFromScancode(SDL_Scancode);		

	static int ResizingEventWatcher(void* data, SDL_Event* event) {

		if (event->type == SDL_WINDOWEVENT &&
			event->window.event == SDL_WINDOWEVENT_RESIZED) {

			SDL_Window* win = SDL_GetWindowFromID(event->window.windowID);

			Window* window = Input::GetWindowFromSDLHandle(win);
			
			if (window != nullptr)
			{				
				Input::Events::WindowResizedEvent _event;
				_event.window = window;
				_event.size = Vec2i(event->window.data1, event->window.data2);
				Input::clientEventStack.Add(_event, &Input::inputEventSystem.windowResizedDispatcher);
				Input::clientEventStack.Add(_event, &window->resizedEventDispatcher);				
			}
		}
		return 0;
	}

	TimingResult InitializeInput()
	{
		Timing timing{ "Input" };

		SDL_AddEventWatch(ResizingEventWatcher, nullptr);
		return timing.GetTimingResult();
	}
	void TerminateInput()
	{

	}

	namespace Input
	{						
		void HandleEvents()
		{			
			windowCreationQueue.CheckRequests([&](const WindowCreationData& data) {
				uint32 flags = 0;

				switch (data.graphicsAPI)
				{
				case WindowHandleGraphicsaAPI::Metal:
					flags = SDL_WINDOW_METAL;
					break;
				case WindowHandleGraphicsaAPI::OpenGL:
					flags = SDL_WINDOW_OPENGL;
					break;
				case WindowHandleGraphicsaAPI::Vulkan:
					flags = SDL_WINDOW_VULKAN;
					break;
				default:
					break;
				}
				Vec2i pos = data.pos;
								
				if (pos.x == INT_MAX)
					pos.x = SDL_WINDOWPOS_CENTERED;
				if (pos.y == INT_MAX)
					pos.y = SDL_WINDOWPOS_CENTERED;
				return (WindowHandle)SDL_CreateWindow(data.name.Ptr(), pos.x, pos.y, data.size.x, data.size.y, SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE | flags);
				});
			windowDestructionQueue.CheckRequests([&](const WindowDestructionData& data) {
				SDL_DestroyWindow((SDL_Window*)data.handle);
				});
			
			SDL_Event event;
			while (SDL_PollEvent(&event))
				switch (event.type)
				{
				case SDL_MOUSEWHEEL: {
					Events::MouseScroll _event;
					_event.value = event.wheel.y;
					clientEventStack.Add(_event, &inputEventSystem.mouseScrollDispatcher);
					break;
				}
				case SDL_MOUSEMOTION: {
					Events::MouseMotion  _event;
					_event.delta = { event.motion.xrel, event.motion.yrel };
					_event.pos = { event.motion.x, event.motion.y };
					break;
				}
				case SDL_MOUSEBUTTONDOWN: {
					Key key = (Key)(event.button.button - 1 + (uint8)Key::MouseLeft);

					Events::KeyPressed _event;
					_event.key = key;
					_event.combo = 0;
					_event.time = 0;
					clientEventStack.Add(_event, &inputEventSystem.keyPressedDispatcher);
					break;
				}
				case SDL_MOUSEBUTTONUP: {
					Key key = (Key)(event.button.button - 1 + (uint8)Key::MouseLeft);

					Events::KeyReleased _event;
					_event.key = key;
					_event.time = 0;
					clientEventStack.Add(_event, &inputEventSystem.keyReleasedDispatcher);

					break;
				}
				case SDL_KEYDOWN: {
					Key key = GetKeyFromScancode(event.key.keysym.scancode);

					Events::KeyPressed _event;
					_event.key = key;
					_event.combo = 0;
					_event.time = 0;
					clientEventStack.Add(_event, &inputEventSystem.keyPressedDispatcher);
					break;

					break;
				}
				case SDL_KEYUP: {
					Key key = GetKeyFromScancode(event.key.keysym.scancode);

					Events::KeyReleased _event;
					_event.key = key;
					_event.time = 0;
					clientEventStack.Add(_event, &inputEventSystem.keyReleasedDispatcher);
					break;
				}
				case SDL_TEXTINPUT: {
					Events::TextInput _event;
					_event.input = StringUTF8((void*)event.text.text, strlen(event.text.text));
					clientEventStack.Add(_event, &inputEventSystem.textInputDispatcher);
					break;
				}
				case SDL_WINDOWEVENT: {					
						switch (event.window.event) {
						case SDL_WINDOWEVENT_MOVED: {
							Events::WindowMovedEvent _event;
							Window* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event.window.windowID));
							_event.window = window;
							_event.pos = Vec2i(event.window.data1, event.window.data2);
							clientEventStack.Add(_event, &inputEventSystem.windowMovedDispatcher);
							if (window != nullptr)
								clientEventStack.Add(_event, &_event.window->movedEventDispatcher);
							break;
						}
						case SDL_WINDOWEVENT_MINIMIZED: {
							Events::WindowMinimizedEvent _event;
							Window* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event.window.windowID));
							_event.window = window;
							clientEventStack.Add(_event, &inputEventSystem.windowMinimizedDispatcher);
							if (window != nullptr)
								clientEventStack.Add(_event, &_event.window->minimizedEventDispatcher);
							break;
						}
						case SDL_WINDOWEVENT_MAXIMIZED: {
							Events::WindowMaximizedEvent _event;
							Window* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event.window.windowID));
							_event.window = window;
							clientEventStack.Add(_event, &inputEventSystem.windowMaximizedDispatcher);
							if (window != nullptr)
								clientEventStack.Add(_event, &_event.window->maximizedEventDispatcher);
							break;
						}
						case SDL_WINDOWEVENT_SIZE_CHANGED: {
							//Covered by event callback
							/*
							Events::WindowResizedEvent _event;
							_event.window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event.window.windowID));
							_event.size = Vec2i(event.window.data1, event.window.data2);
							clientEventStack.Add(_event, &inputEventSystem.windowResizedDispatcher);
							break;
							*/
						}
						case SDL_WINDOWEVENT_RESIZED: {
							/*
							Covered by SDL_WINDOWEVENT_SIZE_CHANGED
							*/
							break;
						}
						case SDL_WINDOWEVENT_FOCUS_GAINED: {
							Events::WindowFocusGainedEvent _event;
							Window* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event.window.windowID));
							_event.window = window;
							clientEventStack.Add(_event, &inputEventSystem.windowFocusGainedDispatcher);
							if (window != nullptr)
								clientEventStack.Add(_event, &_event.window->focusGainedEventDispatcher);
							break;
						}
						case SDL_WINDOWEVENT_FOCUS_LOST: {
							Events::WindowFocusLostEvent _event;
							Window* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event.window.windowID));
							_event.window = window;
							clientEventStack.Add(_event, &inputEventSystem.windowFocusLostDispatcher);
							if (window != nullptr)
								clientEventStack.Add(_event, &_event.window->focusLostEventDispatcher);
							break;
						}
						case SDL_WINDOWEVENT_CLOSE: {
							Events::WindowCloseEvent _event;
							Window* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event.window.windowID));
							_event.window = window;
							clientEventStack.Add(_event, &inputEventSystem.windowCloseDispatcher);
							if (window != nullptr)
								clientEventStack.Add(_event, &_event.window->closeEventDispatcher);
							break;
						}
						case SDL_WINDOWEVENT_ENTER: {
							Events::WindowMouseEnterEvent _event;
							Window* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event.window.windowID));
							_event.window = window;
							clientEventStack.Add(_event, &inputEventSystem.windowMouseEnterDispatcher);
							if (window != nullptr)
								clientEventStack.Add(_event, &_event.window->mouseEnterDispatcher);
							break;
						}
						case SDL_WINDOWEVENT_LEAVE: {
							Events::WindowMouseLeaveEvent _event;
							Window* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event.window.windowID));
							_event.window = window;
							clientEventStack.Add(_event, &inputEventSystem.windowMouseLeaveDispatcher);
							if (window != nullptr)
								clientEventStack.Add(_event, &_event.window->mouseLeaveDispatcher);
							break;
						}
						}
				}
				}			
		}
	}
}