#include "BlazeEngine/Input/Key.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "source/BlazeEngine/Internal/EngineData.h"

#include "SDL/SDL.h"

#include <map>


namespace Blaze
{	
	SDL_Scancode GetScancodeFromKey(Key key)
	{
		return engineData->scancodemap.at(key);
	}
	Key GetKeyFromScancode(SDL_Scancode code)
	{
		auto it = engineData->keymap.find(code);
		if (it != engineData->keymap.end())
			return it->second;
		return Key::Unknown;
	}

	String GetKeyName(Key key)
	{		
		return SDL_GetScancodeName(GetScancodeFromKey(key));		
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
}