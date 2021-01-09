#include "BlazeEngine/Input/Input.h"
#include "BlazeEngine/Core/Window.h"
#include "BlazeEngine/Core/Logger.h"
#include "Engine.h"
#include <map>

#define SDL_MAIN_HANDLED
#include "SDL/SDL.h"

#include "BlazeEngine/Utilities/Time.h"

namespace Blaze
{
	extern void* initWindow;
	
	namespace Input
	{
		
		inline void CallEventFunction(InputEvent event)
		{
			if (engine->Input.eventFunctions[(int)event] != nullptr)
				((void(*)())engine->Input.eventFunctions[(int)event])();
		}
		inline void CallEventFunction(InputEvent event, Key key)
		{
			if (engine->Input.eventFunctions[(int)event] != nullptr)
				((void(*)(Key))engine->Input.eventFunctions[(int)event])(key);
		}
		inline void CallEventFunction(InputEvent event, int x)
		{
			if (engine->Input.eventFunctions[(int)event] != nullptr)
				((void(*)(int))engine->Input.eventFunctions[(int)event])(x);
		}
		inline void CallEventFunction(InputEvent event, int x, int y)
		{
			if (engine->Input.eventFunctions[(int)event] != nullptr)
				((void(*)(int, int))engine->Input.eventFunctions[(int)event])(x, y);
		}
		inline void CallEventFunction(InputEvent event, Window* win)
		{
			if (engine->Input.eventFunctions[(int)event] != nullptr)
				((void(*)(Window*))engine->Input.eventFunctions[(int)event])(win);
		}
		inline void CallEventFunction(InputEvent event, int x, int y, Window* win)
		{
			if (engine->Input.eventFunctions[(int)event] != nullptr)
				((void(*)(int, int, Window*))engine->Input.eventFunctions[(int)event])(x, y, win);
		}



		void Update()
		{
			auto GetWindowFromSDLid = [](uint32 id) -> Window*
			{
				void* win = SDL_GetWindowFromID(id);
				for (auto& w : engine->Application.allWindows)
					if (w->ptr == win)
						return w;
				return nullptr;
			};

			for (uint i = 0; i < (int)Key::Key_Count; ++i)
			{
				KeyState& state = engine->Input.keyStates[i];

				//the state of the key coresponds to its event				
				CallEventFunction((InputEvent)state, (Key)i);					

				if (state == KeyState::Pressed)
					state = KeyState::Down;
				else if (state == KeyState::Released)
					state = KeyState::Up;
			}

			SDL_Event event;			
			uint scancode;

			engine->Input.mouseScroll = 0;
			while (SDL_PollEvent(&event))
				switch (event.type)
				{
				case SDL_MOUSEWHEEL: {
					CallEventFunction(InputEvent::MouseWheel, event.wheel.y);					
					engine->Input.mouseScroll += event.wheel.y;
					break;
				}
				case SDL_MOUSEBUTTONDOWN: {
					scancode = event.button.button - 1 + (int)Key::MouseLeft;
					KeyState& state = engine->Input.keyStates[scancode];

					if (state == KeyState::Up)					
						state = KeyState::Pressed;											
					break;
				}
				case SDL_MOUSEBUTTONUP: {
					scancode = event.button.button - 1 + (int)Key::MouseLeft;					
					KeyState& state = engine->Input.keyStates[scancode];

					if (state == KeyState::Down)
						state = KeyState::Released;
					break;
				}
				case SDL_KEYDOWN: {
					scancode = event.key.keysym.scancode;
					auto it = engine->Input.keymap.find(scancode);
					if (it != engine->Input.keymap.end())
					{
						Key key = it->second;
						KeyState& state = engine->Input.keyStates[(int)key];

						if (state == KeyState::Up)
							state = KeyState::Pressed;
					}
					break;
				}
				case SDL_KEYUP: {
					scancode = event.key.keysym.scancode;
					auto it = engine->Input.keymap.find(scancode);
					if (it != engine->Input.keymap.end())
					{
						Key key = it->second;
						KeyState& state = engine->Input.keyStates[(int)key];

						if (state == KeyState::Down)
							state = KeyState::Released;
					}
					break;
				}
				case SDL_WINDOWEVENT:
					if (engine->Application.initWindow == nullptr)
						switch (event.window.event) {
						case SDL_WINDOWEVENT_MOVED: {	
							Window* win = GetWindowFromSDLid(event.window.windowID);
							CallEventFunction(InputEvent::WindowMoved, event.window.data1, event.window.data2, win);							
							break;
						}					
						case SDL_WINDOWEVENT_RESTORED: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							CallEventFunction(InputEvent::WindowSizeChanged, win->GetSize().x, win->GetSize().y, win);							
							break;
						}
						case SDL_WINDOWEVENT_MINIMIZED: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							CallEventFunction(InputEvent::WindowMinimized, win);									
							break;
						}
						case SDL_WINDOWEVENT_MAXIMIZED: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							CallEventFunction(InputEvent::WindowMaximized, win);														
							break;
						}
						case SDL_WINDOWEVENT_SIZE_CHANGED: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							CallEventFunction(InputEvent::WindowSizeChanged, event.window.data1, event.window.data2, win);							
							break;
						}
						case SDL_WINDOWEVENT_RESIZED: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							CallEventFunction(InputEvent::WindowResized, event.window.data1, event.window.data2, win);							
							break;
						}						
						case SDL_WINDOWEVENT_FOCUS_GAINED: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							engine->Input.focusedWindow = win;
							CallEventFunction(InputEvent::WindowFocusGained, win);															
							break;
						}
						case SDL_WINDOWEVENT_FOCUS_LOST: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							engine->Input.focusedWindow = nullptr;
							CallEventFunction(InputEvent::WindowFocusLost, win);
							break;
						}
						case SDL_WINDOWEVENT_CLOSE: {							
							Window* win = GetWindowFromSDLid(event.window.windowID);
							CallEventFunction(InputEvent::WindowClosed, win);
							break;
						}
						case SDL_WINDOWEVENT_ENTER: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							CallEventFunction(InputEvent::MouseEnter, win);
							break;
						}
						case SDL_WINDOWEVENT_LEAVE: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							CallEventFunction(InputEvent::MouseLeave, win);
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
					CallEventFunction(InputEvent::MouseMotion, engine->Input.mouseMovement.x, engine->Input.mouseMovement.y);
			}
		}

		const KeyState GetKeyState(Key code)
		{
			return engine->Input.keyStates[(int)code];
		}
		
		void SetEventFunction(InputEvent event, void* function)
		{			
			engine->Input.eventFunctions[(int)event] = function;
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