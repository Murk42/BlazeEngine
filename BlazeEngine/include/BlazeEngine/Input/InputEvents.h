#pragma once
#include "BlazeEngine/Input/Key.h"

namespace Blaze
{
	class WindowBase;	

	namespace Input::Events
	{
		struct KeyPressed             { WindowBase* window; Key key; TimePoint time; uint combo; };
		struct KeyReleased            { WindowBase* window; Key key; TimePoint time;             };				
		struct MouseMotion            { WindowBase* window; Vec2i delta;                      };
		struct MouseScroll            { WindowBase* window; int value;                        };		
		struct TextInput              { WindowBase* window; StringUTF8 input;                 };		
		struct WindowResizedEvent     { WindowBase* window; Vec2u size;                       };
		struct WindowMovedEvent       { WindowBase* window; Vec2i pos;                        };
		struct WindowMinimizedEvent   { WindowBase* window;                                   };
		struct WindowMaximizedEvent   { WindowBase* window;                                   };
		struct WindowFocusGainedEvent { WindowBase* window;                                   };
		struct WindowFocusLostEvent   { WindowBase* window;                                   };
		struct WindowCloseEvent       { WindowBase* window;                                   };
		struct WindowMouseEnterEvent  { WindowBase* window;	                                  };
		struct WindowMouseLeaveEvent  { WindowBase* window;	                                  };
		struct InputPreUpdateEvent    {                                                       };
		struct InputPostUpdateEvent   {                                                       };

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