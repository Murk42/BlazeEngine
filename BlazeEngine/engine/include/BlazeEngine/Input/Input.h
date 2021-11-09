#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/DataStructures/Common.h"

#include "Key.h"
#include "Event.h"

#include <functional>

namespace Blaze
{	
	class Window; 	

	namespace Input
	{							
		BLAZE_API KeyState GetKeyState(Key key);		
		BLAZE_API KeyState GetKeyState(MouseKey key);
						
		int BLAZE_API GetMouseScroll();
		Vec2i BLAZE_API GetMousePos();
		Vec2i BLAZE_API GetMouseMovement();
		BLAZE_API Window* GetFocusedWindow();
		double BLAZE_API GetDoubleClickInterval();

		void BLAZE_API ShowCursor(bool show);
		void BLAZE_API LockCursor(bool lock);

		void BLAZE_API SetDoubleClickInterval(double interval);
		void BLAZE_API SetMousePos(Vec2i);		
	}
}