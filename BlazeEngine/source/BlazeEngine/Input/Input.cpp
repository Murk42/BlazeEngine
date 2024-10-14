#include "pch.h"
#include "BlazeEngine/Input/Input.h"
#include "BlazeEngine/Internal/GlobalData.h"
#include "BlazeEngine/Window/WindowSDL.h"
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_clipboard.h"

namespace Blaze
{
	namespace Input
	{
		void WindowResizedEventFunction(SDL_Event* event);
		int SDLEventWatcher(void* data, SDL_Event* event);
	}

	TimingResult InitializeInput()
	{
		Timing timing{ "Input" };

		SDL_AddEventWatch(Input::SDLEventWatcher, nullptr);	

		SDL_DisplayMode displayMode;
		SDL_GetDesktopDisplayMode(0, &displayMode);
		blazeEngineContext.desktopHeight = displayMode.h;

		return timing.GetTimingResult();
	}
	void TerminateInput()
	{
		SDL_DelEventWatch(Input::SDLEventWatcher, nullptr);

		for (auto& cursor : blazeEngineContext.cursors)
			if (cursor != nullptr)
				SDL_FreeCursor(cursor);
	}

	namespace Input
	{
		InputEventSystem& GetInputEventSystem()
		{
			return blazeEngineContext.inputEventSystem;
		}
		void Update()
		{						
			blazeEngineContext.inputEventSystem.inputPreUpdateDispatcher.Call({ });

			{
				std::lock_guard<std::mutex> lk{ blazeEngineContext.windowsMutex };
				for (auto window : blazeEngineContext.windows)
					window->ResetPressedAndReleasedKeys();
			}

			SDL_GetGlobalMouseState(&blazeEngineContext.desktopMousePos.x, &blazeEngineContext.desktopMousePos.y);
			blazeEngineContext.desktopMousePos.y = blazeEngineContext.desktopHeight - blazeEngineContext.desktopMousePos.y - 1;			
			SDL_GetRelativeMouseState(&blazeEngineContext.desktopMouseMovement.x, &blazeEngineContext.desktopMouseMovement.y);
			blazeEngineContext.desktopMouseMovement.y *= -1;

			blazeEngineContext.eventStack.ProcessAndClear();

			blazeEngineContext.inputEventSystem.inputPostUpdateDispatcher.Call({ });
		}

		Vec2i GetDesktopMousePos()
		{
			return blazeEngineContext.desktopMousePos;
		}

		Vec2i GetDesktopMouseMovement()
		{
			return blazeEngineContext.desktopMouseMovement;
		}

		void SetCursorType(CursorType type)
		{
			auto& cursor = blazeEngineContext.cursors[(uint)type];
			
			if (cursor == nullptr)
				cursor = SDL_CreateSystemCursor((SDL_SystemCursor)type);

			SDL_SetCursor(cursor);			
		}

		bool HasClipboardText()
		{			
			return SDL_HasClipboardText();			
		}

		StringUTF8 GetClipboardText()
		{
			char* text = SDL_GetClipboardText();
			uintMem bufferSize = strlen(text);

			StringUTF8 out{ (void*)text, bufferSize };

			SDL_free(text);
			return out;
		}

		void SetClipboardText(StringViewUTF8 text)
		{
			SDL_SetClipboardText((char*)text.Buffer());
		}

		WindowSDL* GetWindowFromSDLHandle(SDL_Window* window)
		{
			return (WindowSDL*)SDL_GetWindowData(window, "Blaze");
		}		

		template<typename T>
		inline void AddEventToInputEventStack(T&& event, EventDispatcher<T>* dispatcher, WindowSDL* window)
		{
			const char* name = typeid(T).name();
			uintMem length = strlen(name);
			blazeEngineContext.eventStack.Add<T>(StringView(name, length), [dispatcher](T event) {
				dispatcher->Call(event);
				}, std::forward<T>(event));
		}				

		int SDLEventWatcher(void* data, SDL_Event* event)
		{			
			if (event->type == blazeEngineContext.mainThreadTaskEventIdentifier)
			{
				//We return here because the SDLEventWatcher can be run on a another thread and the task needs to be finished on the main thread
				return 0;
			}
			if (event->type == blazeEngineContext.clientThreadExitEventIdentifier)
			{
				blazeEngineContext.clientThreadExited.test_and_set();
				return 0;
			}

			switch (event->type)
			{
			case SDL_MOUSEWHEEL: {
				Events::MouseScroll _event;
				_event.value = event->wheel.y;

				WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));

				if (window != nullptr)
					AddEventToInputEventStack(std::move(_event), &window->mouseScrollDispatcher, window);

				break;
			}
			case SDL_MOUSEMOTION: {
				Events::MouseMotion  _event;
				_event.delta = { event->motion.xrel, -event->motion.yrel };
				
				WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));

