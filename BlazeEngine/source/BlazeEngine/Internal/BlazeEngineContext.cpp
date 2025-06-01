#include "pch.h"
#include "BlazeEngine/Internal/BlazeEngineContext.h"
#include "BlazeEngine/Internal/Libraries/SDL.h"
#include "BlazeEngine/Internal/Libraries/SDLConversions.h"
#include "BlazeEngine/Console/Console.h"
#include "BlazeEngine/Window/Window.h"
#include "SDL3/SDL_events.h"

namespace Blaze
{
	ConsoleOutputStream consoleOutputStream;
	BlazeEngineContext blazeEngineContext;
	
	template<typename T>
	static void AddToInputEventStack(const T& event, EventDispatcher<T>* dispatcher)
	{
		const char* name = typeid(T).name();
		uintMem length = strlen(name);
		blazeEngineContext.eventStack.Add<T>(StringView(name, length), [](const T& event, EventDispatcher<T>* dispatcher) {
			dispatcher->Call(event);
			}, event, dispatcher);		
	}
	//TODO make this more performant bu passing user_data parameter to function so that a lambda doesn't need to capute anything and therefore no heap allocations
	template<typename T, typename F> requires std::invocable<F, const T&>
	static void AddToInputEventStack(const T& event, F&& function)
	{
		const char* name = typeid(T).name();
		uintMem length = strlen(name);
		blazeEngineContext.eventStack.Add<T>(StringView(name, length), std::forward<F>(function), event);
	}

	struct WindowObjects
	{
		SDL_Window* windowSDL;
		Window* window;
	};
	static WindowObjects GetWindowObjects(SDL_WindowID id)
	{
		WindowObjects out{ nullptr, nullptr };

		if (id == 0)		
			return out;

		out.windowSDL = SDL_GetWindowFromID(id);

		if (out.windowSDL == nullptr)		
			return out;		

		out.window = Window::GetWindowFromHandle(out.windowSDL);		

		return out;
	}

	BlazeEngineContext::BlazeEngineContext() :
		mainThreadID(std::this_thread::get_id()),
		clientThreadExitEventIdentifier(0),
		mainThreadTaskEventIdentifier(0),
		cursors(),
		primaryDesktopHeight(0),
		nCmdShow(0)
	{
		downKeys.reset();
		pressedKeysDuringUpdate.reset();
		releasedKeysDuringUpdate.reset();		
	}

	BlazeEngineContext::~BlazeEngineContext()
	{
	}

