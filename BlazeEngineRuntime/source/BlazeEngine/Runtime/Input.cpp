#include "pch.h"
#include "BlazeEngine/Runtime/Input.h"
#include "BlazeEngine/Runtime/BlazeEngineContext.h"
#include "BlazeEngine/External/SDL/SDL.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_clipboard.h"

namespace Blaze
{
	namespace Input
	{

		EventDispatcher<InputPreUpdateEvent>& GetInputPreUpdateEventDispatcher()
		{
			return blazeEngineContext->inputPreUpdateEventDispatcher;
		}
		EventDispatcher<InputPostUpdateEvent>& GetInputPostUpdateEventDispatcher()
		{
			return blazeEngineContext->inputPostUpdateEventDispatcher;
		}

		void Update()
		{
			blazeEngineContext->inputPreUpdateEventDispatcher.Call({ });

			blazeEngineContext->UpdateInputAndEvents();

			blazeEngineContext->inputPostUpdateEventDispatcher.Call({ });
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
		KeyFrameState GetKeyFrameState(Key key)
		{
			return blazeEngineContext->GetKeyFrameState(key);
		}
		MouseButtonFrameState Blaze::Input::GetMouseButtonFrameState(MouseButton button)
		{
			return blazeEngineContext->GetMouseButtonFrameState(button);
		}

		void SetCursorType(CursorType type)
		{
			if (!SDL_SetCursor((SDL_Cursor*)blazeEngineContext->GetSystemCursor(type)))
				BLAZE_LOG_ERROR("SDL_SetCursor failed with message: \"{}\"", SDL_GetError());
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
}