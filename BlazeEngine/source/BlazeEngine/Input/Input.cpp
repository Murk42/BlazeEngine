#include "pch.h"
#include "BlazeEngine/Internal/Libraries/SDL.h"
#include "BlazeEngine/Input/Input.h"
#include "BlazeEngine/Internal/BlazeEngineContext.h"
#include "BlazeEngine/Window/Window.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_clipboard.h"

namespace Blaze
{	
	TimingResult InitializeInput()
	{
		Timing timing{ "Input" };		

		SDL_Rect bounds;
		if (SDL_GetDisplayBounds(SDL_GetPrimaryDisplay(), &bounds))
			blazeEngineContext.primaryDesktopHeight = bounds.h;
		else
			BLAZE_ENGINE_CORE_FATAL("Failed to get display bounds. SDL returned error: \"" + GetSDLError() + "\"");

		return timing.GetTimingResult();
	}
	void TerminateInput()
	{		
		for (auto& cursor : blazeEngineContext.cursors)
			if (cursor != nullptr)
				SDL_DestroyCursor(cursor);
	}

	namespace Input
	{		
		static SDL_SystemCursor ConvertToSDL(CursorType value)
		{
			switch (value)
			{
			case Blaze::Input::CursorType::Default:		return SDL_SYSTEM_CURSOR_DEFAULT;
			case Blaze::Input::CursorType::Text:		return SDL_SYSTEM_CURSOR_TEXT;
			case Blaze::Input::CursorType::Wait:		return SDL_SYSTEM_CURSOR_WAIT;
			case Blaze::Input::CursorType::Crosshair:	return SDL_SYSTEM_CURSOR_CROSSHAIR;
			case Blaze::Input::CursorType::Progress:	return SDL_SYSTEM_CURSOR_PROGRESS;
			case Blaze::Input::CursorType::NotAllowed:	return SDL_SYSTEM_CURSOR_NOT_ALLOWED;
			case Blaze::Input::CursorType::Move:		return SDL_SYSTEM_CURSOR_MOVE;
			case Blaze::Input::CursorType::Pointer:		return SDL_SYSTEM_CURSOR_POINTER;
			case Blaze::Input::CursorType::ResizeNWSE:	return SDL_SYSTEM_CURSOR_NWSE_RESIZE;
			case Blaze::Input::CursorType::ResizeNESW:	return SDL_SYSTEM_CURSOR_NESW_RESIZE;
			case Blaze::Input::CursorType::ResizeNW:	return SDL_SYSTEM_CURSOR_NW_RESIZE;
			case Blaze::Input::CursorType::ResizeNS:	return SDL_SYSTEM_CURSOR_NS_RESIZE;
			case Blaze::Input::CursorType::ResizeNE:	return SDL_SYSTEM_CURSOR_NE_RESIZE;
			case Blaze::Input::CursorType::ResizeSE:	return SDL_SYSTEM_CURSOR_SE_RESIZE;
			case Blaze::Input::CursorType::ResizeSW:	return SDL_SYSTEM_CURSOR_SW_RESIZE;
			case Blaze::Input::CursorType::ResizeEW:	return SDL_SYSTEM_CURSOR_EW_RESIZE;
			case Blaze::Input::CursorType::ResizeN:		return SDL_SYSTEM_CURSOR_N_RESIZE;
			case Blaze::Input::CursorType::ResizeW:		return SDL_SYSTEM_CURSOR_W_RESIZE;
			case Blaze::Input::CursorType::ResizeS:		return SDL_SYSTEM_CURSOR_S_RESIZE;
			case Blaze::Input::CursorType::ResizeE:		return SDL_SYSTEM_CURSOR_E_RESIZE;
			case Blaze::Input::CursorType::CursorCount:				
			default:
				BLAZE_ENGINE_FATAL("Invalid CursorType value");
				return SDL_SYSTEM_CURSOR_DEFAULT;
				break;
			}						
		}
		EventDispatcher<InputPreUpdateEvent>& GetInputPreUpdateEventDispatcher()
		{
			return blazeEngineContext.inputPreUpdateEventDispatcher;
		}
		EventDispatcher<InputPostUpdateEvent>& GetInputPostUpdateEventDispatcher()
		{
			return blazeEngineContext.inputPostUpdateEventDispatcher;
		}
		void Update()
		{
			blazeEngineContext.inputPreUpdateEventDispatcher.Call({ });
			
			blazeEngineContext.UpdateInputState();
			
			blazeEngineContext.ProcessAllEvents();

			blazeEngineContext.inputPostUpdateEventDispatcher.Call({ });
		}

		Vec2f GetLastUpdateDesktopMousePos()
		{
			return blazeEngineContext.lastUpdateDesktopMousePos;
		}

		Vec2f GetLastUpdateMouseMovement()
		{
			return blazeEngineContext.lastUpdateMouseMovement;
		}

		void SetCursorType(CursorType type)
		{
			auto& cursor = blazeEngineContext.cursors[(uint)type];

			if (cursor == nullptr)
				cursor = SDL_CreateSystemCursor(ConvertToSDL(type));

			SDL_SetCursor(cursor);
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

		StringUTF8 GetClipboardText()
		{
			char* text = SDL_GetClipboardText();
			uintMem bufferSize = strlen(text);

			StringUTF8 out{ (void*)text, bufferSize };

			SDL_free(text);
			return out;
		}

		void SetClipboardText(StringViewUTF8 text)
		{
			SDL_SetClipboardText((char*)text.Buffer());
		}		
	}
}