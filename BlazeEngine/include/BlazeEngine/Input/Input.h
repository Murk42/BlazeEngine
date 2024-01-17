#pragma once
#include "BlazeEngine/Input/Key.h"
#include "BlazeEngine/Input/InputEvents.h"
#include "BlazeEngine/Event/EventDispatcher.h"

namespace Blaze
{		
	namespace Input
	{
		struct InputEventSystem
		{			
			EventDispatcher<Events::WindowResizedEvent    > windowResizedDispatcher;
			EventDispatcher<Events::WindowMovedEvent      > windowMovedDispatcher;
			EventDispatcher<Events::WindowMinimizedEvent  > windowMinimizedDispatcher;
			EventDispatcher<Events::WindowMaximizedEvent  > windowMaximizedDispatcher;
			EventDispatcher<Events::WindowFocusGainedEvent> windowFocusGainedDispatcher;
			EventDispatcher<Events::WindowFocusLostEvent  > windowFocusLostDispatcher;
			EventDispatcher<Events::WindowCloseEvent      > windowCloseDispatcher;
			EventDispatcher<Events::WindowMouseEnterEvent > windowMouseEnterDispatcher;
			EventDispatcher<Events::WindowMouseLeaveEvent > windowMouseLeaveDispatcher;
			EventDispatcher<Events::InputPreUpdateEvent   > inputPreUpdateDispatcher;
			EventDispatcher<Events::InputPostUpdateEvent   > inputPostUpdateDispatcher;
		};

		struct KeyState
		{
			bool pressed;
			bool down;
			bool released;
			bool up;
			uint combo;

			//Last moment the key state was updated, the value is got with TimePoint::GetRunTime()
			double time;
		};					

		BLAZE_API InputEventSystem& GetInputEventSystem();

		BLAZE_API void Update();		

		BLAZE_API KeyState GetLastKeyState(Key key);		

		enum class CursorType
		{
			Arrow,
			IBeam,
			Wait,
			Crosshair,
			WaitArrow,
			SizeNWSE,
			SizeWE,
			SizeNW,
			SizeAll,
			No,
			Hand,
			CursorCount
		};

		BLAZE_API void SetCursorType(CursorType type);
	}
}