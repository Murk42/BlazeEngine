#include "pch.h"
#include "BlazeEngine/External/SDL/SDL.h"
#include "BlazeEngine/Runtime/BlazeEngineContext.h"

#include <SDL3/SDL_main.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_version.h>
#include <SDL3/SDL_error.h>


namespace Blaze
{
	StringView SDL_GetError()
	{
		const char* ptr = ::SDL_GetError();
		uintMem len = strlen(ptr);
		return StringView(ptr, len);
	}

	bool InitializeSDL(TimingTree& timingTree)
	{
		SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, "Blaze Engine application");
		SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_VERSION_STRING, "0.1");
		SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_IDENTIFIER_STRING, "blaze_engine.app");
		SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING, "Marko Radojevic");
		SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_COPYRIGHT_STRING, "No copyright for now");
		SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_URL_STRING, "https://github.com/Murk42/BlazeEngine");
		SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING, "application");

		SDL_SetHint("SDL_HINT_VIDEO_ALLOW_SCREENSAVER", "1");

		SDL_SetMainReady();

		if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS) == false)
		{
			BLAZE_LOG_ERROR("Failed to initialize the SDL library. SDL returned error: \"" + SDL_GetError() + "\"");
			return false;
		}

		return true;
	}

	void TerminateSDL()
	{
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