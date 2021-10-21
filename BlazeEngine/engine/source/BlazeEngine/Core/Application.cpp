#include "source/BlazeEngine/Internal/Engine.h"
#include "BlazeEngine/Core/Application.h"

#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/Core/Logger.h"
#include "BlazeEngine/Console/Console.h"

#include "SDL/SDL.h"
#include "PDC/curses.h"
#include "GL/glew.h"
#include "IL/il.h"
#include "IL/ilu.h"
#include "freetype/freetype.h"

#ifdef __WINDOWS__
#include <Windows.h>
#endif

#undef GetMessage

namespace Blaze
{
	namespace EngineInitialization
	{
		void(*construct)(BlazeEngineBase*) = nullptr;
		void(*destruct)(BlazeEngineBase*) = nullptr;
		size_t size = 0;
	}	

	void BlazeEngineBase::Initialize(Constructor c, Destructor d, size_t size)
	{		
		EngineInitialization::construct = c;
		EngineInitialization::destruct = d;
		EngineInitialization::size = size;
	}

	enum class StateFlags
	{
		Initialized = 0x1,
		Running = 0x2,
	};

	enum class InitFlags
	{
		SDL			= 0x1,
		GLEW		= 0x2,
		DevIL		= 0x4,
		PDC			= 0x8,
		FreeType	= 0x10,
		Blaze		= 0x20,
	};

	namespace Logger
	{
		void OpenGLCallbackFunc(unsigned, unsigned, int, unsigned, unsigned, const char*);
	}	

	namespace Console
	{
		void ConsoleInputThreadFunction();
	}	

	namespace Input
	{
		void Update();
	}

	namespace Application
	{
		void Stop()
		{
			engine->App.state ^= (uint32)StateFlags::Running;
		}

		void SetTargetFramerate(uint fps)
		{
			engine->App.targetDeltaTime = 1.0f / double(fps);
		}
		void SetTargetDeltaTime(double deltaTime)
		{
			engine->App.targetDeltaTime = deltaTime;
		}		

		uint GetTargetDeltaTime()
		{
			return engine->App.targetDeltaTime;
		}
		uint GetFPS()
		{
			return engine->App.FPS;
		}
		double GetDeltaTime()
		{
			return engine->App.deltaTime;
		}
	}

	void BlazeEngineBase::NewLog(const Log& log)
	{
		Console::Write("(file: \"" + log.GetFilePath() + "\")\n[" + String::Convert(log.GetThreadID()) + "] (" + log.GetFunctionName() + ", " + String::Convert(log.GetLine()) + ") " + log.GetSource() + ":\n" + log.GetMessage() + "\n");
	}	

	void InitializeDevIL()
	{
		ILenum error;

		ilInit();

		error = ilGetError();
		if (error != IL_NO_ERROR)
			throw 
				"Failed to initialize the DevIL library!\n"
				"DevIL error code: " + String::Convert(error);

		iluInit();

		error = ilGetError();
		if (error != IL_NO_ERROR)
			throw 
				"Failed to initialize the DevIL library!\n"
				"DevIL error code: " + String::Convert(error);
	}
	void TerminateDevIL()
	{
		ilShutDown();
	}
	void InitializeSDL()
	{
		SDL_SetMainReady();
		int state = SDL_Init(SDL_INIT_VIDEO);		

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
	void InitializeGLEW()
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

		engine->App.initWindow = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);

		if (engine->App.initWindow == nullptr)
			throw 
				"Failed to create initialization window!\n"
				"SDL error code: " + StringView(SDL_GetError());

		engine->App.openGLContext = SDL_GL_CreateContext((SDL_Window*)engine->App.initWindow);

		if (engine->App.openGLContext == nullptr)
			throw 
				"Failed to create OpenGL context!\n"
				"SDL error code: " + StringView(SDL_GetError());

		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
			throw
				"Failed to initialize the GLEW library!\n"
				"GLEW error code: " + String::Convert(glGetError());		

		//Enable callback debug
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback((GLDEBUGPROC)Logger::OpenGLCallbackFunc, nullptr);

