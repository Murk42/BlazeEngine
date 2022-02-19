#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/Input/Key.h"

#include <functional>

namespace Blaze
{	
	class Window; 	

	namespace Input
	{
		struct KeyState
		{
			bool down;
			bool pressed;
			bool released;
			uint combo;
			double time;

			constexpr operator bool() const { return pressed; }
		};

		BLAZE_API void Update();

		BLAZE_API KeyState GetKeyState(Key key);
		BLAZE_API KeyState GetKeyState(MouseKey key);

		BLAZE_API int GetMouseScroll();
		BLAZE_API Vec2i GetMousePos();
		BLAZE_API Vec2i GetMouseMovement();
		BLAZE_API Window* GetFocusedWindow();
		BLAZE_API double GetDoubleClickInterval();

		BLAZE_API void ShowCursor(bool show);
		BLAZE_API void LockCursor(bool lock);

		BLAZE_API void SetDoubleClickInterval(double interval);
		BLAZE_API void SetMousePos(Vec2i);
	}
}