#include "Engine.h"
#include "BlazeEngine/Core/Application.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/Vector.h"

#include "BlazeEngine/Core/Logger.h"

#define SDL_MAIN_HANDLED
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
	enum class StateFlags
	{
		Initialized = 0x1,
		Running = 0x2,
	};
	extern uint32 state;

	enum class InitFlags
	{
		SDL			= 0x1,
		GLEW		= 0x2,
		DevIL		= 0x4,
		PDC			= 0x8,
		FreeType	= 0x10,
		Blaze		= 0x20,
	};
	extern uint32 initState;
	

	extern BaseApplication* instance;
	extern void (*constructInstance)(BaseApplication*);
	extern void (*destructInstance)(BaseApplication*);
	extern size_t instanceSize;
	
	extern void* initWindow;
	extern void* openGLContext;

	extern FT_Library ft_library;
	
	namespace Input
	{
		extern Vec2i mousePos;
	}

	namespace Logger
	{
		void OpenGLCallbackFunc(unsigned, unsigned, int, unsigned, unsigned, const char*);
	}

	void BaseApplication::Setup(void(*construct)(BaseApplication*), void(*destruct)(BaseApplication*), size_t size)
	{
		constructInstance = construct;
		destructInstance = destruct;
		instanceSize = size;
	}

	void BaseApplication::Stop()
	{
		
		state ^= (uint32)StateFlags::Running;
	}

	BaseApplication* BaseApplication::Instance()
	{
		return instance;
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

		initWindow = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);

		if (initWindow == nullptr)
			throw String(format_string,
				"Failed to create initialization window!\n"
				"SDL error code: %i", SDL_GetError());

		openGLContext = SDL_GL_CreateContext((SDL_Window*)initWindow);

		if (openGLContext == nullptr)
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
		SDL_GL_DeleteContext((SDL_GLContext)openGLContext);
		if (initWindow != nullptr)
			SDL_DestroyWindow((SDL_Window*)initWindow);
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
		if (FT_Init_FreeType(&ft_library) != 0)
			throw String("Failed to initialize the FreeType libary");
	}
	void TerminateFreeType()
	{
		FT_Done_FreeType(ft_library);
	}
	void InitializeBlaze()
	{
		SDL_GetGlobalMouseState(&Input::mousePos.x, &Input::mousePos.y);		

		start_color();
		noecho();
		cbreak();
		nodelay(stdscr, true);
		keypad(stdscr, true);
		curs_set(0);

		for (short f = 0; f < 8; f++)
			for (short b = 0; b < 8; b++)
				init_pair(f + b * 8, f, b);

		instance = (BaseApplication*)new uint8[instanceSize];
		constructInstance(instance);		
	}
	void TerminateBlaze()
	{		
		destructInstance(instance);
		delete[](uint8*)instance;
	}


	void Initialize()
	{
		try
		{
			InitializeDevIL();
			initState |= (uint32)InitFlags::DevIL;

			InitializeSDL();
			initState |= (uint32)InitFlags::SDL;

			InitializeGLEW();
			initState |= (uint32)InitFlags::GLEW;

			InitializePDC();
			initState |= (uint32)InitFlags::PDC;

			InitializeFreeType();
			initState |= (uint32)InitFlags::FreeType;

			InitializeBlaze();
			initState |= (uint32)InitFlags::Blaze;
		}
		catch (const String& message)
		{
			throw message;
		}
	}
	void Terminate()
	{
		if (initState & (uint32)InitFlags::Blaze)
		{
			TerminateBlaze();
			initState ^= (uint32)InitFlags::Blaze;
		}
		if (initState & (uint32)InitFlags::FreeType)
		{
			TerminateFreeType();
			initState ^= (uint32)InitFlags::FreeType;
		}
		if (initState & (uint32)InitFlags::PDC)
		{
			TerminatePDC();
			initState ^= (uint32)InitFlags::PDC;
		}
		if (initState & (uint32)InitFlags::GLEW)
		{
			TerminateGLEW();
			initState ^= (uint32)InitFlags::GLEW;
		}
		if (initState & (uint32)InitFlags::SDL)
		{
			TerminateSDL();
			initState ^= (uint32)InitFlags::SDL;
		}
		if (initState & (uint32)InitFlags::DevIL)
		{
			TerminateDevIL();
			initState ^= (uint32)InitFlags::DevIL;
		}
	}

	void Run()
	{
		instance->Startup();

		while (state & (uint32)StateFlags::Running)
			instance->Frame();

		instance->Cleanup();
	}
}

int main()
{
	try
	{
		Blaze::Initialize();
		Blaze::state |= (Blaze::uint32)Blaze::StateFlags::Initialized;
		Blaze::state |= (Blaze::uint32)Blaze::StateFlags::Running;

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
}