	void BlazeEngineContext::ExecuteTaskOnMainThread(void(*function)(void*), void* userData)
	{		
		if (std::this_thread::get_id() == mainThreadID)		
			function(userData);		
		else
		{
			std::lock_guard lk{ mainThreadTaskMutex };
			mainThreadTaskfunction = function;
			mainThreadTaskUserData = userData;			

			mainThreadTaskFlag.test_and_set();

			PostSDLMainThreadTaskEvent();			

			mainThreadTaskFlag.wait(1);
		}
	}
	void BlazeEngineContext::ExecuteMainThreadTask()
	{
		if (mainThreadTaskFlag.test())
		{
			if (mainThreadTaskfunction)
				mainThreadTaskfunction(mainThreadTaskUserData);

			mainThreadTaskfunction = nullptr;
			mainThreadTaskUserData = nullptr;

			mainThreadTaskFlag.clear();
			mainThreadTaskFlag.notify_one();
		}
	}
	void BlazeEngineContext::UpdateInputState()
	{				
		releasedKeysDuringUpdate.reset();
		pressedKeysDuringUpdate.reset();

		lastUpdateDesktopMousePos = currentDesktopMousePos;
		lastUpdateMouseMovement = currentUpdateMouseMovementSum;
		currentUpdateMouseMovementSum = Vec2f();
	}
	void BlazeEngineContext::ProcessAllEvents()
	{
		if (!createClientThread)
		{
			SDL_Event event;
			while (SDL_PollEvent(&event));
		}

		eventStack.ProcessAndClear();
	}
	static Mouse::MouseButton GetBlazeMouseButton(int value)
	{
		switch (value)
		{
		case SDL_BUTTON_LEFT: return Mouse::MouseButton::Left;
		case SDL_BUTTON_RIGHT: return Mouse::MouseButton::Right;
		case SDL_BUTTON_MIDDLE: return Mouse::MouseButton::Middle;
		default:
			return Mouse::MouseButton::Other;
			break;
		}
	}
	void BlazeEngineContext::HandleSDLEvent(const SDL_Event& event)
	{
		//BLAZE_ENGINE_CORE_DEBUG(GetSDLEventName(event.type));

		if (event.type == mainThreadTaskEventIdentifier)
		{
			if (!createClientThread)
				BLAZE_ENGINE_CORE_FATAL("Main thred task event has been submited but the application was run without creating a client thread");
			
			return;
		}
		if (event.type == clientThreadExitEventIdentifier)
		{
			clientThreadExited.test_and_set();
			return;
		}
								
		switch (event.type)
		{
		case SDL_EVENT_MOUSE_WHEEL: {				
			auto [windowSDL, window] = GetWindowObjects(event.wheel.windowID);

			Vec2f windowPos = Vec2f(event.wheel.x, window == nullptr ? primaryDesktopHeight - event.wheel.y : window->size.y - event.wheel.y);
			Vec2f desktopPos = window == nullptr ? windowPos : windowPos + (Vec2f)window->pos;

			Mouse::MouseScrollEvent _event{
				.window = window,
				.timeNS = event.wheel.timestamp,
				.mouseID = event.wheel.which,
				.amount = { event.wheel.x, event.wheel.y },
				.direction = event.wheel.direction == SDL_MOUSEWHEEL_NORMAL ? Mouse::ScrollDirection::Normal : Mouse::ScrollDirection::Flipped,
				.pos = windowPos,
				.desktopPos = desktopPos
			};

			AddToInputEventStack(_event, &mouseScrollEventDispatcher);
			if (window != nullptr)
				AddToInputEventStack(_event, &window->mouseScrollEventDispatcher);

			break;
		}
		case SDL_EVENT_MOUSE_MOTION: {		
			auto [windowSDL, window] = GetWindowObjects(event.motion.windowID);

			Vec2f windowPos = Vec2f(event.motion.x, window == nullptr ? primaryDesktopHeight - event.motion.y : window->size.y - event.motion.y);			
			Vec2f desktopPos = window == nullptr ? windowPos : windowPos + (Vec2f)window->pos;			
			
			currentUpdateMouseMovementSum += Vec2f(event.motion.xrel, -event.motion.yrel);			
			currentDesktopMousePos = desktopPos;

			Mouse::MouseMotionEvent _event{
				.window = window,
				.timeNS = event.motion.timestamp,
				.mouseID = event.motion.which,
				.delta = { event.motion.xrel, -event.motion.yrel },				
				.pos = windowPos,
				.desktopPos = desktopPos
			};			

			AddToInputEventStack(_event, &mouseMotionEventDispatcher);
			if (window != nullptr)
				AddToInputEventStack(_event, &window->mouseMotionEventDispatcher);

			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_DOWN: {
			auto [windowSDL, window] = GetWindowObjects(event.button.windowID);

			Vec2f windowPos = Vec2f(event.button.x, window == nullptr ? primaryDesktopHeight - event.button.y : window->size.y - event.button.y);
			Vec2f desktopPos = window == nullptr ? windowPos : windowPos + (Vec2f)window->pos;			

			Mouse::MouseButtonDownEvent _event{
				.window = window,
				.timeNS = event.button.timestamp,
				.mouseID = event.button.which,
				.button = GetBlazeMouseButton(event.button.button),
				.combo = event.button.clicks,
				.pos = windowPos,
				.desktopPos = desktopPos
			};

			AddToInputEventStack(_event, &mouseButtonDownEventDispatcher);
			if (window != nullptr)
				AddToInputEventStack(_event, &window->mouseButtonDownEventDispatcher);
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_UP: {
			auto [windowSDL, window] = GetWindowObjects(event.button.windowID);

			Vec2f windowPos = Vec2f(event.button.x, window == nullptr ? primaryDesktopHeight - event.button.y : window->size.y - event.button.y);
			Vec2f desktopPos = window == nullptr ? windowPos : windowPos + (Vec2f)window->pos;

			Mouse::MouseButtonUpEvent _event{
				.window = window,
				.timeNS = event.button.timestamp,
				.mouseID = event.button.which,
				.button = GetBlazeMouseButton(event.button.button),
				.combo = event.button.clicks,
				.pos = windowPos,
				.desktopPos = desktopPos
			};

			AddToInputEventStack(_event, &mouseButtonUpEventDispatcher);
			if (window != nullptr)
				AddToInputEventStack(_event, &window->mouseButtonUpEventDispatcher);
			
			break;
		}
		case SDL_EVENT_KEY_DOWN: {			
			auto [windowSDL, window] = GetWindowObjects(event.key.windowID);

			Keyboard::KeyDownEvent _event{
				.window = window,
				.timeNS = event.key.timestamp,
				.keyboardID = event.key.which,
				.key = (Keyboard::Key)event.key.scancode,
				.modifier = (Keyboard::KeyModifier)event.key.mod,
				.keyCode = (Keyboard::KeyCode)event.key.key,
				.repeat = event.key.repeat
			};

			
			downKeys.set((uintMem)event.key.scancode);

			AddToInputEventStack(_event, [this](const Keyboard::KeyDownEvent& event) {
				if (!event.repeat)
				{
					pressedKeysDuringUpdate.set((uintMem)event.key);
				}

				keyDownEventDispatcher.Call(event);
				});
			if (window != nullptr)
				AddToInputEventStack(_event, &window->keyDownEventDispatcher);
			break;
		}
		case SDL_EVENT_KEY_UP: {
			auto [windowSDL, window] = GetWindowObjects(event.key.windowID);

			Keyboard::KeyUpEvent _event{
				.window = window,
				.timeNS = event.key.timestamp,
				.keyboardID = event.key.which,
				.key = (Keyboard::Key)event.key.scancode,
				.modifier = (Keyboard::KeyModifier)event.key.mod,
				.keyCode = (Keyboard::KeyCode)event.key.key,
			};			

			downKeys.reset(event.key.scancode);

			AddToInputEventStack(_event, [this](const Keyboard::KeyUpEvent& event) {				
				releasedKeysDuringUpdate.set((uintMem)event.key);

				keyUpEventDispatcher.Call(event);
				});
			if (window != nullptr)
				AddToInputEventStack(_event, &window->keyUpEventDispatcher);

			break;
		}
		case SDL_EVENT_TEXT_INPUT: {
			auto [windowSDL, window] = GetWindowObjects(event.text.windowID);

			Keyboard::TextInputEvent _event{
				.window = window,
				.timeNS = event.text.timestamp,			
				.string = StringUTF8((void*)event.text.text, strlen(event.text.text))
			};			
			
			AddToInputEventStack(_event, &textInputEventDispatcher);
			if (window != nullptr)
				AddToInputEventStack(_event, &window->textInputEventDispatcher);

			break;
		}
		case SDL_EVENT_DISPLAY_ORIENTATION: {

			Display::DisplayEvent _event{
				.displayID = (Display::DisplayID)event.display.displayID,
				.timeNS = event.display.timestamp,				
			};

			AddToInputEventStack(_event, &displayEventDispatcher);			
			break;
			}
		case SDL_EVENT_DISPLAY_ADDED: {
			Display::DisplayEvent _event{
				.displayID = (Display::DisplayID)event.display.displayID,
				.timeNS = event.display.timestamp,				
			};

			AddToInputEventStack(_event, &displayEventDispatcher);
			break;
			}
		case SDL_EVENT_DISPLAY_REMOVED: {
			Display::DisplayEvent _event{
				.displayID = (Display::DisplayID)event.display.displayID,
				.timeNS = event.display.timestamp,
			};

			AddToInputEventStack(_event, &displayEventDispatcher);
			break;
			}
		case SDL_EVENT_DISPLAY_MOVED: {			
			Display::DisplayEvent _event{
				.displayID = (Display::DisplayID)event.display.displayID,
				.timeNS = event.display.timestamp
			};

			AddToInputEventStack(_event, &displayEventDispatcher);
			break;
			}
		case SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED: {
			Display::DisplayEvent _event{
				.displayID = (Display::DisplayID)event.display.displayID,
				.timeNS = event.display.timestamp,				
			};

			AddToInputEventStack(_event, &displayEventDispatcher);
			break;
			}
		case SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED: {			
			Display::DisplayEvent _event{
				.displayID = (Display::DisplayID)event.display.displayID,
				.timeNS = event.display.timestamp
			};

			AddToInputEventStack(_event, &displayEventDispatcher);
			break;
			}
		case SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED: {
			Display::DisplayEvent _event{
				.displayID = (Display::DisplayID)event.display.displayID,
				.timeNS = event.display.timestamp
			};

			AddToInputEventStack(_event, &displayEventDispatcher);
			break;
			}
		case SDL_EVENT_WINDOW_MOVED: {
			auto [windowSDL, window] = GetWindowObjects(event.window.windowID);

			if (window == nullptr)
				break;			

			Window::WindowMovedEvent _event{
				.window = *window,
				.timeNS = event.window.timestamp,
				.pos = Vec2i(event.window.data1, primaryDesktopHeight - event.window.data2)
			}; 			
									
			window->HandleMovedEvent(_event);			
			break;
		}
		case SDL_EVENT_WINDOW_MINIMIZED: {
			auto [windowSDL, window] = GetWindowObjects(event.window.windowID);

			if (window == nullptr)
				break;

			Window::WindowMinimizedEvent _event{
				.window = *window,
				.timeNS = event.window.timestamp,
			};			
			
			AddToInputEventStack(_event, &windowMinimizedEventDispatcher);
			AddToInputEventStack(_event, &window->windowMinimizedEventDispatcher);
			break;
		}
		case SDL_EVENT_WINDOW_MAXIMIZED: {
			auto [windowSDL, window] = GetWindowObjects(event.window.windowID);

			if (window == nullptr)
				break;

			Window::WindowMaximizedEvent _event{
				.window = *window,
				.timeNS = event.window.timestamp,
			};
			
			AddToInputEventStack(_event, &windowMaximizedEventDispatcher);
			AddToInputEventStack(_event, &window->windowMaximizedEventDispatcher);
			break;
		}
		case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: {			
			auto [windowSDL, window] = GetWindowObjects(event.window.windowID);

			if (window == nullptr)
				break;			

			Window::WindowResizedEvent _event{
				.window = *window,
				.timeNS = event.window.timestamp,
				.size = Vec2u(event.window.data1, event.window.data2)
			};			
			
			window->HandleResizeEvent(_event);
			break;
		}
		case SDL_EVENT_WINDOW_FOCUS_GAINED: {
			auto [windowSDL, window] = GetWindowObjects(event.window.windowID);

			if (window == nullptr)
				break;

			Window::WindowFocusGainedEvent _event{
				.window = *window,
				.timeNS = event.window.timestamp,
			};			

			SDL_StartTextInput(windowSDL);
			
			AddToInputEventStack(_event, &windowFocusGainedEventDispatcher);
			AddToInputEventStack(_event, &window->windowFocusGainedEventDispatcher);

			break;
		}
		case SDL_EVENT_WINDOW_FOCUS_LOST: {
			auto [windowSDL, window] = GetWindowObjects(event.window.windowID);

			if (window == nullptr)
				break;

			Window::WindowFocusLostEvent _event{
				.window = *window,
				.timeNS = event.window.timestamp,
			};			
			
			AddToInputEventStack(_event, &windowFocusLostEventDispatcher);
			AddToInputEventStack(_event, &window->windowFocusLostEventDispatcher);

			SDL_StopTextInput(windowSDL);
			break;
		}
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
			auto [windowSDL, window] = GetWindowObjects(event.window.windowID);

			if (window == nullptr)
				break;

			Window::WindowCloseEvent _event{
				.window = *window,
				.timeNS = event.window.timestamp,
			};			
			
			AddToInputEventStack(_event, &windowCloseEventDispatcher);
			AddToInputEventStack(_event, &window->windowCloseEventDispatcher);

			break;
		}
		case SDL_EVENT_WINDOW_MOUSE_ENTER: {
			auto [windowSDL, window] = GetWindowObjects(event.window.windowID);

			if (window == nullptr)
				break;
			
			Window::WindowMouseEnterEvent _event{
				.window = *window,
				.timeNS = event.window.timestamp,
			};
			
			AddToInputEventStack(_event, &windowMouseEnterEventDispatcher);
			AddToInputEventStack(_event, &window->windowMouseEnterEventDispatcher);

			break;
		}
		case SDL_EVENT_WINDOW_MOUSE_LEAVE: {
			auto [windowSDL, window] = GetWindowObjects(event.window.windowID);

			if (window == nullptr)
				break;

			Window::WindowMouseLeaveEvent _event{
				.window = *window,
				.timeNS = event.window.timestamp,
			};			
					
			AddToInputEventStack(_event, &windowMouseLeaveEventDispatcher);
			AddToInputEventStack(_event, &window->windowMouseLeaveEventDispatcher);

			break;
		}
		case SDL_EVENT_WINDOW_DISPLAY_CHANGED: {
			auto [windowSDL, window] = GetWindowObjects(event.window.windowID);

			if (window == nullptr)
				break;
			
			Window::WindowDisplayChangedEvent _event{
				.window = *window,
				.timeNS = event.window.timestamp,
				.displayID = static_cast<uint32>(event.window.data1)
			};						

			AddToInputEventStack(_event, &windowDisplayChangedEventDispatcher);
			AddToInputEventStack(_event, &window->windowDisplayChangedEventDispatcher);

			break;
		}
		}
	}		
}