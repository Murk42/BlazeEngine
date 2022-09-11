#include "BlazeEngine/Input/Input.h"
#include "BlazeEngine/Core/Window.h"
#include "BlazeEngine/Console/Console.h"
#include "BlazeEngine/Logging/Logger.h"
#include "source/BlazeEngine/Internal/InternalKeyStateData.h"
#include "BlazeEngine/Core/Startup.h"
#include "BlazeEngine/Event/EventDispatcher.h"
#include "BlazeEngine/Event/EventHandler.h"
#include "BlazeEngine/Event/Events.h"
#include <map>

#include "SDL/SDL.h"

#include "BlazeEngine/Utilities/Time.h"

namespace Blaze	
{			
	void* GetOpenGLInitWindow();
	const std::vector<Window*>& GetAllWindows();
	Key GetKeyFromScancode(SDL_Scancode);
	bool WasFirstWindowCreated();

	extern EventDispatcher<Event::KeyPressed		 >        keyPressedDispatcher;
	extern EventDispatcher<Event::KeyReleased		 >       keyReleasedDispatcher;
	extern EventDispatcher<Event::MousePressed	     >      mousePressedDispatcher;
	extern EventDispatcher<Event::MouseReleased	     >     mouseReleasedDispatcher;
	extern EventDispatcher<Event::MouseMotion	     >       mouseMotionDispatcher;
	extern EventDispatcher<Event::MouseScroll	     >       mouseScrollDispatcher;
	extern EventDispatcher<Event::MouseEnter	     >        mouseEnterDispatcher;
	extern EventDispatcher<Event::MouseLeave	     >        mouseLeaveDispatcher;
	extern EventDispatcher<Event::WindowResized	     >     windowResizedDispatcher;
	extern EventDispatcher<Event::WindowMoved		 >       windowMovedDispatcher;
	extern EventDispatcher<Event::WindowMinimized	 >   windowMinimizedDispatcher;
	extern EventDispatcher<Event::WindowMaximized	 >   windowMaximizedDispatcher;
	extern EventDispatcher<Event::WindowFocusGained  > windowFocusGainedDispatcher;
	extern EventDispatcher<Event::WindowFocusLost	 >   windowFocusLostDispatcher;
	extern EventDispatcher<Event::WindowClosed	     >      windowClosedDispatcher;
	extern EventDispatcher<Event::TextInput		     >         textInputDispatcher;
	extern EventDispatcher<Event::ViewportChanged    >   viewportChangedDispatcher;

	static Input::KeyState keyStates[(int)Key::KeyCount];
	static Input::KeyState mouseKeyStates[(int)MouseKey::KeyCount];

	static Key keyPressedArray[16];
	static size_t keyPressedArraySize = 0;
	static Key keyReleasedArray[16];
	static size_t keyReleasedArraySize = 0;

	static Vec2i mousePos;
	static Vec2i mouseMovement;
	static int mouseScroll = 0;
	static double doubleClickInterval = 0.2;

	static Window* focusedWindow = nullptr;

	Startup::InputInitInfo InitializeInput()
	{		
		Startup::InputInitInfo initInfo;
		TimePoint startTimePoint = TimePoint::GetWorldTime();

		SDL_GetGlobalMouseState(&mousePos.x, &mousePos.y);

		for (auto& keyState : keyStates)
		{
			keyState.time = 0;
			keyState.combo = 0;
			keyState.down = false;
			keyState.pressed = false;
			keyState.released = false;
		}
		for (auto& keyState : mouseKeyStates)
		{
			keyState.time = 0;
			keyState.combo = 0;
			keyState.down = false;
			keyState.pressed = false;
			keyState.released = false;
		}

		initInfo.initTime = TimePoint::GetWorldTime() - startTimePoint;
		return initInfo;
	}
	void TerminateInput()
	{

	}

