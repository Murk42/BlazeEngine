#include "pch.h"
#include "BlazeEngine/Input/Input.h"
#include "BlazeEngine/Internal/GlobalData.h"
#include "BlazeEngine/Window/WindowSDL.h"
#include "SDL2/SDL_events.h"

namespace Blaze
{
	namespace Input
	{
		int SDLEventWatcher(void* data, SDL_Event* event);
	}

	TimingResult InitializeInput()
	{
		Timing timing{ "Input" };

		SDL_AddEventWatch(Input::SDLEventWatcher, nullptr);		

		return timing.GetTimingResult();
	}
	void TerminateInput()
	{
		for (auto& cursor : globalData->cursors)
			if (cursor != nullptr)
				SDL_FreeCursor(cursor);
	}

	namespace Input
	{											
		InputEventSystem& GetInputEventSystem()
		{
			return globalData->inputEventSystem;
		}

		void Update()
		{			
			globalData->inputEventSystem.inputPreUpdateDispatcher.Call({ });

			globalData->inputEventStack.ProcessAndClear();

			globalData->inputEventSystem.inputPostUpdateDispatcher.Call({ });
		}				
		KeyState GetLastKeyState(Key key)
		{
			if ((uint)key > _countof(globalData->keyStates))
				return { };

			std::lock_guard<std::mutex> lk{ globalData->keyStatesMutex };
			return globalData->keyStates[(uint)key];
		}

		void SetCursorType(CursorType type)
		{
			auto& cursor = globalData->cursors[(uint)type];
			
			if (cursor == nullptr)
				cursor = SDL_CreateSystemCursor((SDL_SystemCursor)type);

			SDL_SetCursor(cursor);			
		}

		WindowSDL* GetWindowFromSDLHandle(SDL_Window* window)
		{
			return (WindowSDL*)SDL_GetWindowData(window, "Blaze");
		}

		void KeyPressedEvent(Key key, WindowSDL* window)
		{
			if ((uint)key > _countof(globalData->keyStates))
				return;

			Events::KeyPressed _event;

			{
				std::lock_guard<std::mutex> lk{ globalData->keyStatesMutex };
				auto& state = globalData->keyStates[(uint)key];

				double time = TimePoint::GetRunTime();

				if (state.down)				
					state.pressed = false;
				else
				{
					state.pressed = true;
					state.down = true;
					state.up = false;
					state.released = false;					
				}

				if (time - state.time <= globalData->keyComboTime)
					++state.combo;
				else
					state.combo = 1;

				state.time = time;

				_event.key = key;
				_event.combo = state.combo;
				_event.time = time;
			}

			if (window != nullptr)
				globalData->inputEventStack.Add(_event, &window->keyPressedDispatcher);
		}
		void KeyReleasedEvent(Key key, WindowSDL* window)
		{
			if ((uint)key > _countof(globalData->keyStates))
				return;

			Events::KeyReleased _event;

			{
				std::lock_guard<std::mutex> lk{ globalData->keyStatesMutex };
				auto& state = globalData->keyStates[(uint)key];				
				
				if (state.up)
				{
					state.released = false;					
				}
				else
				{
					state.pressed = false;
					state.down = false;
					state.up = true;
					state.released = true;					
				}				

				_event.key = key;				
				_event.time = state.time;
			}

			if (window != nullptr)
				globalData->inputEventStack.Add(_event, &window->keyReleasedDispatcher);
		}

		int SDLEventWatcher(void* data, SDL_Event* event)
		{
			if (event->type == globalData->mainThreadTaskEventIdentifier)
				return 0;
			if (event->type == globalData->clientThreadExitEventIdentifier)
			{
				globalData->clientThreadExited.test_and_set();
				return 0;
			}

			switch (event->type)
			{
			case SDL_MOUSEWHEEL: {
				Events::MouseScroll _event;
				_event.value = event->wheel.y;

				WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));

				if (window != nullptr)
					globalData->inputEventStack.Add(_event, &window->mouseScrollDispatcher);

				break;
			}
			case SDL_MOUSEMOTION: {
				Events::MouseMotion  _event;
				_event.delta = { event->motion.xrel, -event->motion.yrel };

				WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));

