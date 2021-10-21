#include "BlazeEngine/Input/Input.h"
#include "BlazeEngine/Core/Window.h"
#include "BlazeEngine/Core/Logger.h"
#include "source/BlazeEngine/Internal/Engine.h"
#include "source/BlazeEngine/Internal/InternalKeyStateData.h"
#include <map>

#include "SDL/SDL.h"

#include "BlazeEngine/Utilities/Time.h"

namespace Blaze
{
	extern void* initWindow;

#define CALL_EVENT_FUNC(name, args) \
	if (engine->Input_ name)	\
		engine->Input_ name args
	
	namespace Input
	{				
		void KeyPressed(Key key, float time)
		{
			KeyState& data = engine->Input.keyStates[(int)key];


			if (engine->Input.keyPressedArraySize < _countof(engine->Input.keyPressedArray))
			{
				engine->Input.keyPressedArray[engine->Input.keyPressedArraySize] = key;
				engine->Input.keyPressedArraySize++;				

				if (time - data.timePressed < engine->Input.doubleClickInterval)
					++data.combo;
				else
					data.combo = 1;

				data.pressed = true;
				data.down = true;			

				engine->Input_KeyPressed({ key, time, data.combo });
				data.timePressed = time;
			}
			else			
				BLAZE_WARNING_LOG("Blaze Engine", "engine->Input.keyPressedArray got full, aborted keyPressed event.");
		}
		void KeyReleased(Key key, float time)
		{
			KeyState& data = engine->Input.keyStates[(int)key];			


			if (engine->Input.keyReleasedArraySize < _countof(engine->Input.keyReleasedArray))
			{
				engine->Input.keyReleasedArray[engine->Input.keyReleasedArraySize] = key;
				engine->Input.keyReleasedArraySize++;

				data.down = false;
				data.released = true;

				engine->Input_KeyReleased({ key, time - data.timePressed });
			}
			else			
				BLAZE_WARNING_LOG("Blaze Engine", "engine->Input.keyReleasedArray got full, aborted keyReleased event.");			
		} 

