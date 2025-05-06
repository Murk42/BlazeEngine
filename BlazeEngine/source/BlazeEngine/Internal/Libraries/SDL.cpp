#include "pch.h"
#include "BlazeEngine/Internal/Libraries/SDL.h"
#include "BlazeEngine/Internal/BlazeEngineContext.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

namespace Blaze 
{
	StringView GetSDLError()
	{
		const char* ptr = SDL_GetError();
		uintMem len = strlen(ptr);
		return StringView(ptr, len);
	}

	void PostSDLMainThreadTaskEvent()
	{
		SDL_Event event;
		SDL_memset(&event, 0, sizeof(event));
		event.type = blazeEngineContext.mainThreadTaskEventIdentifier;
		SDL_PushEvent(&event);
	}
	void PostSDLClientThreadExitEvent()
	{
		SDL_Event event;
		SDL_memset(&event, 0, sizeof(event));
		event.type = blazeEngineContext.clientThreadExitEventIdentifier;
		SDL_PushEvent(&event);
	}
	static bool SDLEventWatcher(void*, SDL_Event* event)
	{
		blazeEngineContext.HandleSDLEvent(*event);
		return false;
	}


	TimingResult InitializeSDL()
	{
		Timing timing{ "SDL" };		
				
		SDL_SetHint("SDL_HINT_VIDEO_ALLOW_SCREENSAVER", "1");

		SDL_SetMainReady();			

		SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, "Blaze Engine application");
		//SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_VERSION_STRING, "");
		//SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_IDENTIFIER_STRING, "");
		//SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING, "");
		//SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_COPYRIGHT_STRING, "");
		//SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_URL_STRING, "");
		//SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING, "");

		if (SDL_InitSubSystem(SDL_INIT_VIDEO) == false)
			BLAZE_ENGINE_CORE_FATAL("Failed to initialize the SDL library. SDL returned error: \"" + GetSDLError() + "\"");

		if (SDL_InitSubSystem(SDL_INIT_EVENTS) == false)
			BLAZE_ENGINE_CORE_FATAL("Failed to initialize the SDL library. SDL returned error: \"" + GetSDLError() + "\"");		

		blazeEngineContext.mainThreadTaskEventIdentifier = SDL_RegisterEvents(1);
		blazeEngineContext.clientThreadExitEventIdentifier = SDL_RegisterEvents(1);		
		
		if (!SDL_AddEventWatch(SDLEventWatcher, nullptr))
			BLAZE_ENGINE_CORE_FATAL("Failed to add event watcher for SDL. SDL returned error: \"" + GetSDLError() + "\"");		

		int version = SDL_GetVersion();
		BLAZE_ENGINE_CORE_INFO("Successfully initialized SDL " + StringParsing::Convert(SDL_VERSIONNUM_MAJOR(version)) + "." + StringParsing::Convert(SDL_VERSIONNUM_MINOR(version)) + "." + StringParsing::Convert(SDL_VERSIONNUM_MICRO(version)));


		return timing.GetTimingResult();
	}

	void TerminateSDL()
	{
		SDL_RemoveEventWatch(SDLEventWatcher, nullptr);

		SDL_QuitSubSystem(SDL_INIT_VIDEO);
		SDL_QuitSubSystem(SDL_INIT_EVENTS);
	}
	void* SDLAllocator::Allocate(uintMem size)
	{
		return SDL_malloc(size);		
	}
	void SDLAllocator::Free(void* ptr)
	{
		SDL_free(ptr);
	}
}