				if (window != nullptr)
					globalData->inputEventStack.Add(_event, &window->mouseMotionDispatcher);
				break;
			}
			case SDL_MOUSEBUTTONDOWN: {				
				Key key = (Key)(event->button.button - 1 + (uint8)Key::MouseLeft);

				KeyPressedEvent(key, GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID)));

				break;
			}
			case SDL_MOUSEBUTTONUP: {
				Key key = (Key)(event->button.button - 1 + (uint8)Key::MouseLeft);

				KeyReleasedEvent(key, GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID)));

				break;
			}
			case SDL_KEYDOWN: {
				auto it = globalData->keymap.Find(event->key.keysym.scancode);

				if (it.IsNull())
					break;

				KeyPressedEvent(it->value, GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID)));								
				break;
			}
			case SDL_KEYUP: {
				auto it = globalData->keymap.Find(event->key.keysym.scancode);

				if (it.IsNull())
					break;

				KeyReleasedEvent(it->value, GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID)));				
				break;
			}
			case SDL_TEXTINPUT: {
				Events::TextInput _event;
				_event.input = StringUTF8((void*)event->text.text, strlen(event->text.text));

				WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));

				if (window != nullptr)
					globalData->inputEventStack.Add(_event, &window->textInputDispatcher);
				break;
			}
			case SDL_WINDOWEVENT: {
				switch (event->window.event) {
				case SDL_WINDOWEVENT_MOVED: {
					Events::WindowMovedEvent _event;
					WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));
					_event.window = window;
					_event.pos = Vec2i(event->window.data1, event->window.data2);
					globalData->inputEventStack.Add(_event, &globalData->inputEventSystem.windowMovedDispatcher);

					if (window != nullptr)
						globalData->inputEventStack.Add(_event, &_event.window->movedEventDispatcher);
					break;
				}
				case SDL_WINDOWEVENT_MINIMIZED: {
					Events::WindowMinimizedEvent _event;
					WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));
					_event.window = window;
					globalData->inputEventStack.Add(_event, &globalData->inputEventSystem.windowMinimizedDispatcher);

					if (window != nullptr)
						globalData->inputEventStack.Add(_event, &_event.window->minimizedEventDispatcher);
					break;
				}
				case SDL_WINDOWEVENT_MAXIMIZED: {
					Events::WindowMaximizedEvent _event;
					WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));
					_event.window = window;
					globalData->inputEventStack.Add(_event, &globalData->inputEventSystem.windowMaximizedDispatcher);

					if (window != nullptr)
						globalData->inputEventStack.Add(_event, &_event.window->maximizedEventDispatcher);
					break;
				}											  
				case SDL_WINDOWEVENT_RESIZED: {
					globalData->windowSwappingSkipFlag.test_and_set();
					
					Input::Events::WindowResizedEvent _event;

					WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));
					_event.window = window;
					_event.size = Vec2u(event->window.data1, event->window.data2);

					globalData->inputEventStack.Add(_event, &globalData->inputEventSystem.windowResizedDispatcher);

					if (window != nullptr)
						globalData->inputEventStack.Add(_event, &window->resizedEventDispatcher);

					globalData->inputEventStack.WaitUntilEmpty();

					globalData->windowSwappingSkipFlag.clear();

					//if (globalData->windowSwappingWaitingMutex.try_lock())
					//	globalData->windowSwappingWaitingMutex.unlock();
					//else
					//{
					//	globalData->windowSwappingPossibleFlag.test_and_set();
					//	globalData->windowSwappingPossibleFlag.notify_all();						
					//
					//	globalData->windowSwappingPossibleFlag.wait(1);
					//}
					
					break;
				}
				case SDL_WINDOWEVENT_FOCUS_GAINED: {
					Events::WindowFocusGainedEvent _event;

					WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));
					_event.window = window;

					globalData->inputEventStack.Add(_event, &globalData->inputEventSystem.windowFocusGainedDispatcher);

					if (window != nullptr)
						globalData->inputEventStack.Add(_event, &_event.window->focusGainedEventDispatcher);

					break;
				}
				case SDL_WINDOWEVENT_FOCUS_LOST: {
					Events::WindowFocusLostEvent _event;
					WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));
					_event.window = window;
					globalData->inputEventStack.Add(_event, &globalData->inputEventSystem.windowFocusLostDispatcher);

					if (window != nullptr)
						globalData->inputEventStack.Add(_event, &_event.window->focusLostEventDispatcher);
					break;
				}
				case SDL_WINDOWEVENT_CLOSE: {
					Events::WindowCloseEvent _event;
					WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));
					_event.window = window;
					globalData->inputEventStack.Add(_event, &globalData->inputEventSystem.windowCloseDispatcher);

					if (window != nullptr)
						globalData->inputEventStack.Add(_event, &_event.window->closeEventDispatcher);
					break;
				}
				case SDL_WINDOWEVENT_ENTER: {
					Events::WindowMouseEnterEvent _event;
					WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));
					_event.window = window;
					globalData->inputEventStack.Add(_event, &globalData->inputEventSystem.windowMouseEnterDispatcher);

					if (window != nullptr)
						globalData->inputEventStack.Add(_event, &_event.window->mouseEnterDispatcher);
					break;
				}
				case SDL_WINDOWEVENT_LEAVE: {
					Events::WindowMouseLeaveEvent _event;
					WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));
					_event.window = window;
					globalData->inputEventStack.Add(_event, &globalData->inputEventSystem.windowMouseLeaveDispatcher);

					if (window != nullptr)
						globalData->inputEventStack.Add(_event, &_event.window->mouseLeaveDispatcher);
					break;
				}
				}
			}
			}

			return 0;
		}
	}
}