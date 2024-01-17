#include "pch.h"
#include "BlazeEngine/Internal/Libraries/SDL.h"
#include "BlazeEngine/Internal/GlobalData.h"

#include "SDL2/SDL.h"

namespace Blaze 
{
	static StringView GetSDLError()
	{
		const char* ptr = SDL_GetError();
		uintMem len = strlen(ptr);
		return StringView(ptr, len);
	}

	TimingResult InitializeSDL()
	{
		Timing timing{ "SDL" };		
		
		SDL_SetMainReady();				

		if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
			Debug::Logger::LogFatal("Blaze Engine", "Failed to initialize the SDL library. SDL_GetError returned: \"" + GetSDLError() + "\"");

		SDL_StartTextInput();

		globalData->mainThreadTaskEventIdentifier = SDL_RegisterEvents(1);
		globalData->clientThreadExitEventIdentifier = SDL_RegisterEvents(1);

		SDL_version version;
		SDL_GetVersion(&version);
		Debug::Logger::LogInfo("Blaze Engine", "Successfully initialized SDL " + StringParsing::Convert(version.major) + "." + StringParsing::Convert(version.minor) + "." + StringParsing::Convert(version.patch));		

		return timing.GetTimingResult();
	}

	void TerminateSDL()
	{
		SDL_StopTextInput();
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
	}
}