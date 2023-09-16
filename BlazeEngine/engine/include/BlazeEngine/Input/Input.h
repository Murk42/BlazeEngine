#pragma once
#include "BlazeEngine/Input/Key.h"
#include "BlazeEngine/Input/InputEvents.h"

namespace Blaze
{	
	class Window; 	

	namespace Input
	{
		struct InputEventSystem
		{
			EventDispatcher<Events::KeyPressed            > keyPressedDispatcher;
			EventDispatcher<Events::KeyReleased           > keyReleasedDispatcher;
			EventDispatcher<Events::MouseMotion           > mouseMotionDispatcher;
			EventDispatcher<Events::MouseScroll           > mouseScrollDispatcher;
			EventDispatcher<Events::TextInput             > textInputDispatcher;
			EventDispatcher<Events::WindowResizedEvent    > windowResizedDispatcher;
			EventDispatcher<Events::WindowMovedEvent      > windowMovedDispatcher;
			EventDispatcher<Events::WindowMinimizedEvent  > windowMinimizedDispatcher;
			EventDispatcher<Events::WindowMaximizedEvent  > windowMaximizedDispatcher;
			EventDispatcher<Events::WindowFocusGainedEvent> windowFocusGainedDispatcher;
			EventDispatcher<Events::WindowFocusLostEvent  > windowFocusLostDispatcher;
			EventDispatcher<Events::WindowCloseEvent      > windowCloseDispatcher;
			EventDispatcher<Events::WindowMouseEnterEvent > windowMouseEnterDispatcher;
			EventDispatcher<Events::WindowMouseLeaveEvent > windowMouseLeaveDispatcher;
		};

		BLAZE_API InputEventSystem& GetInputEventSystem();

		BLAZE_API void Update();		

		struct KeyState
		{
			bool pressed;
			bool down;
			bool released;
			bool up;
			uint combo;
			double time;
		};

		inline Vec2i GetMousePos() { return Vec2i(); }
		inline int GetMouseScroll() { return 0; }
		inline Vec2i GetMouseMovement() { return Vec2i(); }
		inline KeyState GetKeyState(Key key) { return { }; }
	}
}