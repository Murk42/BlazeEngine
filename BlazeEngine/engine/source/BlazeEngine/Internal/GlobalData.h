#pragma once
#include <BlazeEngine/Event/EventDispatcher.h>

struct EngineData
{
	Blaze::EventDispatcher<Blaze::Event::KeyPressed			> keyPressedDispatcher;
	Blaze::EventDispatcher<Blaze::Event::KeyReleased		> keyReleasedDispatcher;
	Blaze::EventDispatcher<Blaze::Event::MousePressed		> mousePressedDispatcher;
	Blaze::EventDispatcher<Blaze::Event::MouseReleased		> mouseReleasedDispatcher;
	Blaze::EventDispatcher<Blaze::Event::MouseMotion		> mouseMotionDispatcher;
	Blaze::EventDispatcher<Blaze::Event::MouseScroll		> mouseScrollDispatcher;
	Blaze::EventDispatcher<Blaze::Event::MouseEnter			> mouseEnterDispatcher;
	Blaze::EventDispatcher<Blaze::Event::MouseLeave			> mouseLeaveDispatcher;
	Blaze::EventDispatcher<Blaze::Event::WindowResized		> windowResizedDispatcher;
	Blaze::EventDispatcher<Blaze::Event::WindowMoved		> windowMovedDispatcher;
	Blaze::EventDispatcher<Blaze::Event::WindowMinimized	> windowMinimizedDispatcher;
	Blaze::EventDispatcher<Blaze::Event::WindowMaximized	> windowMaximizedDispatcher;
	Blaze::EventDispatcher<Blaze::Event::WindowFocusGained	> windowFocusGainedDispatcher;
	Blaze::EventDispatcher<Blaze::Event::WindowFocusLost	> windowFocusLostDispatcher;
	Blaze::EventDispatcher<Blaze::Event::WindowClosed		> windowClosedDispatcher;
	Blaze::EventDispatcher<Blaze::Event::TextInput			> textInputDispatcher;
	Blaze::EventDispatcher<Blaze::Event::ViewportChanged	> viewportChangedDispatcher;
}* engineData;