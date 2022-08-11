#include "source/BlazeEngine/Internal/Libraries/SDL.h"
#include "BlazeEngine/Utilities/Time.h"

#include "SDL/SDL.h"


namespace Blaze
{
	Startup::BlazeLibrariesInitInfo::SDLInitInfo InitializeSDL()
	{
		Startup::BlazeLibrariesInitInfo::SDLInitInfo initInfo;
		TimePoint startTimePoint = TimePoint::GetWorldTime();

		SDL_SetMainReady();		
		int state = SDL_InitSubSystem(SDL_INIT_VIDEO);

		if (state != 0)
			throw
			"Failed to initialize the SDL library\n"
			"SDL error code: " + StringView(SDL_GetError());

		SDL_StartTextInput();

		initInfo.initTime = TimePoint::GetWorldTime() - startTimePoint;
		return initInfo;
	}
	void TerminateSDL()
	{
		SDL_StopTextInput();
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
	}
}