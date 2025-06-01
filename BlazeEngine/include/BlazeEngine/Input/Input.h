#pragma once
#include "BlazeEngineCore/Event/EventDispatcher.h"
#include "BlazeEngineCore/Math/Vector.h"
#include "BlazeEngineCore/DataStructures/StringUTF8.h"
#include "BlazeEngine/BlazeEngineDefines.h"

namespace Blaze
{		
	namespace Input
	{
		struct InputPreUpdateEvent {};
		struct InputPostUpdateEvent {};

		BLAZE_API EventDispatcher<InputPreUpdateEvent>& GetInputPreUpdateEventDispatcher();
		BLAZE_API EventDispatcher<InputPostUpdateEvent>& GetInputPostUpdateEventDispatcher();

		BLAZE_API void Update();				

		//Move to mouse namespace ?
		BLAZE_API Vec2f GetLastUpdateDesktopMousePos();
		BLAZE_API Vec2f GetLastUpdateMouseMovement();

		enum class CursorType
		{
			Default,
			Text,
			Wait,
			Crosshair,
			Progress,
			NotAllowed,
			Move,
			Pointer,
			ResizeNWSE,
			ResizeNESW,
			ResizeNW,
			ResizeNS,
			ResizeNE,
			ResizeSE,
			ResizeSW,
			ResizeEW,
			ResizeN,
			ResizeW,
			ResizeS,
			ResizeE,
			CursorCount
		};
		
		BLAZE_API void SetCursorType(CursorType type);
		BLAZE_API void ShowCursor(bool show);	
		//To here

		//Move to keyboard namespace ?
		BLAZE_API bool HasClipboardText();
		BLAZE_API StringUTF8 GetClipboardText();
		BLAZE_API void SetClipboardText(StringViewUTF8 text);		
		//To here
	}
}