		//Default settings
		glFrontFace(GL_CW);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);		
		glDepthMask(GL_TRUE);		
		glEnable(GL_DEPTH_TEST);		
		
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	void TerminateGLEW()
	{
		SDL_GL_DeleteContext((SDL_GLContext)engine->App.openGLContext);
		if (engine->App.initWindow != nullptr)
			SDL_DestroyWindow((SDL_Window*)engine->App.initWindow);
	}
	void InitializePDC()
	{
		//initscr();
	}
	void TerminatePDC()
	{
		//endwin();		
	}
	void InitializeFreeType()
	{
		if (FT_Init_FreeType(&engine->ft_library) != 0)
			throw String("Failed to initialize the FreeType libary");
	}
	void TerminateFreeType()
	{
		FT_Done_FreeType(engine->ft_library);
	}
	void InitializeBlaze()
	{
		SDL_GetGlobalMouseState(&engine->Input.mousePos.x, &engine->Input.mousePos.y);

		engine->Core.threadIDs[std::this_thread::get_id()] = 0;

		//Input
		{
			engine->Console.Input.thread = std::move(std::thread(Console::ConsoleInputThreadFunction));

			for (auto& keyState : engine->Input.keyStates)
			{
				keyState.timePressed = 0;
				keyState.combo = 0;
				keyState.down = false;
				keyState.pressed = false;
				keyState.released = false;
			}
			for (auto& keyState : engine->Input.mouseKeyStates)
			{
				keyState.timePressed = 0;
				keyState.combo = 0;
				keyState.down = false;
				keyState.pressed = false;
				keyState.released = false;
			}


		}

		engine->AppInstance.constructor = EngineInitialization::construct;		
		engine->AppInstance.destructor = EngineInitialization::destruct;
		engine->AppInstance.size = EngineInitialization::size;
		BlazeEngineBase* instance = (BlazeEngineBase*)new uint8[engine->AppInstance.size];
		engine->AppInstance.ptr = instance;
		engine->AppInstance.constructor(instance);
		
		Engine::Cache::InitializeRenderTextureCache();
		Engine::Cache::InitializeRenderTextCache();
		Engine::Cache::InitializeRenderSDFCache();
	}
	void TerminateBlaze()
	{	
		Engine::Cache::DeinitializeRenderSDFCache();
		Engine::Cache::DeinitializeRenderTextCache();
		Engine::Cache::DeinitializeRenderTextureCache();		

		engine->Console.Input.m.lock();
		engine->Console.Input.exit = true;
		engine->Console.Input.m.unlock();
		engine->Console.Input.thread.detach();			

		engine->AppInstance.destructor(engine->AppInstance.ptr);
		delete[] (uint8*)engine->AppInstance.ptr;
	}

	void Initialize()
	{
		try
		{
			InitializeDevIL();
			engine->App.initState |= (uint32)InitFlags::DevIL;

			InitializeSDL();
			engine->App.initState |= (uint32)InitFlags::SDL;

			InitializeGLEW();
			engine->App.initState |= (uint32)InitFlags::GLEW;

			InitializePDC();
			engine->App.initState |= (uint32)InitFlags::PDC;

			InitializeFreeType();
			engine->App.initState |= (uint32)InitFlags::FreeType;

			InitializeBlaze();
			engine->App.initState |= (uint32)InitFlags::Blaze;
		}
		catch (const String& message)
		{
			throw message;
		}
	}
	void Terminate()
	{
		if (engine->App.initState & (uint32)InitFlags::Blaze)
		{
			TerminateBlaze();
			engine->App.initState ^= (uint32)InitFlags::Blaze;
		}
		if (engine->App.initState & (uint32)InitFlags::FreeType)
		{
			TerminateFreeType();
			engine->App.initState ^= (uint32)InitFlags::FreeType;
		}
		if (engine->App.initState & (uint32)InitFlags::PDC)
		{
			TerminatePDC();
			engine->App.initState ^= (uint32)InitFlags::PDC;
		}
		if (engine->App.initState & (uint32)InitFlags::GLEW)
		{
			TerminateGLEW();
			engine->App.initState ^= (uint32)InitFlags::GLEW;
		}
		if (engine->App.initState & (uint32)InitFlags::SDL)
		{
			TerminateSDL();
			engine->App.initState ^= (uint32)InitFlags::SDL;
		}
		if (engine->App.initState & (uint32)InitFlags::DevIL)
		{
			TerminateDevIL();
			engine->App.initState ^= (uint32)InitFlags::DevIL;
		}
	}

	void Run()
	{
		engine->AppInstance.ptr->Startup();

		while (engine->App.state & (uint32)StateFlags::Running)
		{
			std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> frameDuration = now - engine->App.lastFrame;
			engine->App.deltaTime = frameDuration.count();
			engine->App.lastFrame = now;
						

			if (engine->App.FPSstopwatch.GetTime() > 1.0f)
			{
				engine->App.FPS = engine->App.FPScounter;
				engine->App.FPScounter = 0;
				engine->App.FPSstopwatch.Reset();
			}
			++engine->App.FPScounter;


			Input::Update();

			engine->AppInstance.ptr->Frame();

			std::this_thread::sleep_until(now + std::chrono::duration<double>(std::chrono::microseconds(uint(engine->App.targetDeltaTime * 1000000))));			
		}

		engine->AppInstance.ptr->Cleanup();
	}
}

int main()
{	
	Blaze::Buffer engineBuffer;

	engineBuffer.Allocate(sizeof(Blaze::Engine));
	new (engineBuffer.Ptr())Blaze::Engine();
	
	Blaze::engine = (Blaze::Engine*)engineBuffer.Ptr();
	try
	{
		Blaze::Initialize();
		Blaze::engine->App.state |= (Blaze::uint32)Blaze::StateFlags::Initialized;
		Blaze::engine->App.state |= (Blaze::uint32)Blaze::StateFlags::Running;

		Blaze::Run();

		Blaze::Terminate();
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

	((Blaze::Engine*)engineBuffer.Ptr())->~Engine();
	engineBuffer.Free();

	return 0;
}