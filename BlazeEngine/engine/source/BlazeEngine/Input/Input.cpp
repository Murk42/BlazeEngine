#include "BlazeEngine/Input/Input.h"
#include "BlazeEngine/Core/Window.h"
#include "BlazeEngine/Core/Logger.h"
#include <map>

#define SDL_MAIN_HANDLED
#include "SDL/SDL.h"

#include "BlazeEngine/Utilities/Time.h"

namespace Blaze
{
	extern void* initWindow;
	
	namespace Input
	{
		extern std::map<uint, Key> keymap;		

		extern KeyState keyStates[];
		extern void* eventFunctions[];
		
		extern Vec2i mousePos;
		extern Vec2i lastMouseRealPos;
		extern Vec2i mouseMovement;
		extern int mouseScroll;
		
		extern Window* focusedWindow;
		
		inline void CallEventFunction(InputEvent event)
		{
			if (eventFunctions[(int)event] != nullptr)
				((void(*)())eventFunctions[(int)event])();
		}
		inline void CallEventFunction(InputEvent event, Key key)
		{
			if (eventFunctions[(int)event] != nullptr)
				((void(*)(Key))eventFunctions[(int)event])(key);
		}
		inline void CallEventFunction(InputEvent event, int x)
		{
			if (eventFunctions[(int)event] != nullptr)
				((void(*)(int))eventFunctions[(int)event])(x);
		}
		inline void CallEventFunction(InputEvent event, int x, int y)
		{
			if (eventFunctions[(int)event] != nullptr)
				((void(*)(int, int))eventFunctions[(int)event])(x, y);
		}
		inline void CallEventFunction(InputEvent event, Window* win)
		{
			if (eventFunctions[(int)event] != nullptr)
				((void(*)(Window*))eventFunctions[(int)event])(win);
		}
		inline void CallEventFunction(InputEvent event, int x, int y, Window* win)
		{
			if (eventFunctions[(int)event] != nullptr)
				((void(*)(int, int, Window*))eventFunctions[(int)event])(x, y, win);
		}



		void Update()
		{
			auto GetWindowFromSDLid = [](uint32 id) -> Window*
			{
				void* win = SDL_GetWindowFromID(id);
				for (auto& w : Window::allWindows)
					if (w->ptr == win)
						return w;
				return nullptr;
			};

			for (uint i = 0; i < (int)Key::Key_Count; ++i)
			{
				KeyState& state = keyStates[i];

				//the state of the key coresponds to its event				
				CallEventFunction((InputEvent)state, (Key)i);					

				if (state == KeyState::Pressed)
					state = KeyState::Down;
				else if (state == KeyState::Released)
					state = KeyState::Up;
			}

			SDL_Event event;			
			uint scancode;

			mouseScroll = 0;
			while (SDL_PollEvent(&event))
				switch (event.type)
				{
				case SDL_MOUSEWHEEL: {
					CallEventFunction(InputEvent::MouseWheel, event.wheel.y);					
					mouseScroll += event.wheel.y;
					break;
				}
				case SDL_MOUSEBUTTONDOWN: {
					scancode = event.button.button - 1 + (int)Key::MouseLeft;
					KeyState& state = keyStates[scancode];

					if (state == KeyState::Up)					
						state = KeyState::Pressed;											
					break;
				}
				case SDL_MOUSEBUTTONUP: {
					scancode = event.button.button - 1 + (int)Key::MouseLeft;					
					KeyState& state = keyStates[scancode];

					if (state == KeyState::Down)
						state = KeyState::Released;
					break;
				}
				case SDL_KEYDOWN: {
					scancode = event.key.keysym.scancode;
					auto it = keymap.find(scancode);
					if (it != keymap.end())
					{
						Key key = it->second;
						KeyState& state = keyStates[(int)key];

						if (state == KeyState::Up)
							state = KeyState::Pressed;
					}
					break;
				}
				case SDL_KEYUP: {
					scancode = event.key.keysym.scancode;
					auto it = keymap.find(scancode);
					if (it != keymap.end())
					{
						Key key = it->second;
						KeyState& state = keyStates[(int)key];

						if (state == KeyState::Down)
							state = KeyState::Released;
					}
					break;
				}
				case SDL_WINDOWEVENT:
					if (initWindow == nullptr)
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
							focusedWindow = win;
							CallEventFunction(InputEvent::WindowFocusGained, win);															
							break;
						}
						case SDL_WINDOWEVENT_FOCUS_LOST: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							focusedWindow = nullptr;
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

				if (focusedWindow != nullptr && focusedWindow->lockedMouse)
				{
					Vec2i wpos = focusedWindow->GetPos();
					if (mousePos == focusedWindow->lockedMousePos + wpos)
						mouseMovement = Vec2i(0);
					else
					{
						mouseMovement = mouseRealPos - (focusedWindow->lockedMousePos + wpos);
						SetMousePos(focusedWindow->lockedMousePos);
					}
				}
				else
				{
					mouseMovement = mouseRealPos - lastMouseRealPos;
					lastMouseRealPos = mouseRealPos;

					if (focusedWindow != nullptr)
						mousePos = mouseRealPos - focusedWindow->GetPos();
					else
						mousePos = mouseRealPos;
				}

				static bool first = true;
				if (first)
					mouseMovement = Vec2i(0);
				first = false;

				if (mouseMovement != Vec2i(0))
					CallEventFunction(InputEvent::MouseMotion, mouseMovement.x, mouseMovement.y);
			}
		}

		const KeyState GetKeyState(Key code)
		{
			return keyStates[(int)code];
		}
		
		void SetEventFunction(InputEvent event, void* function)
		{			
			eventFunctions[(int)event] = function;
		}				

		int GetMouseScroll() 
		{
			return mouseScroll;
		}
		Vec2i GetMousePos()
		{
			return mousePos;
		}
		Vec2i GetMouseMovement()
		{
			return mouseMovement;
		}
		Window* GetFocusedWindow()
		{
			return focusedWindow;
		}

		void SetMousePos(const Vec2i& p)
		{
			if (focusedWindow != nullptr)
				SDL_WarpMouseInWindow((SDL_Window*)focusedWindow->ptr, p.x, p.y);
			else
				SDL_WarpMouseGlobal(p.x, p.y);
			mousePos = p;
		}
	}
}