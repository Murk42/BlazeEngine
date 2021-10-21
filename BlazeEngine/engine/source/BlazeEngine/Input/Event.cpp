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
}