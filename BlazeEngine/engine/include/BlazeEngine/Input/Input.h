#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/DataStructures/Common.h"

#include "Key.h"

namespace Blaze
{	
	class Window;

	enum class InputEvent
	{
		//DONT CHANGE PLACES AND ORDER OF FRIST FOUR
		KeyUp,				/* void Function(Key, double) */
		KeyDown,			/* void Function(Key, double) */
		KeyPressed,			/* void Function(Key, double, uint) */
		KeyReleased,		/* void Function(Key, double) */		
		MouseMotion,		/* void Function(int dx, int dy) */
		MouseWheel,			/* void Function(int v) */
		MouseEnter,			/* void Function(Window*) */
		MouseLeave,			/* void Function(Window*) */
		WindowSizeChanged,	/* void Function(int w, int h, Window*) */
		WindowResized,		/* void Function(int w, int h, Window*) */
		WindowMoved,		/* void Function(int x, int y, Window*) */
		WindowMinimized,	/* void Function(Window*) */
		WindowMaximized,	/* void Function(Window*) */
		WindowFocusGained,	/* void Function(Window*) */
		WindowFocusLost,	/* void Function(Window*) */
		WindowClosed,		/* void Function(Window*) */
		InputEvent_Count
	};			

	enum class KeyState
	{
		Up = (int)InputEvent::KeyUp,
		Down = (int)InputEvent::KeyDown,
		Pressed = (int)InputEvent::KeyPressed,
		Released = (int)InputEvent::KeyReleased,		
	};

	struct KeyStateData
	{	
		double lastPress;
		uint pressCount;
		KeyState state;
	};

	namespace Input
	{							
		void BLAZE_API Update();

		const BLAZE_API KeyStateData GetKeyState(Key code);		

		void BLAZE_API SetEventFunction(InputEvent event, void* function);			
		
		int BLAZE_API GetMouseScroll();
		Vec2i BLAZE_API GetMousePos();
		Vec2i BLAZE_API GetMouseMovement();
		BLAZE_API Window* GetFocusedWindow();
		double BLAZE_API GetDoubleClickInterval();

		void BLAZE_API SetDoubleClickInterval(double interval);
		void BLAZE_API SetMousePos(const Vec2i&);
	}
}