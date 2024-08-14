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

		BLAZE_API InputEventSystem& GetInputEventSystem();

		BLAZE_API void Update();				

		BLAZE_API Vec2i GetDesktopMousePos();

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

		BLAZE_API bool HasClipboardText();
		BLAZE_API StringUTF8 GetClipboardText();
		BLAZE_API void SetClipboardText(StringViewUTF8 text);
	}
}