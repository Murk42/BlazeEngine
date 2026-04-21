#include "pch.h"
#include "BlazeEngine/Runtime/BlazeEngineContextInternal.h"
#include "BlazeEngine/External/SDL/SDLConversions.h"
#include "BlazeEngine/External/SDL/SDL.h"
#include "BlazeEngine/External/FreeType/FreeType.h"
#include "BlazeEngine/External/Sail/Sail.h"

namespace Blaze
{
	static BlazeEngineContextInternal* blazeEngineContext;

	Window* BlazeEngineContext::GetWindowFromHandle(void* handle) const
	{
		if ((BlazeEngineContextInternal*)this != blazeEngineContext)
		{
			BLAZE_LOG_ERROR("Calling GetWindowFromHandle from an unexpected context object");
			return nullptr;
		}

		if (this == nullptr)
		{
			BLAZE_LOG_ERROR("Calling GetWindowFromHandle from an nullptr object");
			return nullptr;
		}

		return blazeEngineContext->GetWindowFromHandle((SDL_Window*)handle);
	}

	void BlazeEngineContext::SetInitializationTimingTree(TimingTree initializationTimingTree)
	{
		this->initializationTimingTree = std::move(initializationTimingTree);
	}


#define _STRINGIZE(x) #x
#define STRINGIZE(x) _STRINGIZE(x)

	bool BlazeEngineContext::InitializeEngine(const InitializationParameters& params, TimingTree& timingTree)
	{
		if (blazeEngineContext == nullptr)
		{
			timingTree.Start("SDL initialization");

			if (!InitializeSDL(timingTree))
				return false;

			float SDLTime = timingTree.End();
			int SDLVersion = SDL_GetVersion();
			BLAZE_LOG_INFO("<color=green>Successfully<color/> initialized SDL {}.{}.{} ({.1f}ms)", SDL_VERSIONNUM_MAJOR(SDLVersion), SDL_VERSIONNUM_MINOR(SDLVersion), SDL_VERSIONNUM_MICRO(SDLVersion), SDLTime * 1000.0);


			timingTree.Start("FreeType initialization");

			if (!InitializeFreeType(timingTree))
				return false;

			float freeTypeTime = timingTree.End();
			BLAZE_LOG_INFO("<color=green>Successfully<color/> initialized FreeType " STRINGIZE(FREETYPE_MAJOR) "." STRINGIZE(FREETYPE_MINOR) "." STRINGIZE(FREETYPE_PATCH) " ({.1f}ms)", freeTypeTime * 1000.0);


			timingTree.Start("Sail initialization");

			if (!InitializeSail(timingTree))
				return false;

			float sailTime = timingTree.End();
			BLAZE_LOG_INFO("<color=green>Successfully<color/> initialized sail " SAIL_VERSION_STRING " ({.1f}ms)", sailTime * 1000.0);


			timingTree.Start("Blaze context initialization");
			new BlazeEngineContextInternal(params);
			timingTree.End();


			return true;
		}
		else
			return false;
	}
	void BlazeEngineContext::TerminateEngine()
	{
		if (blazeEngineContext)
		{
			delete blazeEngineContext;
			blazeEngineContext = nullptr;

			TerminateSail();
			TerminateFreeType();
			TerminateSDL();
		}
	}
	BlazeEngineContext* BlazeEngineContext::GetEngineContext()
	{
		return blazeEngineContext;
	}

