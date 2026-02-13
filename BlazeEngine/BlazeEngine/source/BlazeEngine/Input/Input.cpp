#include "pch.h"
#include "BlazeEngine/Input/Input.h"
#include "BlazeEngine/BlazeEngineContext.h"
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
			blazeEngineContext->GetSystemCursor(type);
		}

		void ShowCursor(bool show)
		{
			if (show)
				SDL_ShowCursor();
			else
				SDL_HideCursor();
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
			SDL_SetClipboardText(reinterpret_cast<const char*>(text.Ptr()));
		}
	}
}