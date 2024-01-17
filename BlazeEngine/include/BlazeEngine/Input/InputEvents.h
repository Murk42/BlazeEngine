#pragma once
#include "BlazeEngine/Input/Key.h"

namespace Blaze
{
	class WindowBase;	

	namespace Input::Events
	{
		struct KeyPressed             { Key key; double time; uint combo; };
		struct KeyReleased            { Key key; double time;             };				
		struct MouseMotion            { Vec2i delta;           };
		struct MouseScroll            { int value;                        };		
		struct TextInput              { StringUTF8 input;                 };		
		struct WindowResizedEvent     { WindowBase* window; Vec2u size;       };
		struct WindowMovedEvent       { WindowBase* window; Vec2i pos;        };
		struct WindowMinimizedEvent   { WindowBase* window;                   };
		struct WindowMaximizedEvent   { WindowBase* window;                   };
		struct WindowFocusGainedEvent { WindowBase* window;                   };
		struct WindowFocusLostEvent   { WindowBase* window;                   };
		struct WindowCloseEvent       { WindowBase* window;                   };
		struct WindowMouseEnterEvent  { WindowBase* window;	                  };
		struct WindowMouseLeaveEvent  { WindowBase* window;	                  };
		struct InputPreUpdateEvent    { };
		struct InputPostUpdateEvent { };

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