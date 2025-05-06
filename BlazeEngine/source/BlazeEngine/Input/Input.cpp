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
				cursor = SDL_CreateSystemCursor((SDL_SystemCursor)type);

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