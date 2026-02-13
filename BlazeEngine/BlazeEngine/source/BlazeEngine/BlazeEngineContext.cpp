#include "pch.h"
#include "BlazeEngine/BlazeEngineContext.h"
#include "BlazeEngine/External/SDL/SDL.h"
#include "BlazeEngine/External/SDL/SDLConversions.h"
#include "BlazeEngine/Core/Console/Console.h"
#include "BlazeEngine/Window/Window.h"

#define IGNORE_EVENT_USER_EVENT_CODE 0

namespace Blaze
{
	BlazeEngineContext* blazeEngineContext;


	BlazeEngineContext::BlazeEngineContext(BlazeEngineContextInitializationParameters parameters) :
		initializationTimings(parameters.initializationTimings),
		mainThreadSDLEventIdentifier(parameters.mainThreadSDLEventIdentifier),
		separateClientThreadMode(parameters.separateClientThreadMode),
		nCmdShow(parameters.nCmdShow)
	{
		ResetInputState();

		SDL_AddEventWatch(SDLEventWatch, this);
	}
	BlazeEngineContext::~BlazeEngineContext()
	{
		for (auto& cursor : cursors)
			if (cursor != nullptr)
				SDL_DestroyCursor(cursor);

		SDL_RemoveEventWatch(SDLEventWatch, this);
	}
	void BlazeEngineContext::RegisterWindow(Window& window)
	{
		if (window.GetHandle() == nullptr)
			BLAZE_LOG_FATAL("Registering a null window");

		std::lock_guard lg{ windowsMutex };
		windows.Insert(window.GetHandle(), &window);
	}
	void BlazeEngineContext::UnregisterWindow(Window& window)
	{
		if (window.GetHandle() == nullptr)
			BLAZE_LOG_FATAL("Unregistering a null window");

		std::lock_guard lg{ windowsMutex };
		windows.Erase(window.GetHandle());
	}
	void BlazeEngineContext::MoveWindow(Window& window)
	{
		if (window.GetHandle() == nullptr)
			BLAZE_LOG_FATAL("Moving a null window");

		std::lock_guard lg{ windowsMutex };

		auto it = windows.Find(window.GetHandle());

		if (it.IsNull())
			BLAZE_LOG_FATAL("Window is not registered");

		it->value = &window;
	}
	Window* BlazeEngineContext::GetWindowFromHandle(void* handle)
	{
		if (handle == nullptr)
			return nullptr;

		std::lock_guard lg{ windowsMutex };

		auto it = windows.Find(handle);

		if (it.IsNull())
			return nullptr;

		return it->value;
	}
	void BlazeEngineContext::UpdateInputAndEvents()
	{
		CopyInputState();

		if (!separateClientThreadMode)
		{
			SDL_Event event;
			while (SDL_PollEvent(&event));
		}

		clientEventStack.ProcessEvents();
	}
	SDL_Cursor* BlazeEngineContext::GetSystemCursor(Input::CursorType type)
	{
		std::lock_guard lg{ cursorsMutex };

		auto& cursor = cursors[(uint)type];

		if (cursor == nullptr)
		{
			struct Parameters
			{
				SDL_SystemCursor type;
				SDL_Cursor** cursor;
			} parameters { GetSDLSystemCursor(type), &cursor };

			uint64 timeoutMilliseconds = 1000;
			if (!ExecuteOnMainThread([](void* parameters) { *((Parameters*)parameters)->cursor = SDL_CreateSystemCursor(((Parameters*)parameters)->type); }, &parameters, timeoutMilliseconds))
				BLAZE_LOG_ERROR("Timeout of {}ms elapsed for executing a function on the main thread", timeoutMilliseconds);
		}

		return cursor;
	}
	Vec2f BlazeEngineContext::GetMouseLastFramePos() const
	{
		return mouseLastFramePos;
	}
	Vec2f BlazeEngineContext::GetMouseLastFrameMovementSum() const
	{
		return mouseLastFrameMovementSum;
	}
	Vec2f BlazeEngineContext::GetMouseLastFrameScrollSum() const
	{
		return mouseLastFrameScrollSum;
	}
	Input::KeyFrameState BlazeEngineContext::GetKeyFrameState(Input::Key key) const
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
	Input::MouseButtonFrameState BlazeEngineContext::GetMouseButtonFrameState(Input::MouseButton button) const
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
	void BlazeEngineContext::ResetInputState()
	{
		std::lock_guard lg{ inputCurrentFrameMutex };

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
	void BlazeEngineContext::CopyInputState()
	{
		std::lock_guard lg{ inputCurrentFrameMutex };

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
		downKeysDuringCurrentFrame.reset();
		mouseCurrentFrameMovementSum = Vec2f();
		mouseCurrentScrollSum = Vec2f();
	}
	bool BlazeEngineContext::ExecuteOnMainThread(void(*function)(void*), void* data, uint64 timeoutMilliseconds)
	{
		if (separateClientThreadMode)
		{
			struct Parameters
			{
				void(*function)(void*);
				void* data;

				bool finished;
				std::mutex mutex;
				std::condition_variable cv;
			} parameters{ function, data, false };

			auto tempFunc = [](void* _parameters) {
				Parameters* parameters = (Parameters*)_parameters;

				parameters->function(parameters->data);

				std::unique_lock lk{ parameters->mutex };
				parameters->finished = true;
				parameters->cv.notify_all();
				};

			SDL_Event event;
			SDL_zero(event);
			event.type = mainThreadSDLEventIdentifier;
			event.user.data1 = (void(*)(void*))tempFunc;
			event.user.data2 = &parameters;

			std::unique_lock lk{ parameters.mutex };
			SDL_PushEvent(&event);
			return parameters.cv.wait_for(lk, std::chrono::milliseconds(timeoutMilliseconds), [&]() {
				return parameters.finished;
				});
		}
		else
		{
			function(data);
			return true;
		}
	}
	Window* BlazeEngineContext::GetWindowFromWindowID(SDL_WindowID id)
	{
		std::lock_guard lg{ windowsMutex };

		Window* out = nullptr;

		if (id == 0)
			return out;

		SDL_Window* handle = SDL_GetWindowFromID(id);

		if (handle == nullptr)
			return nullptr;

		auto it = windows.Find(handle);

		if (it.IsNull())
			return nullptr;

		return it->value;
	}

	bool BlazeEngineContext::SDLEventWatch(void* data, SDL_Event* event)
	{
		BlazeEngineContext* context = (BlazeEngineContext*)data;

		switch (event->type)
		{
		case SDL_EVENT_MOUSE_WHEEL: {
			Window* window = context->GetWindowFromWindowID(event->wheel.windowID);

			if (window == nullptr)
				break;

			{
				std::lock_guard lg{ context->inputCurrentFrameMutex };
				switch (event->wheel.direction)
				{
				case SDL_MOUSEWHEEL_NORMAL:
					context->mouseCurrentScrollSum += Vec2f(event->wheel.x, event->wheel .y);
					break;
				case SDL_MOUSEWHEEL_FLIPPED:
					context->mouseCurrentScrollSum -= Vec2f(event->wheel.x, event->wheel.y);
					break;
				}
			}

			Input::MouseScrollEvent _event{
				.window = window,
				.timeNS = event->wheel.timestamp,
				.mouseID = event->wheel.which,
				.amount = { event->wheel.x, event->wheel.y },
				.direction = event->wheel.direction == SDL_MOUSEWHEEL_NORMAL ? Input::ScrollDirection::Normal : Input::ScrollDirection::Flipped,
				.pos = Vec2f(event->wheel.mouse_x, event->wheel.mouse_y)
			};

			context->AddToClientEventStack(_event, &window->mouseScrollEventDispatcher);

			break;
		}
		case SDL_EVENT_MOUSE_MOTION: {
			Window* window = context->GetWindowFromWindowID(event->motion.windowID);

			if (window == nullptr)
				break;

			{
				std::lock_guard lg{ context->inputCurrentFrameMutex };
				context->mouseCurrentFrameMovementSum += Vec2f(event->motion.xrel, event->motion.yrel);
			}

			Input::MouseMotionEvent _event{
				.window = window,
				.timeNS = event->motion.timestamp,
				.mouseID = event->motion.which,
				.delta = { event->motion.xrel, -event->motion.yrel },
				.pos = Vec2f(event->motion.x, event->motion.y)
			};

			context->AddToClientEventStack(_event, &window->mouseMotionEventDispatcher);

			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_DOWN: {
			Window* window = context->GetWindowFromWindowID(event->button.windowID);

			if (window == nullptr)
				break;

			auto button = GetBlazeMouseButton(event->button.button);

			{
				std::lock_guard lg{ context->inputCurrentFrameMutex };
				context->downMouseButtonsDuringCurrentFrame.set((uintMem)button);
				context->mouseButtonsComboDuringCurrentFrame[(uintMem)button] = std::max(context->mouseButtonsComboDuringCurrentFrame[(uintMem)button], event->button.clicks);
				context->pressedMouseButtonsDuringCurrentFrame.set((uintMem)button);
			}

			Input::MouseButtonDownEvent _event{
				.window = window,
				.timeNS = event->button.timestamp,
				.mouseID = event->button.which,
				.button = button,
				.combo = event->button.clicks,
				.pos = Vec2f(event->button.x, event->button.y)
			};

			context->AddToClientEventStack(_event, &window->mouseButtonDownEventDispatcher);
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_UP: {
			Window* window = context->GetWindowFromWindowID(event->button.windowID);

			if (window == nullptr)
				break;

			auto button = GetBlazeMouseButton(event->button.button);

			{
				std::lock_guard lg{ context->inputCurrentFrameMutex };
				context->downMouseButtonsDuringCurrentFrame.reset((uintMem)button);
				context->releasedMouseButtonsDuringCurrentUpdate.set((uintMem)button);
			}

			Input::MouseButtonUpEvent _event{
				.window = window,
				.timeNS = event->button.timestamp,
				.mouseID = event->button.which,
				.button = button,
				.combo = event->button.clicks,
				.pos = Vec2f(event->button.x, event->button.y)
			};

			context->AddToClientEventStack(_event, &window->mouseButtonUpEventDispatcher);
			break;
		}
		case SDL_EVENT_KEY_DOWN: {
			Window* window = context->GetWindowFromWindowID(event->key.windowID);

			if (window == nullptr)
				break;

			{
				std::lock_guard lg{ context->inputCurrentFrameMutex };

				context->downKeysDuringCurrentFrame.set(event->key.scancode);
				context->pressedKeysDuringCurrentFrame.set(event->key.scancode);
			}

			Input::KeyDownEvent _event{
				.window = window,
				.timeNS = event->key.timestamp,
				.keyboardID = event->key.which,
				.key = (Input::Key)event->key.scancode,
				.modifier = (Input::KeyModifier)event->key.mod,
				.keyCode = (Input::KeyCode)event->key.key,
				.repeat = event->key.repeat
			};

			context->AddToClientEventStack(_event, &window->keyDownEventDispatcher);
			break;
		}
		case SDL_EVENT_KEY_UP: {
			Window* window = context->GetWindowFromWindowID(event->key.windowID);

			if (window == nullptr)
				break;

			Input::KeyUpEvent _event{
				.window = window,
				.timeNS = event->key.timestamp,
				.keyboardID = event->key.which,
				.key = (Input::Key)event->key.scancode,
				.modifier = (Input::KeyModifier)event->key.mod,
				.keyCode = (Input::KeyCode)event->key.key,
			};

			{
				std::lock_guard lg{ context->inputCurrentFrameMutex };

				context->releasedKeysDuringCurrentFrame.set(event->key.scancode);
				context->downKeysDuringCurrentFrame.reset(event->key.scancode);
			}

			context->AddToClientEventStack(_event, &window->keyUpEventDispatcher);
			break;
		}
		case SDL_EVENT_TEXT_INPUT: {
			Window* window = context->GetWindowFromWindowID(event->text.windowID);

			if (window == nullptr)
				break;

			Input::TextInputEvent _event{
				.window = window,
				.timeNS = event->text.timestamp,
				.string = u8String(event->text.text, strlen(event->text.text))
			};

			context->AddToClientEventStack(_event, &window->textInputEventDispatcher);

			break;
		}
		case SDL_EVENT_DISPLAY_ORIENTATION: {
			Display::DisplayEvent _event{
				.displayID = (Display::DisplayID)event->display.displayID,
				.timeNS = event->display.timestamp,
			};

			context->AddToClientEventStack(_event, &context->displayEventDispatcher);
			break;
		}
		case SDL_EVENT_DISPLAY_ADDED: {
			Display::DisplayEvent _event{
				.displayID = (Display::DisplayID)event->display.displayID,
				.timeNS = event->display.timestamp,
			};

			context->AddToClientEventStack(_event, &context->displayEventDispatcher);
			break;
		}
		case SDL_EVENT_DISPLAY_REMOVED: {
			Display::DisplayEvent _event{
				.displayID = (Display::DisplayID)event->display.displayID,
				.timeNS = event->display.timestamp,
			};

			context->AddToClientEventStack(_event, &context->displayEventDispatcher);
			break;
		}
		case SDL_EVENT_DISPLAY_MOVED: {
			Display::DisplayEvent _event{
				.displayID = (Display::DisplayID)event->display.displayID,
				.timeNS = event->display.timestamp
			};

			context->AddToClientEventStack(_event, &context->displayEventDispatcher);
			break;
		}
		case SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED: {
			Display::DisplayEvent _event{
				.displayID = (Display::DisplayID)event->display.displayID,
				.timeNS = event->display.timestamp,
			};

			context->AddToClientEventStack(_event, &context->displayEventDispatcher);
			break;
		}
		case SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED: {
			Display::DisplayEvent _event{
				.displayID = (Display::DisplayID)event->display.displayID,
				.timeNS = event->display.timestamp
			};

			context->AddToClientEventStack(_event, &context->displayEventDispatcher);
			break;
		}
		case SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED: {
			Display::DisplayEvent _event{
				.displayID = (Display::DisplayID)event->display.displayID,
				.timeNS = event->display.timestamp
			};

			context->AddToClientEventStack(_event, &context->displayEventDispatcher);
			break;
		}
		case SDL_EVENT_WINDOW_MOVED: {
			Window* window = context->GetWindowFromWindowID(event->window.windowID);

			if (window == nullptr)
				break;

			Window::WindowMovedEvent _event{
				.window = *window,
				.timeNS = event->window.timestamp,
				.pos = Vec2i(event->window.data1, event->window.data2)
			};

			context->AddToClientEventStack(_event, &window->windowMovedEventDispatcher);
			break;
		}
		case SDL_EVENT_WINDOW_MINIMIZED: {
			Window* window = context->GetWindowFromWindowID(event->window.windowID);

			if (window == nullptr)
				break;

			Window::WindowMinimizedEvent _event{
				.window = *window,
				.timeNS = event->window.timestamp,
			};

			context->AddToClientEventStack(_event, &window->windowMinimizedEventDispatcher);
			break;
		}
		case SDL_EVENT_WINDOW_MAXIMIZED: {
			Window* window = context->GetWindowFromWindowID(event->window.windowID);

			if (window == nullptr)
				break;

			Window::WindowMaximizedEvent _event{
				.window = *window,
				.timeNS = event->window.timestamp,
			};

			context->AddToClientEventStack(_event, &window->windowMaximizedEventDispatcher);
			break;
		}
		case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: {
			Window* window = context->GetWindowFromWindowID(event->window.windowID);

			if (window == nullptr)
				break;

			Window::WindowResizedEvent _event{
				.window = *window,
				.timeNS = event->window.timestamp,
				.size = Vec2u(event->window.data1, event->window.data2)
			};

			context->AddToClientEventStack(_event, &window->windowResizedEventDispatcher);
			break;
		}
		case SDL_EVENT_WINDOW_FOCUS_GAINED: {
			Window* window = context->GetWindowFromWindowID(event->window.windowID);

			if (window == nullptr)
				break;

			Window::WindowFocusGainedEvent _event{
				.window = *window,
				.timeNS = event->window.timestamp,
			};

			SDL_StartTextInput((SDL_Window*)window->GetHandle());

			context->AddToClientEventStack(_event, &window->windowFocusGainedEventDispatcher);

			break;
		}
		case SDL_EVENT_WINDOW_FOCUS_LOST: {
			Window* window = context->GetWindowFromWindowID(event->window.windowID);

			if (window == nullptr)
				break;

			context->ResetInputState();

			Window::WindowFocusLostEvent _event{
				.window = *window,
				.timeNS = event->window.timestamp,
			};

			context->AddToClientEventStack(_event, &window->windowFocusLostEventDispatcher);

			SDL_StopTextInput((SDL_Window*)window->GetHandle());
			break;
		}
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
			Window* window = context->GetWindowFromWindowID(event->window.windowID);

			if (window == nullptr)
				break;

			Window::WindowCloseEvent _event{
				.window = *window,
				.timeNS = event->window.timestamp,
			};

			context->AddToClientEventStack(_event, &window->windowCloseEventDispatcher);

			break;
		}
		case SDL_EVENT_WINDOW_MOUSE_ENTER: {
			Window* window = context->GetWindowFromWindowID(event->window.windowID);

			if (window == nullptr)
				break;

			Window::WindowMouseEnterEvent _event{
				.window = *window,
				.timeNS = event->window.timestamp,
			};

			context->AddToClientEventStack(_event, &window->windowMouseEnterEventDispatcher);

			break;
		}
		case SDL_EVENT_WINDOW_MOUSE_LEAVE: {
			Window* window = context->GetWindowFromWindowID(event->window.windowID);

			if (window == nullptr)
				break;

			Window::WindowMouseLeaveEvent _event{
				.window = *window,
				.timeNS = event->window.timestamp,
			};

			context->AddToClientEventStack(_event, &window->windowMouseLeaveEventDispatcher);

			break;
		}
		case SDL_EVENT_WINDOW_DISPLAY_CHANGED: {
			Window* window = context->GetWindowFromWindowID(event->window.windowID);

			if (window == nullptr)
				break;

			Window::WindowDisplayChangedEvent _event{
				.window = *window,
				.timeNS = event->window.timestamp,
				.displayID = static_cast<uint32>(event->window.data1)
			};

			context->AddToClientEventStack(_event, &window->windowDisplayChangedEventDispatcher);
			break;
		}
		}

		//The return value is ignored when this function is used as an event watcher
		return false;
	}

}