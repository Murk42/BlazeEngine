#include "Engine.h"
#include "BlazeEngine/Core/Application.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/Vector.h"

#include "BlazeEngine/Core/Logger.h"

#include "SDL/SDL.h"
#include "PDC/curses.h"
#include "GL/glew.h"
#include "IL/il.h"
#include "IL/ilu.h"
#include "freetype/freetype.h"

#ifdef __WINDOWS__
#include <Windows.h>
#endif

namespace Blaze
{
	namespace EngineInitialization
	{
		void(*construct)(BaseApplication*) = nullptr;
		void(*destruct)(BaseApplication*) = nullptr;
		size_t size = 0;
	}

	void BaseApplication::Initialize(Constructor c, Destructor d, size_t size)
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

	void BaseApplication::Stop()
	{
		engine->App.state ^= (uint32)StateFlags::Running;
	}

	BaseApplication& BaseApplication::Instance()
	{
		return *engine->AppInstance.ptr;
	}

	using namespace Blaze;

	void InitializeDevIL()
	{
		ILenum error;

		ilInit();

		error = ilGetError();
		if (error != IL_NO_ERROR)
			throw String(format_string,
				"Failed to initialize the DevIL library!\n"
				"DevIL error code: %i", error);

		iluInit();

		error = ilGetError();
		if (error != IL_NO_ERROR)
			throw String(format_string,
				"Failed to initialize the DevIL library!\n"
				"DevIL error code: %i", error);
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
			throw String(format_string,
				"Failed to initialize the SDL library\n"
				"SDL error code: %i", SDL_GetError());
	}
	void TerminateSDL()
	{
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
			throw String(format_string,
				"Failed to create initialization window!\n"
				"SDL error code: %i", SDL_GetError());

		engine->App.openGLContext = SDL_GL_CreateContext((SDL_Window*)engine->App.initWindow);

		if (engine->App.openGLContext == nullptr)
			throw String(format_string,
				"Failed to create OpenGL context!\n"
				"SDL error code: %i", SDL_GetError());

		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
			throw String(format_string,
				"Failed to initialize the GLEW library!\n"
				"GLEW error code: %i", glGetError());

		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback((GLDEBUGPROC)Logger::OpenGLCallbackFunc, nullptr);
	}
	void TerminateGLEW()
	{
		SDL_GL_DeleteContext((SDL_GLContext)engine->App.openGLContext);
		if (engine->App.initWindow != nullptr)
			SDL_DestroyWindow((SDL_Window*)engine->App.initWindow);
	}
	void InitializePDC()
	{
		initscr();
	}
	void TerminatePDC()
	{
		endwin();
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

		start_color();
		noecho();
		cbreak();
		nodelay(stdscr, true);
		keypad(stdscr, true);
		curs_set(0);

		for (short f = 0; f < 8; f++)
			for (short b = 0; b < 8; b++)
				init_pair(f + b * 8, f, b);

		engine->AppInstance.constructor = EngineInitialization::construct;		
		engine->AppInstance.destructor = EngineInitialization::destruct;
		engine->AppInstance.size = EngineInitialization::size;
		BaseApplication* instance = (BaseApplication*)new uint8[engine->AppInstance.size];
		engine->AppInstance.ptr = instance;
		engine->AppInstance.constructor(instance);
	}
	void TerminateBlaze()
	{		
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
			engine->AppInstance.ptr->Frame();

		engine->AppInstance.ptr->Cleanup();
	}
}

int main()
{
	Blaze::Engine engineValue;
	Blaze::engine = &engineValue;
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

	return 0;
}