	BlazeEngineContextInternal::BlazeEngineContextInternal(InitializationParameters parameters) :
		previousEventFilter(nullptr), previousEventFilterUserData(nullptr)
	{
		blazeEngineContext = this;

		nudgeEventNumber = SDL_RegisterEvents(1);

		SDL_GetEventFilter(&previousEventFilter, &previousEventFilterUserData);
		SDL_SetEventFilter(SDLEventFilter, this);
	}
	BlazeEngineContextInternal::~BlazeEngineContextInternal()
	{
		if (!windowsData.Empty())
			BLAZE_LOG_ERROR("The engine context is being destroyed but there are windows that aren't destroyed");

		SDL_SetEventFilter(previousEventFilter, previousEventFilterUserData);
	}
	void BlazeEngineContextInternal::RegisterWindow(Window& window)
	{
		if (window.GetHandle() == nullptr)
			BLAZE_LOG_FATAL("Registering a null window");

		std::lock_guard lg{ windowsMutex };
		windowsData.Insert(static_cast<SDL_Window*>(window.GetHandle()), WindowData{ &window });
	}
	void BlazeEngineContextInternal::UnregisterWindow(Window& window)
	{
		if (window.GetHandle() == nullptr)
			BLAZE_LOG_FATAL("Unregistering a null window");

		std::lock_guard lg{ windowsMutex };
		windowsData.Erase(static_cast<SDL_Window*>(window.GetHandle()));
	}
	void BlazeEngineContextInternal::MoveWindow(Window& window)
	{
		if (window.GetHandle() == nullptr)
			BLAZE_LOG_FATAL("Moving a null window");

		std::lock_guard lg{ windowsMutex };

		auto it = windowsData.Find(static_cast<SDL_Window*>(window.GetHandle()));

		if (it.IsNull())
			BLAZE_LOG_FATAL("Window is not registered");

		it->value.object = &window;
	}
	Window* BlazeEngineContextInternal::GetWindowFromHandle(SDL_Window* handle)
	{
		if (handle == nullptr)
			return nullptr;

		std::lock_guard lg{ windowsMutex };

		auto it = windowsData.Find(handle);

		if (it.IsNull())
			return nullptr;

		return it->value.object;
	}
	Window* BlazeEngineContextInternal::GetWindowFromWindowID(SDL_WindowID id)
	{
		std::lock_guard lg{ windowsMutex };

		Window* out = nullptr;

		if (id == 0)
			return out;

		SDL_Window* handle = SDL_GetWindowFromID(id);

		if (handle == nullptr)
			return nullptr;

		auto it = windowsData.Find(handle);

		if (it.IsNull())
			return nullptr;

		return it->value.object;
	}
	void BlazeEngineContextInternal::NudgeMainThread()
	{
		SDL_Event event{ };
		event.type = nudgeEventNumber;
		SDL_PushEvent(&event);
	}
	bool BlazeEngineContextInternal::GetKeyState(Input::Key key) const
	{
		return keysCurrentState[static_cast<uintMem>(key)].test();
	}
	bool BlazeEngineContextInternal::GetMouseButtonState(Input::MouseButton button) const
	{
		return mouseButtonsCurrentState[static_cast<uintMem>(button)].test();
	}
	Array<Input::MouseData> BlazeEngineContextInternal::GetMice()
	{
		return mice;
	}
	void BlazeEngineContextInternal::PollEvents()
	{
		SDL_Event event;
		while (SDL_PollEvent(&event));

		ProcessEventQueue();
	}
	bool BlazeEngineContextInternal::WaitForEvents(float seconds)
	{
		SDL_Event event;
		if (SDL_WaitEventTimeout(&event, seconds * 1000.0f))
		{
			ProcessEventQueue();
			return true;
		}

		return false;
	}
	BlazeEngineContextInternal* BlazeEngineContextInternal::GetEngineContext()
	{
		return blazeEngineContext;
	}
	void BlazeEngineContextInternal::UpdateMouseData(const Input::MouseData& data)
	{
		uintMem index = 0;
		for (; index != mice.Count(); ++index)
			if (mice[index].id == data.id)
				break;

		if (index == mice.Count())
			mice.AddBack(data);
		else
			mice[index] = data;
	}
	void BlazeEngineContextInternal::EraseMouseData(Input::MouseID id)
	{
		uintMem index = 0;
		for (; index != mice.Count(); ++index)
			if (mice[index].id == id)
				break;

		if (index == mice.Count())
			return;

		mice.EraseAt(index);
	}
	void BlazeEngineContextInternal::ProcessEventQueue()
	{
		GlobalGenericEvent event;
		while (eventQueue.GetEvent(event))
		{
			switch (event.GetValueType())
			{
			case GlobalGenericEvent::GetValueTypeOf<Display::DisplayEvent>(): {
				event.TryProcess([&](const Display::DisplayEvent& event) {
					displayEventDispatcher.Call(event);
					});
				break;
			}
			}
		}
	}
	void BlazeEngineContextInternal::ProcessSDLEvent(SDL_Event* event, bool calledFromEventWatch)
	{
		switch (event->type)
		{
		case SDL_EVENT_MOUSE_WHEEL: {
			Window* window = GetWindowFromWindowID(event->wheel.windowID);

			if (window == nullptr)
				break;

			Input::MouseScrollEvent _event{
				.window = *window,
				.timeNS = event->wheel.timestamp,
				.mouseID = Input::MouseID(0),//Input::MouseID(event->button.which),
				.amount = { event->wheel.x, event->wheel.y },
				.direction = event->wheel.direction == SDL_MOUSEWHEEL_NORMAL ? Input::ScrollDirection::Normal : Input::ScrollDirection::Flipped,
				.pos = Vec2f(event->wheel.mouse_x, event->wheel.mouse_y)
			};

			UpdateMouseData({ _event.mouseID, _event.pos, window });

			window->AddInputEvent(_event);
			break;
		}
		case SDL_EVENT_MOUSE_MOTION: {
			Window* window = GetWindowFromWindowID(event->motion.windowID);

			if (window == nullptr)
				break;

			Input::MouseMotionEvent _event{
				.window = *window,
				.timeNS = event->motion.timestamp,
				.mouseID = Input::MouseID(0),//Input::MouseID(event->button.which),
				.delta = { event->motion.xrel, event->motion.yrel },
				.pos = Vec2f(event->motion.x, event->motion.y)
			};

			UpdateMouseData({ _event.mouseID, _event.pos, window });

			window->AddInputEvent(_event);
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_DOWN: {
			Window* window = GetWindowFromWindowID(event->button.windowID);

			if (window == nullptr)
				break;

			auto button = SDL_GetBlazeMouseButton(event->button.button);

			mouseButtonsCurrentState[static_cast<uintMem>(button)].test_and_set();

			Input::MouseButtonDownEvent _event{
				.window = *window,
				.timeNS = event->button.timestamp,
				.mouseID = Input::MouseID(0),//Input::MouseID(event->button.which),
				.button = button,
				.combo = event->button.clicks,
				.pos = Vec2f(event->button.x, event->button.y)
			};

			UpdateMouseData({ _event.mouseID, _event.pos, window });

			window->AddInputEvent(_event);
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_UP: {
			Window* window = GetWindowFromWindowID(event->button.windowID);

			if (window == nullptr)
				break;

			auto button = SDL_GetBlazeMouseButton(event->button.button);

			mouseButtonsCurrentState[static_cast<uintMem>(button)].clear();

			Input::MouseButtonUpEvent _event{
				.window = *window,
				.timeNS = event->button.timestamp,
				.mouseID = Input::MouseID(0),//Input::MouseID(event->button.which), //SDL sometimes retrieved an unkown mouse id so for now we just set it to 0
				.button = button,
				.combo = event->button.clicks,
				.pos = Vec2f(event->button.x, event->button.y)
			};

			UpdateMouseData({ _event.mouseID, _event.pos, window });

			window->AddInputEvent(_event);
			break;
		}
		case SDL_EVENT_KEY_DOWN: {
			Window* window = GetWindowFromWindowID(event->key.windowID);

			if (window == nullptr)
				break;

			keysCurrentState[static_cast<uintMem>(event->key.scancode)].test_and_set();

			Input::KeyDownEvent _event{
				.window = *window,
				.timeNS = event->key.timestamp,
				.keyboardID = Input::KeyboardID(event->key.which),
				.key = (Input::Key)event->key.scancode,
				.modifier = (Input::KeyModifier)event->key.mod,
				.keyCode = (Input::KeyCode)event->key.key,
				.repeat = event->key.repeat
			};

			window->AddInputEvent(_event);
			break;
		}
		case SDL_EVENT_KEY_UP: {
			Window* window = GetWindowFromWindowID(event->key.windowID);

			if (window == nullptr)
				break;

			Input::KeyUpEvent _event{
				.window = *window,
				.timeNS = event->key.timestamp,
				.keyboardID = Input::KeyboardID(event->key.which),
				.key = (Input::Key)event->key.scancode,
				.modifier = (Input::KeyModifier)event->key.mod,
				.keyCode = (Input::KeyCode)event->key.key,
			};

			keysCurrentState[static_cast<uintMem>(event->key.scancode)].clear();

			window->AddInputEvent(_event);
			break;
		}
		case SDL_EVENT_TEXT_INPUT: {
			Window* window = GetWindowFromWindowID(event->text.windowID);

			if (window == nullptr)
				break;

			Input::TextInputEvent _event{
				.window = *window,
				.timeNS = event->text.timestamp,
				.string = u8String(event->text.text, strlen(event->text.text))
			};

			window->AddInputEvent(_event);
			break;
		}
		case SDL_EVENT_DISPLAY_ORIENTATION: {
			Display::DisplayEvent _event{
				.displayID = (Display::DisplayID)event->display.displayID,
				.timeNS = event->display.timestamp,
				.type = Display::DisplayEvent::Type::OrientationChanged
			};

			if (calledFromEventWatch)
				eventQueue.AddEvent(_event);
			else
				displayEventDispatcher.Call(_event);
			break;
		}
		case SDL_EVENT_DISPLAY_ADDED: {
			Display::DisplayEvent _event{
				.displayID = (Display::DisplayID)event->display.displayID,
				.timeNS = event->display.timestamp,
				.type = Display::DisplayEvent::Type::DisplayAdded
			};

			if (calledFromEventWatch)
				eventQueue.AddEvent(_event);
			else
				displayEventDispatcher.Call(_event);
			break;
		}
		case SDL_EVENT_DISPLAY_REMOVED: {
			Display::DisplayEvent _event{
				.displayID = (Display::DisplayID)event->display.displayID,
				.timeNS = event->display.timestamp,
				.type = Display::DisplayEvent::Type::DisplayRemoved
			};

			if (calledFromEventWatch)
				eventQueue.AddEvent(_event);
			else
				displayEventDispatcher.Call(_event);
			break;
		}
		case SDL_EVENT_DISPLAY_MOVED: {
			Display::DisplayEvent _event{
				.displayID = (Display::DisplayID)event->display.displayID,
				.timeNS = event->display.timestamp,
				.type = Display::DisplayEvent::Type::Moved
			};

			eventQueue.AddEvent(_event);
			break;
		}
		case SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED: {
			Display::DisplayEvent _event{
				.displayID = (Display::DisplayID)event->display.displayID,
				.timeNS = event->display.timestamp,
				.type = Display::DisplayEvent::Type::DesktopModeChanged
			};

			if (calledFromEventWatch)
				eventQueue.AddEvent(_event);
			else
				displayEventDispatcher.Call(_event);
			break;
		}
		case SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED: {
			Display::DisplayEvent _event{
				.displayID = (Display::DisplayID)event->display.displayID,
				.timeNS = event->display.timestamp,
				.type = Display::DisplayEvent::Type::CurrentModeChanged
			};

			if (calledFromEventWatch)
				eventQueue.AddEvent(_event);
			else
				displayEventDispatcher.Call(_event);
			break;
		}
		case SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED: {
			Display::DisplayEvent _event{
				.displayID = (Display::DisplayID)event->display.displayID,
				.timeNS = event->display.timestamp,
				.type = Display::DisplayEvent::Type::ContentScaleChanged
			};

			if (calledFromEventWatch)
				eventQueue.AddEvent(_event);
			else
				displayEventDispatcher.Call(_event);
			break;
		} 
		case SDL_EVENT_WINDOW_MOVED: {

			Window* window = GetWindowFromWindowID(event->window.windowID);

			if (window == nullptr)
				break;

			Window::MovedEvent _event{
				.window = *window,
				.timeNS = event->window.timestamp,
				.pos = Vec2i(event->window.data1, event->window.data2)
			};

			window->AddWindowEvent(_event);
			break;
		}
		case SDL_EVENT_WINDOW_MINIMIZED: {
			Window* window = GetWindowFromWindowID(event->window.windowID);

			if (window == nullptr)
				break;

			Window::MinimizedEvent _event{
				.window = *window,
				.timeNS = event->window.timestamp,
			};

			window->AddWindowEvent(_event);
			break;
		}
		case SDL_EVENT_WINDOW_MAXIMIZED: {
			Window* window = GetWindowFromWindowID(event->window.windowID);

			if (window == nullptr)
				break;

			Window::MaximizedEvent _event{
				.window = *window,
				.timeNS = event->window.timestamp,
			};

			window->AddWindowEvent(_event);
			break;
		}
		case SDL_EVENT_WINDOW_EXPOSED:
		{
			Window* window = GetWindowFromWindowID(event->window.windowID);

			if (window == nullptr)
				break;

			break;
		}
		case SDL_EVENT_WINDOW_RESIZED: {
			break;
		}
		case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: {
			Window* window = GetWindowFromWindowID(event->window.windowID);

			if (window == nullptr)
				break;

			Window::ResizedEvent _event{
				.window = *window,
				.timeNS = event->window.timestamp,
				.size = Vec2u(event->window.data1, event->window.data2)
			};

			window->AddWindowEvent(_event);
			break;
		}
		case SDL_EVENT_WINDOW_FOCUS_GAINED: {
			Window* window = GetWindowFromWindowID(event->window.windowID);

			if (window == nullptr)
				break;

			Window::FocusGainedEvent _event{
				.window = *window,
				.timeNS = event->window.timestamp,
			};

			SDL_StartTextInput((SDL_Window*)window->GetHandle());

			window->AddWindowEvent(_event);
			break;
		}
		case SDL_EVENT_WINDOW_FOCUS_LOST: {
			Window* window = GetWindowFromWindowID(event->window.windowID);

			if (window == nullptr)
				break;

			Window::FocusLostEvent _event{
				.window = *window,
				.timeNS = event->window.timestamp,
			};

			window->AddWindowEvent(_event);

			SDL_StopTextInput((SDL_Window*)window->GetHandle());
			break;
		}
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
			Window* window = GetWindowFromWindowID(event->window.windowID);

			if (window == nullptr)
				break;

			Window::CloseEvent _event{
				.window = *window,
				.timeNS = event->window.timestamp,
			}; 

			window->AddWindowEvent(_event);
			break;
		}
		case SDL_EVENT_WINDOW_MOUSE_ENTER: {
			Window* window = GetWindowFromWindowID(event->window.windowID);
			 
			if (window == nullptr)
				break;
		
			Vec2f pos{ };
			SDL_GetGlobalMouseState(&pos.x, &pos.y);
			Vec2i windowPos{ };
			SDL_GetWindowPosition(static_cast<SDL_Window*>(window->GetHandle()), &windowPos.x, &windowPos.y);

			pos -= Vec2f(windowPos);

			Input::MouseEntersWindowEvent _event{
				.window = *window,
				.timeNS = event->window.timestamp,
				.mouseID = Input::MouseID(0),
				.pos = pos
			};

			UpdateMouseData({ _event.mouseID, _event.pos, window });
			
			window->AddInputEvent(_event);
			break;
		}
		case SDL_EVENT_WINDOW_MOUSE_LEAVE: {
			Window* window = GetWindowFromWindowID(event->window.windowID);

			if (window == nullptr)
				break;

			Vec2f pos{ };
			SDL_GetMouseState(&pos.x, &pos.y);

			Input::MouseLeavesWindowEvent _event{
				.window = *window,
				.timeNS = event->window.timestamp,
				.mouseID = Input::MouseID(0),
				.pos = pos
			};

			EraseMouseData(_event.mouseID);

			window->AddInputEvent(_event);
			break;
		}
		case SDL_EVENT_WINDOW_DISPLAY_CHANGED: {
			Window* window = GetWindowFromWindowID(event->window.windowID);

			if (window == nullptr)
				break;

			Window::DisplayChangedEvent _event{
				.window = *window,
				.timeNS = event->window.timestamp,
				.newDisplayID = static_cast<uint32>(event->window.data1)
			};

			window->AddWindowEvent(_event);
			break;
		}
		case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED: {
			Window* window = GetWindowFromWindowID(event->window.windowID);

			if (window == nullptr)
				break;

			Window::ContentScaleChangedEvent _event{
				.window = *window,
				.newContentScale = window->GetContentScale()
			};

			window->AddWindowEvent(_event);
			break;
		}
		}
	}
	bool BlazeEngineContextInternal::SDLEventFilter(void* userData, SDL_Event* event)
	{
		BlazeEngineContextInternal* context = static_cast<BlazeEngineContextInternal*>(userData);
		 
		if (context != blazeEngineContext)
		{
			BLAZE_LOG_ERROR("SDL event filter called with an invalid engine context. Ignoring event");
			return false;
		}

		if (context == nullptr)
		{
			BLAZE_LOG_ERROR("SDL event filter called, but the engine context isn't valid. Ignoring event");
			return false;
		}

		if (context->previousEventFilter != nullptr)
			if (!context->previousEventFilter(context->previousEventFilterUserData, event))
				return false;

		context->ProcessSDLEvent(event, true);

		return false; //The return value is ignored when this function is used as an event watcher
	}
}