		//Event detection
		void Update()
		{
			double frameTime = Time::GetRunTime().GetValue();			

			SDL_Rect rect;
			rect.x = 50;
			rect.y = 50;
			rect.w = 50;
			rect.h = 50;
			SDL_SetTextInputRect(&rect);

			auto GetWindowFromSDLid = [](uint32 id) -> Window*
			{
				void* win = SDL_GetWindowFromID(id);
				for (auto& w : engine->App.allWindows)
					if (w->GetHandle() == win)
						return w;
				return nullptr;
			};

			for (size_t i = 0; i < engine->Input.keyPressedArraySize; ++i)			
				engine->Input.keyStates[(int)engine->Input.keyPressedArray[i]].pressed = false;						
			engine->Input.keyPressedArraySize = 0;

			for (size_t i = 0; i < engine->Input.keyReleasedArraySize; ++i)
				engine->Input.keyStates[(int)engine->Input.keyReleasedArray[i]].released = false;
			engine->Input.keyReleasedArraySize = 0;

			for (auto& state : engine->Input.mouseKeyStates)
				state.pressed = state.released = false;

			SDL_Event event;
			while (SDL_PollEvent(&event))
				switch (event.type)
				{
				case SDL_MOUSEWHEEL: {
					engine->Input.mouseScroll = event.wheel.y;
					engine->Input_MouseScroll({ event.wheel.y });
					break;
				}
				case SDL_MOUSEBUTTONDOWN: {
					MouseKey key = (MouseKey)(event.button.button - 1);

					if (frameTime - engine->Input.mouseKeyStates[(int)key].timePressed < engine->Input.doubleClickInterval)
						++engine->Input.mouseKeyStates[(int)key].combo;
					else
						engine->Input.mouseKeyStates[(int)key].combo = 1;

					engine->Input.mouseKeyStates[(int)key].down = true;
					engine->Input.mouseKeyStates[(int)key].pressed = true;
					engine->Input.mouseKeyStates[(int)key].timePressed = frameTime;

					if (engine->Input.focusedWindow != nullptr)
						engine->Input_MousePressed({ key, Vec2i(event.button.x, engine->Input.focusedWindow->GetSize().y - event.button.y) });
					break;
				}
				case SDL_MOUSEBUTTONUP: {
					MouseKey key = (MouseKey)(event.button.button - 1);

					engine->Input.mouseKeyStates[(int)key].down = false;
					engine->Input.mouseKeyStates[(int)key].released = true;

					if (engine->Input.focusedWindow != nullptr)
						engine->Input_MouseReleased({ key, Vec2i(event.button.x, engine->Input.focusedWindow->GetSize().y - event.button.y) });
					break;
				}
				case SDL_KEYDOWN: {
					SDL_Scancode scancode = event.key.keysym.scancode;
					
					auto it = engine->Input.keymap.find(scancode);
					if (it != engine->Input.keymap.end())
					{
						Key key = it->second;
						if (event.key.repeat == 0)
							KeyPressed(key, frameTime);						
					}
					break;
				}
				case SDL_KEYUP: {
					SDL_Scancode scancode = event.key.keysym.scancode;

					auto it = engine->Input.keymap.find(scancode);
					if (it != engine->Input.keymap.end())
					{
						Key key = it->second;
						KeyReleased(key, frameTime);
					}
					break;
				}
				case SDL_TEXTINPUT: {										
					engine->Input_TextInput({ String((const char*)event.text.text, UTF8Size(event.text.text)) });					
					break;								
				}
				case SDL_WINDOWEVENT: {
					if (engine->App.initWindow == nullptr)
						switch (event.window.event) {
						case SDL_WINDOWEVENT_MOVED: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							engine->Input_WindowMoved({ Vec2i(event.window.data1, event.window.data2), win });
							break;
						}
						case SDL_WINDOWEVENT_MINIMIZED: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							engine->Input_WindowMinimized({ win });
							break;
						}
						case SDL_WINDOWEVENT_MAXIMIZED: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							engine->Input_WindowMaximized({ win });
							break;
						}
						case SDL_WINDOWEVENT_SIZE_CHANGED: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							engine->Input_WindowSizeChanged({ Vec2i(event.window.data1, event.window.data2), win });
							break;
						}
						case SDL_WINDOWEVENT_RESIZED: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							engine->Input_WindowResized({ Vec2i(event.window.data1, event.window.data2), win });
							break;
						}
						case SDL_WINDOWEVENT_FOCUS_GAINED: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							engine->Input.focusedWindow = win;
							engine->Input_WindowFocusGained({ win });
							break;
						}
						case SDL_WINDOWEVENT_FOCUS_LOST: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							engine->Input.focusedWindow = nullptr;
							engine->Input_WindowFocusLost({ win });
							break;
						}
						case SDL_WINDOWEVENT_CLOSE: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							engine->Input_WindowClosed({ win });
							break;
						}
						case SDL_WINDOWEVENT_ENTER: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							engine->Input_MouseEnter({ });
							break;
						}
						case SDL_WINDOWEVENT_LEAVE: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							engine->Input_MouseLeave({ });
							break;
						}
						}
				}
				}			
			
			Vec2i mouseRealPos;
			SDL_GetGlobalMouseState(&mouseRealPos.x, &mouseRealPos.y);			

			SDL_GetRelativeMouseState(&engine->Input.mouseMovement.x, &engine->Input.mouseMovement.y);
			engine->Input.mouseMovement.y = -engine->Input.mouseMovement.y;

			if (engine->Input.focusedWindow != nullptr)
			{
				engine->Input.mousePos = mouseRealPos - engine->Input.focusedWindow->GetPos();
				engine->Input.mousePos.y = engine->Input.focusedWindow->GetSize().y - engine->Input.mousePos.y;
			}
			else
				engine->Input.mousePos = mouseRealPos;

			if (engine->Input.mouseMovement != Vec2i(0))
				engine->Input_MouseMotion({ Vec2i(engine->Input.mousePos.x, engine->Input.mousePos.y), Vec2i(engine->Input.mouseMovement.x, engine->Input.mouseMovement.y) });
		}

		KeyState GetKeyState(Key code)
		{			
			return engine->Input.keyStates[(int)code];
		}

		KeyState GetKeyState(MouseKey code)
		{
			return engine->Input.mouseKeyStates[(int)code];
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

		void ShowCursor(bool show)
		{
			SDL_SetRelativeMouseMode(show ? SDL_TRUE : SDL_FALSE);
		}

		void LockCursor(bool lock)
		{
			SDL_SetRelativeMouseMode(lock ? SDL_TRUE : SDL_FALSE);
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
		void StartTextInput()
		{
			SDL_StartTextInput();
		}
		void StopTextInput()
		{
			SDL_StopTextInput();
		}				
	}
}