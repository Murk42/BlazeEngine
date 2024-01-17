#include "pch.h"
#include "BlazeEngine/Input/Key.h"
#include "BlazeEngine/Internal/GlobalData.h"
#include "SDL2/SDL.h"

namespace Blaze
{	
	SDL_Scancode GetScancodeFromKey(Key key)
	{
		auto it = globalData->scancodemap.Find(key);

		if (it.IsNull())
			return SDL_SCANCODE_UNKNOWN;
		
		return it->value;
	}
	Key GetKeyFromScancode(SDL_Scancode code)
	{
		auto it = globalData->keymap.Find(code);

		if (it.IsNull())
			return Key::Unknown;

		return it->value;
	}

	String GetKeyName(Key key)
	{	
		if (key < Key::MouseLeft)
		{
			const char* str = SDL_GetScancodeName(GetScancodeFromKey(key));
			uintMem len = strlen(str);
			return String(str, len);
		}
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