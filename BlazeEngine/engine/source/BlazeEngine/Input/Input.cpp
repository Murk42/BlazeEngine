#include "BlazeEngine/Input/Input.h"
#include "BlazeEngine/Core/Window.h"
#include "BlazeEngine/Core/Logger.h"
#include "source/BlazeEngine/Internal/Engine.h"
#include <map>

#include "SDL/SDL.h"

#include "BlazeEngine/Utilities/Time.h"

namespace Blaze
{
	extern void* initWindow;

#define CALL_EVENT_FUNC(name, args) \
	if (engine->Input.eventFunctions. name)	\
		engine->Input.eventFunctions. name args
	
	namespace Input
	{		
		void Update()
		{
			double currentTime = Time::GetRunTime();

			auto GetWindowFromSDLid = [](uint32 id) -> Window*
			{
				void* win = SDL_GetWindowFromID(id);
				for (auto& w : engine->App.allWindows)
					if (w->ptr == win)
						return w;
				return nullptr;
			};

			for (uint i = 0; i < (int)Key::Key_Count; ++i)
			{
				KeyStateData& data = engine->Input.keyStates[i];

				//the state of the key coresponds to its event				
				switch (data.state)
				{
				case KeyState::Down:
					CALL_EVENT_FUNC(keyDownFunc, ((Key)i, data.lastPress));					
					break;
				case KeyState::Released:
					CALL_EVENT_FUNC(keyReleasedFunc, ((Key)i, data.lastPress));
					break;
				case KeyState::Up:
					CALL_EVENT_FUNC(keyUpFunc, ((Key)i, data.lastPress));
					break;
				case KeyState::Pressed:
					CALL_EVENT_FUNC(keyPressedFunc, ((Key)i, data.lastPress, data.pressCount));
					break;
				}

				if (data.state == KeyState::Pressed)
					data.state = KeyState::Down;
				else if (data.state == KeyState::Released)
					data.state = KeyState::Up;
			}

			SDL_Event event;
			uint scancode;

			engine->Input.mouseScroll = 0;
			while (SDL_PollEvent(&event))
				switch (event.type)
				{
				case SDL_MOUSEWHEEL: {
					CALL_EVENT_FUNC(mouseWheelFunc, (event.wheel.y));
					engine->Input.mouseScroll += event.wheel.y;
					break;
				}
				case SDL_MOUSEBUTTONDOWN: {
					scancode = event.button.button - 1 + (int)Key::MouseLeft;
					KeyStateData& data = engine->Input.keyStates[scancode];

					if (data.state == KeyState::Up)
					{
						if (currentTime - data.lastPress < engine->Input.doubleClickInterval)						
							++data.pressCount;						
						else
						{							
							data.lastPress = currentTime;
							data.pressCount = 1;
						}

						data.state = KeyState::Pressed;
					}
					break;
				}
				case SDL_MOUSEBUTTONUP: {
					scancode = event.button.button - 1 + (int)Key::MouseLeft;
					KeyStateData& data = engine->Input.keyStates[scancode];

					if (data.state == KeyState::Down)
						data.state = KeyState::Released;
					break;
				}
				case SDL_KEYDOWN: {
					scancode = event.key.keysym.scancode;
					auto it = engine->Input.keymap.find(scancode);
					if (it != engine->Input.keymap.end())
					{
						Key key = it->second;
						KeyStateData& data = engine->Input.keyStates[(int)key];

						if (currentTime - data.lastPress < engine->Input.doubleClickInterval)
						{
							data.lastPress = currentTime;
							++data.pressCount;
						}
						else
						{
							data.lastPress = currentTime;
							data.pressCount = 1;
						}

						data.state = KeyState::Pressed;
					}
					break;
				}
				case SDL_KEYUP: {
					scancode = event.key.keysym.scancode;
					auto it = engine->Input.keymap.find(scancode);
					if (it != engine->Input.keymap.end())
					{
						Key key = it->second;
						KeyStateData& data = engine->Input.keyStates[(int)key];

						if (data.state == KeyState::Down)
							data.state = KeyState::Released;
					}
					break;
				}
				case SDL_WINDOWEVENT:
					if (engine->App.initWindow == nullptr)
						switch (event.window.event) {
						case SDL_WINDOWEVENT_MOVED: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							CALL_EVENT_FUNC(windowMovedFunc, (event.window.data1, event.window.data2, *win));
							break;
						}						
						case SDL_WINDOWEVENT_MINIMIZED: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							CALL_EVENT_FUNC(windowMinimizedFunc, (*win));
							break;
						}
						case SDL_WINDOWEVENT_MAXIMIZED: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							CALL_EVENT_FUNC(windowMaximizedFunc, (*win));
							break;
						}
						case SDL_WINDOWEVENT_SIZE_CHANGED: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							CALL_EVENT_FUNC(windowSizeChangedFunc, (event.window.data1, event.window.data2, *win));
							break;
						}
						case SDL_WINDOWEVENT_RESIZED: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							CALL_EVENT_FUNC(windowResizedFunc, (event.window.data1, event.window.data2, *win));
							break;
						}
						case SDL_WINDOWEVENT_FOCUS_GAINED: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							engine->Input.focusedWindow = win;
							CALL_EVENT_FUNC(windowFocusGainedFunc, (*win));
							break;
						}
						case SDL_WINDOWEVENT_FOCUS_LOST: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							engine->Input.focusedWindow = nullptr;
							CALL_EVENT_FUNC(windowFocusLostFunc, (*win));
							break;
						}
						case SDL_WINDOWEVENT_CLOSE: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							CALL_EVENT_FUNC(windowClosedFunc, (*win));
							break;
						}
						case SDL_WINDOWEVENT_ENTER: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							CALL_EVENT_FUNC(mouseEnterFunc, (*win));
							break;
						}
						case SDL_WINDOWEVENT_LEAVE: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							CALL_EVENT_FUNC(mouseLeaveFunc, (*win));
							break;
						}
						}
				}

				{
					Vec2i mouseRealPos;
					SDL_GetGlobalMouseState(&mouseRealPos.x, &mouseRealPos.y);

					if (engine->Input.focusedWindow != nullptr && engine->Input.focusedWindow->lockedMouse)
					{
						Vec2i wpos = engine->Input.focusedWindow->GetPos();
						if (engine->Input.mousePos == engine->Input.focusedWindow->lockedMousePos + wpos)
							engine->Input.mouseMovement = Vec2i(0);
						else
						{
							engine->Input.mouseMovement = mouseRealPos - (engine->Input.focusedWindow->lockedMousePos + wpos);
							SetMousePos(engine->Input.focusedWindow->lockedMousePos);
						}
					}
					else
					{
						engine->Input.mouseMovement = mouseRealPos - engine->Input.lastMouseRealPos;
						engine->Input.lastMouseRealPos = mouseRealPos;

						if (engine->Input.focusedWindow != nullptr)
							engine->Input.mousePos = mouseRealPos - engine->Input.focusedWindow->GetPos();
						else
							engine->Input.mousePos = mouseRealPos;
					}

					static bool first = true;
					if (first)
						engine->Input.mouseMovement = Vec2i(0);
					first = false;

					if (engine->Input.mouseMovement != Vec2i(0))
						CALL_EVENT_FUNC(mouseMotionFunc, (engine->Input.mouseMovement.x, engine->Input.mouseMovement.y));
				}
		}

		const KeyStateData GetKeyState(Key code)
		{
			return engine->Input.keyStates[(int)code];
		}		

		void SetEventFunction(void* func, InputEvent event)
		{
			switch (event)
			{
			case Blaze::InputEvent::KeyUp:				engine->Input.eventFunctions.keyUpFunc				= *(std::function<void(Key, double)>*)			func; break;
			case Blaze::InputEvent::KeyDown:			engine->Input.eventFunctions.keyDownFunc			= *(std::function<void(Key, double)>*)			func; break;
			case Blaze::InputEvent::KeyPressed:			engine->Input.eventFunctions.keyPressedFunc			= *(std::function<void(Key, double, uint)>*)	func; break;
			case Blaze::InputEvent::KeyReleased:		engine->Input.eventFunctions.keyReleasedFunc		= *(std::function<void(Key, double)>*)			func; break;
			case Blaze::InputEvent::MouseMotion:		engine->Input.eventFunctions.mouseMotionFunc		= *(std::function<void(int, int)>*)				func; break;
			case Blaze::InputEvent::MouseWheel:			engine->Input.eventFunctions.mouseWheelFunc			= *(std::function<void(int)>*)					func; break;
			case Blaze::InputEvent::MouseEnter:			engine->Input.eventFunctions.mouseEnterFunc			= *(std::function<void(Window&)>*)				func; break;
			case Blaze::InputEvent::MouseLeave:			engine->Input.eventFunctions.mouseLeaveFunc			= *(std::function<void(Window&)>*)				func; break;
			case Blaze::InputEvent::WindowSizeChanged:	engine->Input.eventFunctions.windowSizeChangedFunc	= *(std::function<void(int, int, Window&)>*)	func; break;
			case Blaze::InputEvent::WindowResized:		engine->Input.eventFunctions.windowResizedFunc		= *(std::function<void(int, int, Window&)>*)	func; break;
			case Blaze::InputEvent::WindowMoved:		engine->Input.eventFunctions.windowMovedFunc		= *(std::function<void(int, int, Window&)>*)	func; break;
			case Blaze::InputEvent::WindowMinimized:	engine->Input.eventFunctions.windowMinimizedFunc	= *(std::function<void(Window&)>*)				func; break;
			case Blaze::InputEvent::WindowMaximized:	engine->Input.eventFunctions.windowMaximizedFunc	= *(std::function<void(Window&)>*)				func; break;
			case Blaze::InputEvent::WindowFocusGained:	engine->Input.eventFunctions.windowFocusGainedFunc	= *(std::function<void(Window&)>*)				func; break;
			case Blaze::InputEvent::WindowFocusLost:	engine->Input.eventFunctions.windowFocusLostFunc	= *(std::function<void(Window&)>*)				func; break;
			case Blaze::InputEvent::WindowClosed:		engine->Input.eventFunctions.windowClosedFunc		= *(std::function<void(Window&)>*)				func; break;			
			}
		}

		int GetMouseScroll()
		{
			return engine->Input.mouseScroll;
		}
		Vec2i GetMousePos()
		{
			return engine->Input.mousePos;
		}
		Vec2i GetMouseMovement()
		{
			return engine->Input.mouseMovement;
		}
		Window* GetFocusedWindow()
		{
			return engine->Input.focusedWindow;
		}
		double GetDoubleClickInterval()
		{
			return engine->Input.doubleClickInterval;
		}

		void SetDoubleClickInterval(double interval)
		{
			engine->Input.doubleClickInterval = interval;
		}
		void SetMousePos(const Vec2i& p)
		{
			if (engine->Input.focusedWindow != nullptr)
				SDL_WarpMouseInWindow((SDL_Window*)engine->Input.focusedWindow->ptr, p.x, p.y);
			else
				SDL_WarpMouseGlobal(p.x, p.y);
			engine->Input.mousePos = p;
		}
	}
}