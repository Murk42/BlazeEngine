#include "BlazeEngine/Input/Key.h"
#include "BlazeEngine/DataStructures/Common.h"

#include "Engine.h"

#include "SDL/SDL.h"

#include <map>

namespace Blaze
{	
	char KeyToChar(Key key)
	{		
		SDL_Keycode v = SDL_GetKeyFromScancode((SDL_Scancode)engine->Input.scancodemap[key]);
		if (v > 127 || v < 32)
			v = 0;
		return v;
	}
	String GetKeyName(Key key)
	{
		switch (key)
		{
		case Key::MouseLeft: return "Mouse Left Button";
		case Key::MouseRight: return "Mouse Right Button";
		case Key::MouseMiddle: return "Mouse Middle Button";
		default: return SDL_GetScancodeName((SDL_Scancode)key);
		}
	}
}