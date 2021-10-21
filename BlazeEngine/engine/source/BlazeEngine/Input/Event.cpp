#include "BlazeEngine/Input/Event.h"
#include "source/BlazeEngine/Internal/Engine.h"

namespace Blaze
{
	namespace Input
	{		
		void SetEventFunction(const InputEventFunction::KeyPressed& func)			{ engine->Input.eventFunctions.keyPressed			= func; }
		void SetEventFunction(const InputEventFunction::KeyReleased& func)			{ engine->Input.eventFunctions.keyReleased			= func; }
		void SetEventFunction(const InputEventFunction::MousePressed& func)			{ engine->Input.eventFunctions.mousePressed			= func; }
		void SetEventFunction(const InputEventFunction::MouseReleased& func)		{ engine->Input.eventFunctions.mouseReleased		= func; }		
		void SetEventFunction(const InputEventFunction::MouseMotion& func)			{ engine->Input.eventFunctions.mouseMotion			= func; }		
		void SetEventFunction(const InputEventFunction::MouseScroll& func)			{ engine->Input.eventFunctions.mouseScroll			= func; }
		void SetEventFunction(const InputEventFunction::MouseEnter& func)			{ engine->Input.eventFunctions.mouseEnter			= func; }
		void SetEventFunction(const InputEventFunction::MouseLeave& func)			{ engine->Input.eventFunctions.mouseLeave			= func; }
		void SetEventFunction(const InputEventFunction::WindowSizeChanged& func)	{ engine->Input.eventFunctions.windowSizeChanged	= func; }
		void SetEventFunction(const InputEventFunction::WindowResized& func)		{ engine->Input.eventFunctions.windowResized		= func; }
		void SetEventFunction(const InputEventFunction::WindowMoved& func)			{ engine->Input.eventFunctions.windowMoved			= func; }
		void SetEventFunction(const InputEventFunction::WindowMinimized& func)		{ engine->Input.eventFunctions.windowMinimized		= func; }
		void SetEventFunction(const InputEventFunction::WindowMaximized& func)		{ engine->Input.eventFunctions.windowMaximized		= func; }
		void SetEventFunction(const InputEventFunction::WindowFocusGained& func)	{ engine->Input.eventFunctions.windowFocusGained	= func; }
		void SetEventFunction(const InputEventFunction::WindowFocusLost& func)		{ engine->Input.eventFunctions.windowFocusLost		= func; }
		void SetEventFunction(const InputEventFunction::WindowClosed& func)			{ engine->Input.eventFunctions.windowClosed			= func; }
		void SetEventFunction(const InputEventFunction::TextInput& func)			{ engine->Input.eventFunctions.textInput			= func; }
	}

	void Engine::Input_KeyPressed		(InputEvent::KeyPressed			event)
	{
		engine->Input.eventFunctions.keyPressed(event);
		engine->UI_KeyPressedEvent(event);
	}
	void Engine::Input_KeyReleased		(InputEvent::KeyReleased		event)
	{
		engine->Input.eventFunctions.keyReleased(event);
	}
	void Engine::Input_MousePressed		(InputEvent::MousePressed		event)
	{
		engine->Input.eventFunctions.mousePressed(event);
		engine->UI_MousePressedEvent(event);
	}
	void Engine::Input_MouseReleased	(InputEvent::MouseReleased		event)
	{
		engine->Input.eventFunctions.mouseReleased(event);
		engine->UI_MouseReleasedEvent(event);
	}
	void Engine::Input_MouseMotion		(InputEvent::MouseMotion		event)
	{
		engine->Input.eventFunctions.mouseMotion(event);
		engine->UI_MouseMotionEvent(event);
	}
	void Engine::Input_MouseScroll		(InputEvent::MouseScroll		event)
	{
		engine->Input.eventFunctions.mouseScroll(event);
	}
	void Engine::Input_MouseEnter		(InputEvent::MouseEnter			event)
	{
		engine->Input.eventFunctions.mouseEnter(event);
	}
	void Engine::Input_MouseLeave		(InputEvent::MouseLeave			event)
	{
		engine->Input.eventFunctions.mouseLeave(event);
	}
	void Engine::Input_WindowSizeChanged(InputEvent::WindowSizeChanged	event)
	{
		engine->Input.eventFunctions.windowSizeChanged(event);		
	}
	void Engine::Input_WindowResized	(InputEvent::WindowResized		event)
	{
		engine->Input.eventFunctions.windowResized(event);
	}
	void Engine::Input_WindowMoved		(InputEvent::WindowMoved		event)
	{
		engine->Input.eventFunctions.windowMoved(event);
	}
	void Engine::Input_WindowMinimized	(InputEvent::WindowMinimized	event)
	{
		engine->Input.eventFunctions.windowMinimized(event);
	}
	void Engine::Input_WindowMaximized	(InputEvent::WindowMaximized	event)
	{
		engine->Input.eventFunctions.windowMaximized(event);
	}
	void Engine::Input_WindowFocusGained(InputEvent::WindowFocusGained	event)
	{
		engine->Input.eventFunctions.windowFocusGained(event);
	}
	void Engine::Input_WindowFocusLost	(InputEvent::WindowFocusLost	event)
	{
		engine->Input.eventFunctions.windowFocusLost(event);
	}
	void Engine::Input_WindowClosed		(InputEvent::WindowClosed		event)
	{
		engine->Input.eventFunctions.windowClosed(event);
	}
	void Engine::Input_TextInput		(InputEvent::TextInput			event)
	{
		engine->Input.eventFunctions.textInput(event);
		engine->UI_TextInputEvent(event);
	}
}