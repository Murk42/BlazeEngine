#pragma once
#include "BlazeEngine/Input/Key.h"
#include "BlazeEngine/DataStructures/StringUTF8.h"
#include <functional>

namespace Blaze
{
	class Window;	

	namespace Event
	{
		struct KeyPressed			{ Key key; double time; uint count; };
		struct KeyReleased			{ Key key; double time; };		
		struct MousePressed			{ MouseKey key; Vec2i pos; double time; uint count; };
		struct MouseReleased		{ MouseKey key; Vec2i pos; double time; };      		
		struct MouseMotion			{ Vec2i pos; Vec2i delta; };
		struct MouseScroll			{ int value; };
		struct MouseEnter			{ };			
		struct MouseLeave			{ };				
		struct WindowResized		{ Vec2i size; Window* window; };		
		struct WindowMoved			{ Vec2i size; Window* window; };
		struct WindowMinimized		{ Window* window; };	
		struct WindowMaximized		{ Window* window; };	
		struct WindowFocusGained	{ Window* window; };	
		struct WindowFocusLost		{ Window* window; };	
		struct WindowClosed			{ Window* window; };		
		struct TextInput			{ StringUTF8 input; };
		struct ViewportChanged		{ Vec2i pos; Vec2i size; };
	}	
}