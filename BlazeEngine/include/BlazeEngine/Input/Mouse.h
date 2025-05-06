#pragma once
#include "BlazeEngineCore/Math/Vector.h"
#include "BlazeEngineCore/Utilities/TimePoint.h"

namespace Blaze
{
	class Window;
}

namespace Blaze::Mouse
{
	enum class MouseButton : uint8
	{
		Left = 0,
		Right = 1,
		Middle = 2,
	};
	enum class ScrollDirection
	{
		Normal,
		Flipped
	};

	using MouseID = uint32;

	struct MouseButtonDownEvent
	{
		Window* window;
		uint64 timeNS;
		MouseID mouseID;

		MouseButton button; 
		uint combo; //number of consecutive clicks
		Vec2f pos; //position in window coordinates
		Vec2f desktopPos; //position in desktop coordinates
	};
	struct MouseButtonUpEvent
	{
		Window* window;
		uint64 timeNS;
		MouseID mouseID;

		MouseButton button;
		uint combo; //number of consecutive clicks
		Vec2f pos; //position in window coordinates
		Vec2f desktopPos; //position in desktop coordinates
	};
	struct MouseMotionEvent
	{ 
		Window* window;
		uint64 timeNS;
		MouseID mouseID;
		
		Vec2f delta; 
		Vec2f pos; //position in window coordinates
		Vec2f desktopPos; //position in desktop coordinates
	};
	struct MouseScrollEvent
	{
		Window* window;
		uint64 timeNS;
		MouseID mouseID;

		Vec2f amount;
		ScrollDirection direction;

		Vec2f pos; //position in window coordinates
		Vec2f desktopPos; //position in desktop coordinates
	};
}