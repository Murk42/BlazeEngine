#include "pch.h"
#include "BlazeEngine/Core/Threading/Thread.h"
#include "BlazeEngine/Runtime/BlazeEngineContext.h"
#include "BlazeEngine/External/SDL/SDL.h"
#include "BlazeEngine/External/FreeType/FreeType.h"
#include "BlazeEngine/External/Sail/Sail.h"
#include <SDL3/SDL_events.h>

#ifdef BLAZE_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#ifdef BLAZE_STATIC
extern void Setup();
extern void AddLoggerOutputFiles();

static void AddLoggerOutputFiles()
{
}
#else
static void(*Setup)();
static void(*AddLoggerOutputFiles)();
#endif

namespace Blaze
{
	static Timing InitializeLibraries()
	{
		Timing timing{ "Engine libraries" };



		timing.AddNode(InitializeSDL());
		timing.AddNode(InitializeFreeType());
		timing.AddNode(InitializeSail());

		return timing;
	}
	static void TerminateLibraries()
	{
		TerminateSail();
		TerminateFreeType();
		TerminateSDL();
	}

	static unsigned ClientThreadFunction(uint32 clientThreadFinishedSDLEventIdentifier)
	{
		Setup();

		SDL_Event event;
		SDL_zero(event);
		event.type = clientThreadFinishedSDLEventIdentifier;
		SDL_PushEvent(&event);

		return 0;
	}

#ifdef BLAZE_PLATFORM_WINDOWS
	static int BlazeMain(int nCmdShow)
#endif
	{
		auto initializationTimings = InitializeLibraries();

		//This scope is here to ensure `blazeEngineContext` is destroyed before the libraries are terminated
		{
			uint32 mainThreadSDLEventIdentifier = SDL_RegisterEvents(1);

#ifdef BLAZE_PLATFORM_WINDOWS
			bool separateClientThreadMode = true;
#else
			bool separateClientThreadMode = false;
#endif

			BlazeEngineContext blazeEngineContext(BlazeEngineContextInitializationParameters{
				.initializationTimings = std::move(initializationTimings),
				.mainThreadSDLEventIdentifier = mainThreadSDLEventIdentifier ,
				.separateClientThreadMode = separateClientThreadMode,
				.nCmdShow = nCmdShow
				});
			Blaze::blazeEngineContext = &blazeEngineContext;

#ifndef BLAZE_STATIC
#ifdef BLAZE_PLATFORM_WINDOWS
			HMODULE applicationModule = GetModuleHandleA(NULL);

			if (applicationModule == nullptr)
				return 1;

			Setup = (void(*)())GetProcAddress(applicationModule, "Setup");
			AddLoggerOutputFiles = (void(*)())GetProcAddress(applicationModule, "AddLoggerOutputFiles");

			if (AddLoggerOutputFiles == nullptr)
				AddLoggerOutputFiles = []() {};

			if (Setup == nullptr)
			{
				BLAZE_LOG_ERROR("Could not find Setup function in application");
				return 1;
			}

			if (AddLoggerOutputFiles != nullptr)
				AddLoggerOutputFiles();
#endif
#else
			AddLoggerOutputFiles();

#endif

			if (separateClientThreadMode)
			{
				uint32 clientThreadFinishedSDLEventIdentifier = SDL_RegisterEvents(1);;

				Thread clientThread;
				clientThread.Run(&ClientThreadFunction, clientThreadFinishedSDLEventIdentifier);

				bool exitEventLoop = false;
				while (!exitEventLoop)
				{
					SDL_Event event;
					if (SDL_WaitEvent(&event) == 0)
						Debug::Logger::LogError("SDL", "SDL_WaitEvent failed, SDL returned error: \"" + SDL_GetError() + "\"");

					if (event.type == mainThreadSDLEventIdentifier)
					{
						auto function = (void(*)(void*))event.user.data1;

						if (function != nullptr)
							function(event.user.data2);
					}
					else if (event.type == clientThreadFinishedSDLEventIdentifier)
						exitEventLoop = true;
				}

				if (!clientThread.WaitToFinish(5.0f))
					BLAZE_LOG_WARNING("Client thread didn't exit 5 seconds after notifying the main thread that it is exiting");
			}
			else
				Setup();

			SDL_Event event;
			while (SDL_PollEvent(&event));

		}

		TerminateLibraries();
		return 0;
	}
}

#ifdef BLAZE_PLATFORM_WINDOWS
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	return Blaze::BlazeMain(nCmdShow);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
	return Blaze::BlazeMain(nCmdShow);
}
#endif

int main(int argc, char* argv[])
{
#ifdef BLAZE_PLATFORM_WINDOWS
	return Blaze::BlazeMain(SW_SHOWNORMAL);
#endif
}