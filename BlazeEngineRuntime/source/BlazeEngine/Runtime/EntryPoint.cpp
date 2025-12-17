#include "pch.h"
#include "BlazeEngine/Runtime/BlazeEngineContext.h"

#ifdef BLAZE_PLATFORM_WINDOWS
#include "BlazeEngine/Platform/Windows/Windows.h"
#include <Windows.h>
#endif

#ifdef BLAZE_STATIC
extern void Setup();
extern void PreInitialization();
static void PreInitialization()
{
}
#else
static void(*Setup)();
static void(*PreInitialization)();
#endif

namespace Blaze
{
	static int BlazeMain()
	{
#ifndef BLAZE_STATIC
#ifdef BLAZE_PLATFORM_WINDOWS
		HMODULE applicationModule = GetModuleHandleA(NULL);

		if (applicationModule == nullptr)
			return 1;

		Setup = (void(*)())GetProcAddress(applicationModule, "Setup");
		PreInitialization = (void(*)())GetProcAddress(applicationModule, "PreInitialization");

		if (PreInitialization == nullptr)
			PreInitialization = []() {};

		if (Setup == nullptr)
		{
			BLAZE_LOG_ERROR("Could not find Setup function in application");
			return 1;
		}

		if (PreInitialization == nullptr)
			PreInitialization = []() {};
#endif
#endif

		PreInitialization();

		BlazeEngineContext::InitializeEngine({ });

		Setup();

		BlazeEngineContext::TerminateEngine();

		return 0;
	}
}

#ifdef BLAZE_PLATFORM_WINDOWS
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	return Blaze::BlazeMain();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
	return Blaze::BlazeMain();
}
#endif

int main(int argc, char* argv[])
{
	return Blaze::BlazeMain();
}