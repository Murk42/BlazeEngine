#include "BlazeEngine/Application/Application.h"

#include "source/BlazeEngine/Internal/Libraries.h"
#include "BlazeEngine/Console/Console.h"
#include "BlazeEngine/Core/MemoryManager.h"
#include "BlazeEngine/Graphics/Graphics.h"

namespace Blaze 
{			
	void InitializeConsole();
	void TerminateConsole();
	void InitializeInput();
	void TerminateInput();
	void InitializeRenderer();
	void TerminateRenderer();
	void InitializeGraphics();
	void TerminateGraphics();	
	//void InitializeLayerManager();
	//void TerminateLayerManager();

	//static bool appClassInfoSet = false;
	//static BlazeApplicationClassInfo appClassInfo;
	//
	//void SetBlazeApplicationClassInfo(BlazeApplicationClassInfo appClassInfo)
	//{
	//	Blaze::appClassInfo = appClassInfo;
	//	appClassInfoSet = true;
	//}				
}

//extern "C" BLAZE_API void InvalidateApplicationClassInfo()
//{
//	Blaze::appClassInfoSet = false;
//}
//
//extern "C" BLAZE_API bool GetApplicationClassInfo(Blaze::BlazeApplicationClassInfo*& info)
//{
//	if (Blaze::appClassInfoSet)
//	{
//		info = &Blaze::appClassInfo;
//		return true;
//	}
//	return false;
//}

extern "C" BLAZE_API void InitializeBlaze()
{
	std::cout << "BlazeEngine: Initializing libraries\n";
	Blaze::InitializeLibraries();
	std::cout << "BlazeEngine: Initializing console\n";
	Blaze::InitializeConsole();
	std::cout << "BlazeEngine: Initializing input\n";
	Blaze::InitializeInput();
	std::cout << "BlazeEngine: Initializing renderer\n";
	Blaze::InitializeRenderer();
	std::cout << "BlazeEngine: Initializing graphics\n";
	Blaze::InitializeGraphics();
}
extern "C" BLAZE_API void TerminateBlaze()
{
	std::cout << "BlazeEngine: Terminating graphics\n";
	Blaze::TerminateGraphics();
	std::cout << "BlazeEngine: Terminating renderer\n";
	Blaze::TerminateRenderer();
	std::cout << "BlazeEngine: Terminating input\n";
	Blaze::TerminateInput();
	std::cout << "BlazeEngine: Terminating console\n";
	Blaze::TerminateConsole();
	std::cout << "BlazeEngine: Terminating libraries\n";
	Blaze::TerminateLibraries();
}

/*
int main()
{	
	try
	{			
		if (!Blaze::appClassInfoSet)
			throw "No application class created";

		constexpr bool allocateEngineOnStack = true;
		constexpr bool allocateAppOnStack = true;

		void* engine;
		void* app;

		Blaze::InitializeLibraries();		
		
		if (allocateEngineOnStack)
			engine = alloca(sizeof(Blaze::Engine));
		else
			engine = malloc(sizeof(Blaze::Engine));		

		if (allocateAppOnStack)
			app = alloca(Blaze::appClassInfo.size);
		else
			app = malloc(Blaze::appClassInfo.size);

		new (engine) Blaze::Engine();		

		Blaze::InitializeBlaze();

		Blaze::appClassInfo.Constructor(app);		
		
		Blaze::appClassInfo.Setup(app);

		while (Blaze::engine->ProcessInfo.running)
		{
			Blaze::Input::Update();

			Blaze::Renderer::ClearTarget();
			Blaze::LayerManager::RenderLayers();
			Blaze::Graphics::Flush();
			Blaze::Renderer::SwapWindowBuffers();
		}
		
		Blaze::appClassInfo.Destructor(app);

		Blaze::TerminateBlaze();

		((Blaze::Engine*)engine)->~Engine();

		if (!allocateAppOnStack)
			free(app);
		if (!allocateEngineOnStack)
			free(engine);

		Blaze::TerminateLibraries();
	}
	catch (const Blaze::String& message)
	{
#ifdef __WINDOWS__	
		HWND hwnd = GetActiveWindow();
		MessageBox(NULL, (LPCSTR)message.Ptr(), NULL, MB_ICONERROR | MB_OK);
#endif
		return 0;
	}
	catch (const char* message)
	{
#ifdef __WINDOWS__	
		HWND hwnd = GetActiveWindow();
		MessageBox(NULL, (LPCSTR)message, NULL, MB_ICONERROR | MB_OK);
#endif
		return 0;
	}
	catch (const std::exception& exception)
	{
#ifdef __WINDOWS__	
		HWND hwnd = GetActiveWindow();
		MessageBox(NULL, (LPCSTR)exception.what(), NULL, MB_ICONERROR | MB_OK);
#endif
		return 0;
	}
	catch (...)
	{
#ifdef __WINDOWS__	
		HWND hwnd = GetActiveWindow();
		MessageBox(NULL, "An unknown exception was thrown", NULL, MB_ICONERROR | MB_OK);
#endif
		return 0;
	}	

	return 0;
}*/