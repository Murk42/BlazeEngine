#include "pch.h"
#include "BlazeEngine/Runtime/BlazeEngineContextInternal.h"
#include "BlazeEngine/External/SDL/SDLConversions.h"
//#include <SDL3/SDL_events.h>
//#include <SDL3/SDL_mouse.h>
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

	bool BlazeEngineContext::InitializeEngine(const InitializationParameters& params)
	{
		if (blazeEngineContext == nullptr)
		{
			InitializeSDL();
			InitializeFreeType();
			InitializeSail();

			blazeEngineContext = new BlazeEngineContextInternal(params);
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
		ResetInputState();

		SDL_GetEventFilter(&previousEventFilter, &previousEventFilterUserData);
		SDL_SetEventFilter(SDLEventFilter, this);
	}
	BlazeEngineContextInternal::~BlazeEngineContextInternal()
	{
		if (!windowsData.Empty())
			BLAZE_LOG_ERROR("The engine context is being destroyed but there are windows that aren't destroyed");

		//for (auto& cursor : cursors)
		//	if (cursor != nullptr)
		//		SDL_DestroyCursor((SDL_Cursor*)cursor);

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
//	void BlazeEngineContextInternal::UpdateInputAndEvents()
//	{
//		CopyInputState();
//
//		if (!separateClientThreadMode)
//		{
//			SDL_Event event;
//			while (SDL_PollEvent(&event));
//		}
//
//		clientEventStack.ProcessEvents();
//
//#ifdef BLAZE_PLATFORM_WINDOWS
//		sentMovedWindowEvent = false;
//		sentResizedWindowEvent = false;
//#endif
//	}
	//void* BlazeEngineContextInternal::GetSystemCursor(Input::CursorType type)
	//{
	//	std::lock_guard lg{ cursorsMutex };
	//
	//	auto& cursor = cursors[(uint)type];
	//
	//	if (cursor == nullptr)
	//	{
	//		struct Parameters
	//		{
	//			SDL_SystemCursor type;
	//			void** cursor;
	//		} parameters{ SDL_GetSDLSystemCursor(type), &cursor };
	//
	//		uint64 timeoutMilliseconds = 1000;
	//		if (!ExecuteOnMainThread([](void* parameters) { *((Parameters*)parameters)->cursor = SDL_CreateSystemCursor(((Parameters*)parameters)->type); }, &parameters, timeoutMilliseconds))
	//			BLAZE_LOG_ERROR("Timeout of {}ms elapsed for executing a function on the main thread", timeoutMilliseconds);
	//	}
	//
	//	return cursor;
	//}
	Vec2f BlazeEngineContextInternal::GetMouseLastFramePos() const
	{
		return mouseLastFramePos;
	}
	Vec2f BlazeEngineContextInternal::GetMouseLastFrameMovementSum() const
	{
		return mouseLastFrameMovementSum;
	}
	Vec2f BlazeEngineContextInternal::GetMouseLastFrameScrollSum() const
	{
		return mouseLastFrameScrollSum;
	}
	Input::KeyFrameState BlazeEngineContextInternal::GetKeyFrameState(Input::Key key) const
	{
		bool down = downKeysDuringLastFrame[(uintMem)key];
		bool pressed = pressedKeysDuringLastFrame[(uintMem)key];
		bool released = releasedKeysDuringLastFrame[(uintMem)key];
		return Input::KeyFrameState{
			.pressed = pressed,
			.released = released,
			.down = down || pressed,
			.up = !down || released,
		};
	}
	Input::MouseButtonFrameState BlazeEngineContextInternal::GetMouseButtonFrameState(Input::MouseButton button) const
	{
		bool down = downMouseButtonsDuringLastFrame[(uintMem)button];
		bool pressed = pressedMouseButtonsDuringCurrentFrame[(uintMem)button];
		bool released = releasedMouseButtonsDuringCurrentUpdate[(uintMem)button];
		return Input::MouseButtonFrameState{
			.combo = mouseButtonsComboDuringLastFrame[(uintMem)button],
			.pressed = pressed,
			.released = released,
			.down = down || pressed,
			.up = !down || released,
		};
	}
	bool BlazeEngineContextInternal::GetKeyState(Input::Key key) const
	{
		return keysCurrentState[static_cast<uintMem>(key)].test();
	}
	bool BlazeEngineContextInternal::GetMouseButtonState(Input::MouseButton button) const
	{
		return mouseButtonsCurrentState[static_cast<uintMem>(button)].test();
	}
	void BlazeEngineContextInternal::ProcessEvents()
	{
		CopyInputState();

		ProcessEventQueue();

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ProcessEventQueue();
		}

		ProcessEventQueue();
	}
	void BlazeEngineContextInternal::ResetInputState()
	{
		std::lock_guard lg{ inputStateMutex };

		mouseCurrentPos = Vec2f();
		mouseCurrentFrameMovementSum = Vec2f();
		mouseCurrentScrollSum = Vec2f();
		memset(mouseButtonsComboDuringCurrentFrame, 0, sizeof(mouseButtonsComboDuringCurrentFrame));
		pressedMouseButtonsDuringCurrentFrame.reset();
		releasedMouseButtonsDuringCurrentUpdate.reset();
		downMouseButtonsDuringCurrentFrame.reset();
		pressedKeysDuringCurrentFrame.reset();
		releasedKeysDuringCurrentFrame.reset();
		downKeysDuringCurrentFrame.reset();
	}
	void BlazeEngineContextInternal::CopyInputState()
	{
		std::lock_guard lg{ inputStateMutex };

		pressedKeysDuringLastFrame = pressedKeysDuringCurrentFrame;
		releasedKeysDuringLastFrame = releasedKeysDuringCurrentFrame;
		downKeysDuringLastFrame = downKeysDuringCurrentFrame;
		memcpy(mouseButtonsComboDuringLastFrame, mouseButtonsComboDuringCurrentFrame, sizeof(mouseButtonsComboDuringLastFrame));
		pressedMouseButtonsDuringLastFrame = pressedMouseButtonsDuringCurrentFrame;
		releasedMouseButtonsDuringLastUpdate = releasedMouseButtonsDuringCurrentUpdate;
		downMouseButtonsDuringLastFrame = downMouseButtonsDuringCurrentFrame;
		mouseLastFramePos = mouseCurrentPos;
		mouseLastFrameMovementSum = mouseCurrentFrameMovementSum;
		mouseLastFrameScrollSum = mouseLastFrameScrollSum;

		releasedKeysDuringCurrentFrame.reset();
		pressedKeysDuringCurrentFrame.reset();
		downKeysDuringCurrentFrame.reset();
		releasedMouseButtonsDuringCurrentUpdate.reset();
		pressedMouseButtonsDuringCurrentFrame.reset();
		downMouseButtonsDuringCurrentFrame.reset();
		mouseCurrentFrameMovementSum = Vec2f();
		mouseCurrentScrollSum = Vec2f();
	}
	//bool BlazeEngineContextInternal::ExecuteOnMainThread(void(*function)(void*), void* data, uint64 timeoutMilliseconds) const
	//{
	//	if (separateClientThreadMode)
	//	{
	//		struct Parameters
	//		{
	//			void(*function)(void*);
	//			void* data;
	//
	//			bool finished;
	//			std::mutex mutex;
	//			std::condition_variable cv;
	//		} parameters{ function, data, false };
	//
	//		auto tempFunc = [](void* _parameters) {
	//			Parameters* parameters = (Parameters*)_parameters;
	//
	//			parameters->function(parameters->data);
	//
	//			std::unique_lock lk{ parameters->mutex };
	//			parameters->finished = true;
	//			parameters->cv.notify_all();
	//			};
	//
	//		SDL_Event event;
	//		SDL_zero(event);
	//		event.type = mainThreadSDLEventIdentifier;
	//		event.user.data1 = (void(*)(void*))tempFunc;
	//		event.user.data2 = &parameters;
	//
	//		std::unique_lock lk{ parameters.mutex };
	//		SDL_PushEvent(&event);
	//		return parameters.cv.wait_for(lk, std::chrono::milliseconds(timeoutMilliseconds), [&]() {
	//			return parameters.finished;
	//			});
	//	}
	//	else
	//	{
	//		function(data);
	//		return true;
	//	}
	//}
	BlazeEngineContextInternal* BlazeEngineContextInternal::GetEngineContext()
	{
		return blazeEngineContext;
	}
	void BlazeEngineContextInternal::ProcessEventQueue()
	{
		GlobalGenericEvent event;
		while (eventQueue.GetEvent(event))
		{
			switch (event.GetValueType())
			{
			case GlobalGenericEvent::GetValueTypeOf<Display::DisplayEvent>(): {
				event.TryProcess<Display::DisplayEvent>([&](auto& event) {
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

			{
				std::lock_guard lg{ inputStateMutex };
				switch (event->wheel.direction)
				{
				case SDL_MOUSEWHEEL_NORMAL:
					mouseCurrentScrollSum += Vec2f(event->wheel.x, event->wheel.y);
					break;
				case SDL_MOUSEWHEEL_FLIPPED:
					mouseCurrentScrollSum -= Vec2f(event->wheel.x, event->wheel.y);
					break;
				}
			}

			Input::MouseScrollEvent _event{
				.window = *window,
				.timeNS = event->wheel.timestamp,
				.mouseID = Input::MouseID(event->wheel.which),
				.amount = { event->wheel.x, event->wheel.y },
				.direction = event->wheel.direction == SDL_MOUSEWHEEL_NORMAL ? Input::ScrollDirection::Normal : Input::ScrollDirection::Flipped,
				.pos = Vec2f(event->wheel.mouse_x, event->wheel.mouse_y)
			};

			window->AddInputEvent(_event);
			break;
		}
		case SDL_EVENT_MOUSE_MOTION: {
			Window* window = GetWindowFromWindowID(event->motion.windowID);

			if (window == nullptr)
				break;

			{
				std::lock_guard lg{ inputStateMutex };
				mouseCurrentFrameMovementSum += Vec2f(event->motion.xrel, event->motion.yrel);
			}

			Input::MouseMotionEvent _event{
				.window = *window,
				.timeNS = event->motion.timestamp,
				.mouseID = Input::MouseID(event->motion.which),
				.delta = { event->motion.xrel, -event->motion.yrel },
				.pos = Vec2f(event->motion.x, event->motion.y)
			};

			window->AddInputEvent(_event);
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_DOWN: {
			Window* window = GetWindowFromWindowID(event->button.windowID);

			if (window == nullptr)
				break;

			auto button = SDL_GetBlazeMouseButton(event->button.button);

			{
				std::lock_guard lg{ inputStateMutex };
				downMouseButtonsDuringCurrentFrame.set((uintMem)button);
				mouseButtonsComboDuringCurrentFrame[(uintMem)button] = std::max(mouseButtonsComboDuringCurrentFrame[(uintMem)button], event->button.clicks);
				pressedMouseButtonsDuringCurrentFrame.set((uintMem)button);
			}
			mouseButtonsCurrentState[static_cast<uintMem>(button)].test_and_set();

			Input::MouseButtonDownEvent _event{
				.window = *window,
				.timeNS = event->button.timestamp,
				.mouseID = Input::MouseID(event->button.which),
				.button = button,
				.combo = event->button.clicks,
				.pos = Vec2f(event->button.x, event->button.y)
			};

			window->AddInputEvent(_event);
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_UP: {
			Window* window = GetWindowFromWindowID(event->button.windowID);

			if (window == nullptr)
				break;

			auto button = SDL_GetBlazeMouseButton(event->button.button);

			{
				std::lock_guard lg{ inputStateMutex };
				downMouseButtonsDuringCurrentFrame.reset((uintMem)button);
				releasedMouseButtonsDuringCurrentUpdate.set((uintMem)button);
			}
			mouseButtonsCurrentState[static_cast<uintMem>(button)].clear();

			Input::MouseButtonUpEvent _event{
				.window = *window,
				.timeNS = event->button.timestamp,
				.mouseID = Input::MouseID(event->button.which),
				.button = button,
				.combo = event->button.clicks,
				.pos = Vec2f(event->button.x, event->button.y)
			};

			window->AddInputEvent(_event);
			break;
		}
		case SDL_EVENT_KEY_DOWN: {
			Window* window = GetWindowFromWindowID(event->key.windowID);

			if (window == nullptr)
				break;

			{
				std::lock_guard lg{ inputStateMutex };

				downKeysDuringCurrentFrame.set(event->key.scancode);
				pressedKeysDuringCurrentFrame.set(event->key.scancode);
			}
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

			{
				std::lock_guard lg{ inputStateMutex };

				releasedKeysDuringCurrentFrame.set(event->key.scancode);
				downKeysDuringCurrentFrame.reset(event->key.scancode);
			}
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

			ResetInputState();

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

			Window::MouseEnterEvent _event{
				.window = *window,
				.timeNS = event->window.timestamp,
			};

			window->AddWindowEvent(_event);
			break;
		}
		case SDL_EVENT_WINDOW_MOUSE_LEAVE: {
			Window* window = GetWindowFromWindowID(event->window.windowID);

			if (window == nullptr)
				break;

			Window::MouseLeaveEvent _event{
				.window = *window,
				.timeNS = event->window.timestamp,
			};

			window->AddWindowEvent(_event);
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