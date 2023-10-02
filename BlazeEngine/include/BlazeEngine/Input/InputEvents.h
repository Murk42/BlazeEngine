#pragma once
#include "BlazeEngine/Input/Key.h"
#include "BlazeEngine/DataStructures/StringUTF8.h"

namespace Blaze
{
	class Window;	

	namespace Input::Events
	{
		struct KeyPressed             { Key key; double time; uint combo; };
		struct KeyReleased            { Key key; double time;             };				
		struct MouseMotion            { Vec2i pos; Vec2i delta;           };
		struct MouseScroll            { int value;                        };		
		struct TextInput              { StringUTF8 input;                 };		
		struct WindowResizedEvent     { Window* window; Vec2i size;       };
		struct WindowMovedEvent       { Window* window; Vec2i pos;        };
		struct WindowMinimizedEvent   { Window* window;                   };
		struct WindowMaximizedEvent   { Window* window;                   };
		struct WindowFocusGainedEvent { Window* window;                   };
		struct WindowFocusLostEvent   { Window* window;                   };
		struct WindowCloseEvent       { Window* window;                   };
		struct WindowMouseEnterEvent  { Window* window;	                  };
		struct WindowMouseLeaveEvent  { Window* window;	                  };

		enum class InputEventType
		{
			KeyPressed,
			KeyReleased,
			MouseMotion,
			MouseScroll,
			TextInput,
			WindowResizedEvent,
			WindowMovedEvent,
			WindowMinimizedEvent,
			WindowMaximizedEvent,
			WindowFocusGainedEvent,
			WindowFocusLostEvent,
			WindowCloseEvent,
			WindowMouseEnterEvent,
			WindowMouseLeaveEvent,
		};
	}	
}