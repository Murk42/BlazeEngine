#include "pch.h"
#include "BlazeEngine/Runtime/BlazeEngineContext.h"
#include "BlazeEngine/Core/Time/TimingTree.h"

#ifdef BLAZE_PLATFORM_WINDOWS
#include "BlazeEngine/Platform/Windows/Windows.h"
#include <Windows.h>
#endif

extern int Main();
extern bool PreInitialization();

namespace Blaze
{
	static int BlazeMain()
	{
//#ifndef BLAZE_STATIC
//#ifdef BLAZE_PLATFORM_WINDOWS
//		HMODULE applicationModule = GetModuleHandleA(NULL);
//
//		if (applicationModule == nullptr)
//			return 1;
//
//		Setup = (void(*)())GetProcAddress(applicationModule, "Setup");
//		PreInitialization = (void(*)())GetProcAddress(applicationModule, "PreInitialization");
//
//		if (PreInitialization == nullptr)
//			PreInitialization = []() { return true; };
//
//		if (Setup == nullptr)
//		{
//			BLAZE_LOG_ERROR("Could not find Setup function in application");
//			return 1;
//		}
//#endif
//#endif

		TimingTree initializationTimingTree;

		initializationTimingTree.Start("User pre-initialization");
		if (!PreInitialization())
			return 1;
		initializationTimingTree.End();

		initializationTimingTree.Start("Engine initialization");
		if (!BlazeEngineContext::InitializeEngine({ }, initializationTimingTree))
			return 1;
		initializationTimingTree.End();

		BlazeEngineContext::GetEngineContext()->SetInitializationTimingTree(std::move(initializationTimingTree));

		Main();

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