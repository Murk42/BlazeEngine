#include "pch.h"
#include "BlazeEngine/Input/Key.h"

#include "SDL2/SDL.h"

#include <map>


namespace Blaze
{	
	SDL_Scancode GetScancodeFromKey(Key key)
	{
		auto it = engineData->scancodemap.Find(key);

		if (it.IsNull())
			return SDL_SCANCODE_UNKNOWN;
		
		return it->value;
	}
	Key GetKeyFromScancode(SDL_Scancode code)
	{
		auto it = engineData->keymap.Find(code);

		if (it.IsNull())
			return Key::Unknown;

		return it->value;
	}

	String GetKeyName(Key key)
	{	
		if (key < Key::MouseLeft)
			return SDL_GetScancodeName(GetScancodeFromKey(key));		
		else
			switch (key)
			{
			case Key::MouseLeft: return "Mouse left button";
			case Key::MouseMiddle: return "Mouse middle button";
			case Key::MouseRight: return "Mouse right button";
			default: return "Invalid key";
			}
	}	
}