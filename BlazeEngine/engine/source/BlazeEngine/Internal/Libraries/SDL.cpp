#include "source/BlazeEngine/Internal/Libraries/SDL.h"
#include "BlazeEngine/DataStructures/String.h"

#include "SDL/SDL.h"

namespace Blaze
{
	void InitializeSDL()
	{
		SDL_SetMainReady();		
		int state = SDL_InitSubSystem(SDL_INIT_VIDEO);

		if (state != 0)
			throw
			"Failed to initialize the SDL library\n"
			"SDL error code: " + StringView(SDL_GetError());

		SDL_StartTextInput();
	}
	void TerminateSDL()
	{
		SDL_StopTextInput();
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
	}
}