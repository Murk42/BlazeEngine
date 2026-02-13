#include "pch.h"
#include "BlazeEngine/Runtime/IO/Input.h"
#include "BlazeEngine/Runtime/BlazeEngineContextInternal.h"
#include "BlazeEngine/External/SDL/SDL.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_clipboard.h"

namespace Blaze::Input
{
	void Update()
	{
		auto context = BlazeEngineContextInternal::GetEngineContext();

		if (context == nullptr)
		{
			BLAZE_LOG_FATAL("Calling a function dependent on the engine context, but its not initialized");
			return;
		}

		context->ProcessEvents();
	}

	KeyCode GetKeyCode(Key key, KeyModifier modifier)
	{
		return (KeyCode)SDL_GetKeyFromScancode((SDL_Scancode)key, (SDL_Keymod)modifier, false);
	}
	StringView GetKeyName(KeyCode keyCode)
	{
		const char* ptr = SDL_GetKeyName((SDL_Keycode)keyCode);

		if (ptr == nullptr)
			return StringView();
		else
			return StringView(ptr, SDL_strlen(ptr));
	}
	bool GetKeyState(Key key)
	{
		if (auto context = BlazeEngineContextInternal::GetEngineContext())
			return context->GetKeyState(key);
		else
		{
			BLAZE_LOG_FATAL("Calling a function dependent on the engine context, but its not initialized");
			return { };
		}
	}
	bool GetMouseButtonState(MouseButton button)
	{
		if (auto context = BlazeEngineContextInternal::GetEngineContext())
			return context->GetMouseButtonState(button);
		else
		{
			BLAZE_LOG_FATAL("Calling a function dependent on the engine context, but its not initialized");
			return { };
		}
	}	
	 Array<MouseData> GetMice()
	{
		 if (auto context = BlazeEngineContextInternal::GetEngineContext())
			 return context->GetMice();
		 else
		 {
			 BLAZE_LOG_FATAL("Calling a function dependent on the engine context, but its not initialized");
			 return { };
		 }
	}

	StringView GetMouseName(MouseID mouseID)
	{
		const char* name = SDL_GetMouseNameForID(mouseID.GetNumberID());
		if (name == nullptr)
			return { };
		uintMem size = strlen(name);
		return StringView(name, size);
	}

	void SetCursorType(CursorType type)
	{
		//TODO

		//if (!SDL_SetCursor((SDL_Cursor*)blazeEngineContext->GetSystemCursor(type)))
		//	BLAZE_LOG_ERROR("SDL_SetCursor failed with message: \"{}\"", SDL_GetError());
	}

	void ShowCursor(bool show)
	{
		if (show)
		{
			if (!SDL_ShowCursor())
				BLAZE_LOG_ERROR("SDL_ShowCursor failed with message: \"{}\"", SDL_GetError());
		}
		else
			if (!SDL_HideCursor())
				BLAZE_LOG_ERROR("SDL_HideCursor failed with message: \"{}\"", SDL_GetError());
	}

	bool HasClipboardText()
	{
		return SDL_HasClipboardText();
	}

	u8String GetClipboardText()
	{
		char* text = SDL_GetClipboardText();
		uintMem bufferSize = strlen(text);

		u8String out{ reinterpret_cast<const char8_t*>(text), bufferSize };

		SDL_free(text);
		return out;
	}

	void SetClipboardText(u8String text)
	{
		if (!SDL_SetClipboardText(reinterpret_cast<const char*>(text.Ptr())))
			BLAZE_LOG_ERROR("SDL_SetClipboardText failed with message: \"{}\"", SDL_GetError());
	}
}