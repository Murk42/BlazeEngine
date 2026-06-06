#include "pch.h"
#include "BlazeEngine/Runtime/IO/Input.h"
#include "BlazeEngine/Runtime/BlazeEngineContextInternal.h"
#include "BlazeEngine/External/SDL/SDL.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_clipboard.h"

namespace Blaze::Input
{
	void PollEvents()
	{
		auto context = BlazeEngineContextInternal::GetEngineContext();

		if (context == nullptr)
		{
			BLAZE_LOG_FATAL("Calling a function dependent on the engine context, but its not initialized");
			return;
		}

		context->PollEvents();
	}

	bool WaitForEvents(float seconds)
	{
		auto context = BlazeEngineContextInternal::GetEngineContext();

		if (context == nullptr)
		{
			BLAZE_LOG_FATAL("Calling a function dependent on the engine context, but its not initialized");
			return false;
		}

		return context->WaitForEvents(seconds);
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
		static SDL_Cursor* cursor = nullptr;

		if (cursor != nullptr)
			SDL_DestroyCursor(cursor);

		switch (type)
		{
		case Blaze::Input::CursorType::Default:
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
			break;
		case Blaze::Input::CursorType::Text:
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_TEXT);
			break;
		case Blaze::Input::CursorType::Wait:
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
			break;
		case Blaze::Input::CursorType::Crosshair:
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
			break;
		case Blaze::Input::CursorType::Progress:
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_PROGRESS);
			break;
		case Blaze::Input::CursorType::NotAllowed:
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NOT_ALLOWED);
			break;
		case Blaze::Input::CursorType::Move:
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE);
			break;
		case Blaze::Input::CursorType::Pointer:
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
			break;
		case Blaze::Input::CursorType::ResizeNWSE:
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NWSE_RESIZE);
			break;
		case Blaze::Input::CursorType::ResizeNESW:
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NESW_RESIZE);
			break;
		case Blaze::Input::CursorType::ResizeNW:
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NW_RESIZE);
			break;
		case Blaze::Input::CursorType::ResizeNS:
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NS_RESIZE);
			break;
		case Blaze::Input::CursorType::ResizeNE:
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NE_RESIZE);
			break;
		case Blaze::Input::CursorType::ResizeSE:
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SE_RESIZE);
			break;
		case Blaze::Input::CursorType::ResizeSW:
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SW_RESIZE);
			break;
		case Blaze::Input::CursorType::ResizeEW:
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_EW_RESIZE);
			break;
		case Blaze::Input::CursorType::ResizeN:
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_N_RESIZE);
			break;
		case Blaze::Input::CursorType::ResizeW:
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_W_RESIZE);
			break;
		case Blaze::Input::CursorType::ResizeS:
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_S_RESIZE);
			break;
		case Blaze::Input::CursorType::ResizeE:
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_E_RESIZE);
			break;
		default:
			break;
		}

		if (!SDL_SetCursor(cursor))
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

	void SetClipboardText(u8StringView text)
	{
		u8String nullTerminated = text;
		if (!SDL_SetClipboardText(reinterpret_cast<const char*>(nullTerminated.Ptr())))
			BLAZE_LOG_ERROR("SDL_SetClipboardText failed with message: \"{}\"", SDL_GetError());
	}
}