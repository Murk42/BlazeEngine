#include "BlazeEngine/Input/Key.h"
#include "BlazeEngine/DataStructures/Common.h"

#include "source/BlazeEngine/Internal/Engine.h"

#include "SDL/SDL.h"

#include <map>

namespace Blaze
{
	String GetKeyName(Key key)
	{
		auto it = engine->Input.scancodemap.find(key);
		if (it != engine->Input.scancodemap.end())
			return SDL_GetScancodeName(it->second);
		return "Invalid key";
	}
	String GetKeyName(MouseKey key)
	{
		switch (key)
		{
		case Blaze::MouseKey::MouseLeft: return "Mouse left button";
		case Blaze::MouseKey::MouseMiddle: return "Mouse middle button";			
		case Blaze::MouseKey::MouseRight: return "Mouse right button";			
		default: return "Invalid key";
		}
	}
	char KeyToChar(Key key)
	{		
		SDL_Keycode v = SDL_GetKeyFromScancode((SDL_Scancode)engine->Input.scancodemap[key]);
		if (v > 127 || v < 32)
			v = 0;
		return v;
	}	
}