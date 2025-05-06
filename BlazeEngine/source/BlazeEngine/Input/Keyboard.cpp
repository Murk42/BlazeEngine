#include "pch.h"
#include "BlazeEngine/Internal/BlazeEngineContext.h"
#include "BlazeEngine/Input/Keyboard.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_keyboard.h"

namespace Blaze::Keyboard
{	
	KeyCode GetKeyCode(Key key, KeyModifier modifier)
	{
		SDL_GetKeyFromScancode((SDL_Scancode)key, (SDL_Keymod)modifier, false);
		return BLAZE_API KeyCode();
	}
	StringView GetKeyName(KeyCode keyCode)
	{
		const char* ptr = SDL_GetKeyName((SDL_Keycode)keyCode);		

		if (ptr == nullptr)
			return StringView();
		else
			return StringView(ptr, SDL_strlen(ptr));
	}	
	KeyFrameState GetFrameKeyState(Key key)
	{
		bool down = blazeEngineContext.downKeys[(uintMem)key];
		bool pressed = blazeEngineContext.pressedKeysDuringUpdate[(uintMem)key];
		bool released = blazeEngineContext.releasedKeysDuringUpdate[(uintMem)key];
		return KeyFrameState{
			.pressed = pressed,
			.released = released,
			.down = down || pressed,
			.up = !down || released,
		};
	}
	bool GetCurrentKeyState(Keyboard::Key key)
	{
		return blazeEngineContext.downKeys[(uintMem)key];
	}
}