				if (window != nullptr)
					AddEventToInputEventStack(std::move(_event), &window->mouseMotionDispatcher, window);
				break;
			}
			case SDL_MOUSEBUTTONDOWN: {				
				WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));

				if (window != nullptr)
				{
					Events::KeyPressed _event;
					_event.key = (Key)(event->button.button - 1 + (uint8)Key::MouseLeft);
					_event.time = TimePoint::GetCurrentWorldTime();
					_event.window = window;

					blazeEngineContext.HandleWindowKeyEvent(_event);
				}

				break;
			}
			case SDL_MOUSEBUTTONUP: {
				WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));

				if (window != nullptr)
				{
					Events::KeyReleased _event;
					_event.key = (Key)(event->button.button - 1 + (uint8)Key::MouseLeft);
					_event.time = TimePoint::GetCurrentWorldTime();
					_event.window = window;

					blazeEngineContext.HandleWindowKeyEvent(_event);
				}

				break;
			}
			case SDL_KEYDOWN: {
				auto it = blazeEngineContext.keymap.Find(event->key.keysym.scancode);

				if (it.IsNull())
					break;

				WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));

				if (window != nullptr)
				{
					Events::KeyPressed _event;
					_event.key = it->value;
					_event.time = TimePoint::GetCurrentWorldTime();
					_event.window = window;

					blazeEngineContext.HandleWindowKeyEvent(_event);
				}
				break;
			}
			case SDL_KEYUP: {
				auto it = blazeEngineContext.keymap.Find(event->key.keysym.scancode);

				if (it.IsNull())
					break;

				WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));

				if (window != nullptr)
				{
					Events::KeyReleased _event;
					_event.key = it->value;
					_event.time = TimePoint::GetCurrentWorldTime();
					_event.window = window;

					blazeEngineContext.HandleWindowKeyEvent(_event);
				}				
				break;
			}
			case SDL_TEXTINPUT: {
				Events::TextInput _event;
				_event.input = StringUTF8((void*)event->text.text, strlen(event->text.text));

				WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));

				if (window != nullptr)
					AddEventToInputEventStack(std::move(_event), &window->textInputDispatcher, window);
				break;
			}
			case SDL_WINDOWEVENT: {
				switch (event->window.event) {
				case SDL_WINDOWEVENT_MOVED: {
					Events::WindowMovedEvent _event;
					WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));
					_event.window = window;
					_event.pos = Vec2i(event->window.data1, event->window.data2);
					AddEventToInputEventStack(std::move(_event), &blazeEngineContext.inputEventSystem.windowMovedDispatcher, window);

					if (window != nullptr)
						AddEventToInputEventStack(std::move(_event), &_event.window->movedEventDispatcher, window);
					break;
				}
				case SDL_WINDOWEVENT_MINIMIZED: {
					Events::WindowMinimizedEvent _event;
					WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));
					_event.window = window;
					AddEventToInputEventStack(std::move(_event), &blazeEngineContext.inputEventSystem.windowMinimizedDispatcher, window);

					if (window != nullptr)
						AddEventToInputEventStack(std::move(_event), &_event.window->minimizedEventDispatcher, window);
					break;
				}
				case SDL_WINDOWEVENT_MAXIMIZED: {
					Events::WindowMaximizedEvent _event;
					WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));
					_event.window = window;
					AddEventToInputEventStack(std::move(_event), &blazeEngineContext.inputEventSystem.windowMaximizedDispatcher, window);

					if (window != nullptr)
						AddEventToInputEventStack(std::move(_event), &_event.window->maximizedEventDispatcher, window);
					break;
				}											  
				case SDL_WINDOWEVENT_RESIZED: {					
					WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));

					Events::WindowResizedEvent _event = {
						.window = window,
						.size = Vec2u(event->window.data1, event->window.data2)
					};
					if (window != nullptr)						
						blazeEngineContext.HandleWindowResizeEvent(_event);
					
					break;
				}
				case SDL_WINDOWEVENT_FOCUS_GAINED: {
					Events::WindowFocusGainedEvent _event;

					WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));
					_event.window = window;

					AddEventToInputEventStack(std::move(_event), &blazeEngineContext.inputEventSystem.windowFocusGainedDispatcher, window);

					if (window != nullptr)
						AddEventToInputEventStack(std::move(_event), &_event.window->focusGainedEventDispatcher, window);

					break;
				}
				case SDL_WINDOWEVENT_FOCUS_LOST: {
					Events::WindowFocusLostEvent _event;
					WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));
					_event.window = window;
					AddEventToInputEventStack(std::move(_event), &blazeEngineContext.inputEventSystem.windowFocusLostDispatcher, window);

					if (window != nullptr)
						AddEventToInputEventStack(std::move(_event), &_event.window->focusLostEventDispatcher, window);
					break;
				}
				case SDL_WINDOWEVENT_CLOSE: {
					Events::WindowCloseEvent _event;
					WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));
					_event.window = window;
					AddEventToInputEventStack(std::move(_event), &blazeEngineContext.inputEventSystem.windowCloseDispatcher, window);

					if (window != nullptr)
						AddEventToInputEventStack(std::move(_event), &_event.window->closeEventDispatcher, window);
					break;
				}
				case SDL_WINDOWEVENT_ENTER: {
					Events::WindowMouseEnterEvent _event;
					WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));
					_event.window = window;
					AddEventToInputEventStack(std::move(_event), &blazeEngineContext.inputEventSystem.windowMouseEnterDispatcher, window);

					if (window != nullptr)
						AddEventToInputEventStack(std::move(_event), &_event.window->mouseEnterDispatcher, window);
					break;
				}
				case SDL_WINDOWEVENT_LEAVE: {
					Events::WindowMouseLeaveEvent _event;
					WindowSDL* window = GetWindowFromSDLHandle(SDL_GetWindowFromID(event->window.windowID));
					_event.window = window;
					AddEventToInputEventStack(std::move(_event), &blazeEngineContext.inputEventSystem.windowMouseLeaveDispatcher, window);

					if (window != nullptr)
						AddEventToInputEventStack(std::move(_event), &_event.window->mouseLeaveDispatcher, window);
					break;
				}
				}
			}
			}


			return 0;
		}
	}
}