	namespace Input
	{				
		void KeyPressed(Key key, float time)
		{
			KeyState& data = keyStates[(int)key];


			if (keyPressedArraySize < _countof(keyPressedArray))
			{
				keyPressedArray[keyPressedArraySize] = key;
				keyPressedArraySize++;				

				if (time - data.time < doubleClickInterval)
					++data.combo;
				else
					data.combo = 1;

				data.pressed = true;
				data.down = true;			

				keyPressedDispatcher.Call({ key, time, data.combo });
				data.time = time;
			}
			else			
				BLAZE_WARNING_LOG("Blaze Engine", "keyPressedArray got full, aborted keyPressed event.");
		}
		void KeyReleased(Key key, float time)
		{
			KeyState& data = keyStates[(int)key];			


			if (keyReleasedArraySize < _countof(keyReleasedArray))
			{
				keyReleasedArray[keyReleasedArraySize] = key;
				keyReleasedArraySize++;

				data.down = false;
				data.released = true;

				keyReleasedDispatcher.Call({ key, time - data.time });
			}
			else			
				BLAZE_WARNING_LOG("Blaze Engine", "keyReleasedArray got full, aborted keyReleased event.");			
		} 

		//Event detection
		void Update()
		{
			double frameTime = TimePoint::GetRunTime();						

			auto GetWindowFromSDLid = [](uint32 id) -> Window*
			{
				void* win = SDL_GetWindowFromID(id);
				const auto& windows = GetAllWindows();
				for (auto& w : windows)
					if (w->GetHandle() == win)
						return w;
				return nullptr;
			};

			for (size_t i = 0; i < keyPressedArraySize; ++i)			
				keyStates[(int)keyPressedArray[i]].pressed = false;						
			keyPressedArraySize = 0;

			for (size_t i = 0; i < keyReleasedArraySize; ++i)
				keyStates[(int)keyReleasedArray[i]].released = false;
			keyReleasedArraySize = 0;

			for (auto& state : mouseKeyStates)
				state.pressed = state.released = false;

			mouseScroll = 0;
			SDL_Event event;
			while (SDL_PollEvent(&event))
				switch (event.type)
				{
				case SDL_MOUSEWHEEL: {
					mouseScroll = event.wheel.y;
					mouseScrollDispatcher.Call({ event.wheel.y });
					break;
				}
				case SDL_MOUSEBUTTONDOWN: {
					MouseKey key = (MouseKey)(event.button.button - 1);

					if (frameTime - mouseKeyStates[(int)key].time < doubleClickInterval)
						++mouseKeyStates[(int)key].combo;
					else
						mouseKeyStates[(int)key].combo = 1;

					mouseKeyStates[(int)key].down = true;
					mouseKeyStates[(int)key].pressed = true;
					mouseKeyStates[(int)key].time = frameTime;

					if (focusedWindow != nullptr)
						mousePressedDispatcher.Call({ key, Vec2i(event.button.x, focusedWindow->GetSize().y - event.button.y) });
					break;
				}
				case SDL_MOUSEBUTTONUP: {
					MouseKey key = (MouseKey)(event.button.button - 1);

					mouseKeyStates[(int)key].down = false;
					mouseKeyStates[(int)key].released = true;

					if (focusedWindow != nullptr)
						mouseReleasedDispatcher.Call({ key, Vec2i(event.button.x, focusedWindow->GetSize().y - event.button.y) });
					break;
				}
				case SDL_KEYDOWN: {
					SDL_Scancode scancode = event.key.keysym.scancode;
										
					Key key = GetKeyFromScancode(scancode);
					if (event.key.repeat == 0)
						KeyPressed(key, frameTime);											

					break;
				}
				case SDL_KEYUP: {
					SDL_Scancode scancode = event.key.keysym.scancode;

					Key key = GetKeyFromScancode(scancode);
					KeyReleased(key, frameTime);
					break;
				}
				case SDL_TEXTINPUT: {					
					textInputDispatcher.Call({ StringUTF8((void*)event.text.text, strlen(event.text.text)) } );
					break;								
				}
				case SDL_WINDOWEVENT: {
					if (WasFirstWindowCreated())
						switch (event.window.event) {
						case SDL_WINDOWEVENT_MOVED: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							windowMovedDispatcher.Call({ Vec2i(event.window.data1, event.window.data2), win });
							break;
						}
						case SDL_WINDOWEVENT_MINIMIZED: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							windowMinimizedDispatcher.Call({ win });
							break;
						}
						case SDL_WINDOWEVENT_MAXIMIZED: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							windowMaximizedDispatcher.Call({ win });
							break;
						}
						case SDL_WINDOWEVENT_SIZE_CHANGED: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							windowResizedDispatcher.Call({ Vec2i(event.window.data1, event.window.data2), win });
							break;
						}
						case SDL_WINDOWEVENT_RESIZED: {
							/*
							Covered by SDL_WINDOWEVENT_SIZE_CHANGED
							*/
							//Window* win = GetWindowFromSDLid(event.window.windowID);
							//Event_WindowResized({ Vec2i(event.window.data1, event.window.data2), win });
							break;
						}
						case SDL_WINDOWEVENT_FOCUS_GAINED: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							focusedWindow = win;
							windowFocusGainedDispatcher.Call({ win });
							break;
						}
						case SDL_WINDOWEVENT_FOCUS_LOST: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							focusedWindow = nullptr;
							windowFocusLostDispatcher.Call({ win });
							break;
						}
						case SDL_WINDOWEVENT_CLOSE: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							windowClosedDispatcher.Call({ win });
							break;
						}
						case SDL_WINDOWEVENT_ENTER: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							mouseEnterDispatcher.Call({ });
							break;
						}
						case SDL_WINDOWEVENT_LEAVE: {
							Window* win = GetWindowFromSDLid(event.window.windowID);
							mouseLeaveDispatcher.Call({ });
							break;
						}
						}
				}
				}			
			
			Vec2i mouseRealPos;
			SDL_GetGlobalMouseState(&mouseRealPos.x, &mouseRealPos.y);			

			SDL_Rect rect;
			SDL_GetDisplayBounds(0, &rect);
			mouseRealPos.y = rect.h - mouseRealPos.y;

			SDL_GetRelativeMouseState(&mouseMovement.x, &mouseMovement.y);
			mouseMovement.y = -mouseMovement.y;

			if (focusedWindow != nullptr)
			{
				mousePos = mouseRealPos - focusedWindow->GetPos();
				//mousePos.y = focusedWindow->GetSize().y - mousePos.y;
			}
			else
			{ 
				mousePos = mouseRealPos;								
			}

			if (mouseMovement != Vec2i(0))
			{				
				mouseMotionDispatcher.Call({ Vec2i(mousePos.x, mousePos.y), Vec2i(mouseMovement.x, mouseMovement.y) });
			}
		}

		KeyState GetKeyState(Key code)
		{			
			return keyStates[(int)code];
		}

		KeyState GetKeyState(MouseKey code)
		{
			return mouseKeyStates[(int)code];
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
		double GetDoubleClickInterval()
		{
			return doubleClickInterval;
		}

		void ShowCursor(bool show)
		{
			SDL_ShowCursor(show);			
		}

		void LockCursor(bool lock)
		{
			SDL_SetRelativeMouseMode(lock ? SDL_TRUE : SDL_FALSE);
		}

		void SetDoubleClickInterval(double interval)
		{
			doubleClickInterval = interval;
		}
		void SetMousePos(Vec2i p)
		{
			if (focusedWindow != nullptr)
				SDL_WarpMouseInWindow((SDL_Window*)focusedWindow->GetHandle(), p.x, p.y);
			else
				SDL_WarpMouseGlobal(p.x, p.y);
			mousePos = p;
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