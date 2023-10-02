#include "source/BlazeEngine/Internal/Libraries/SDL.h"
#include "BlazeEngine/Utilities/Time.h"

#include "SDL2/SDL.h"
//#include "BlazeEngine/Graphics/GraphicsLibraryType.h"

#include "BlazeEngine/Console/Console.h"

namespace Blaze
{
	static const Vec2i winSize = { 640, 360 };

	static SDL_Window* initWindow;

#ifdef BLAZE_GRAPHICS_OPENGL
	static constexpr uint openGLMajorVersion = 4;
	static constexpr uint openGLMinorVersion = 5;
	static constexpr uint openGLProfile = 0; //0 - Core, 1 - Compatibility

	static constexpr uint depthBufferBitCount = 16;

	//Setting this to 0 results in a opengl error !
	static constexpr uint stencilBufferBitCount = 8;
#endif

	SDL_Window* GetInitWindow()
	{
		return initWindow;
	}

	TimingResult InitializeCoreSDL()
	{
		Timing timing{ "SDL initialization" };

		SDL_SetMainReady();
		int state = SDL_InitSubSystem(SDL_INIT_VIDEO);

		if (state != 0)
			throw
			"Failed to initialize the SDL library\n"
			"SDL error code: " + StringView(SDL_GetError());

		SDL_StartTextInput();

		return timing.GetTimingResult();
	}

	TimingResult CreateInitWindow()
	{		
		Timing timing{ "Initialization window creation" };

#ifdef BLAZE_GRAPHICS_OPENGL
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 
			SDL_GL_CONTEXT_DEBUG_FLAG |
			SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG
		);

		if constexpr (openGLProfile == 0)
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		if constexpr (openGLProfile == 1)
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, openGLMajorVersion);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, openGLMinorVersion);

		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depthBufferBitCount);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, stencilBufferBitCount);
#endif

		initWindow = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, winSize.x, winSize.y, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);

		if (initWindow == nullptr)
			Debug::Logger::LogError("SDL", "Failed to create initialization window! SDL error code: " + StringView(SDL_GetError()));		

		return timing.GetTimingResult();
	}

#ifdef BLAZE_GRAPHICS_OPENGL
	static void* openGLContext;

	void* GetOpenGLContext()
	{
		return openGLContext;
	}

	TimingResult CreateOpenGLContext()
	{
		Timing timing{ "OpenGL context creation" };
		openGLContext = SDL_GL_CreateContext(initWindow);

		if (openGLContext == nullptr)
			Debug::Logger::LogError("SDL", "Failed to create OpenGL context. SDL error code: " + StringView(SDL_GetError()));

		int attribute = 0;

		if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &attribute))
			Debug::Logger::LogError("SDL", "Failed to get attribute value. SDL error code: " + StringView(SDL_GetError()));

		const bool debugContext = attribute & SDL_GL_CONTEXT_DEBUG_FLAG;
		const bool forwardCompatibleContext = attribute & SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
		const bool robustContext = attribute & SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG;
		const bool resetIsolationContext = attribute & SDL_GL_CONTEXT_RESET_ISOLATION_FLAG;		

		if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &attribute))
			Debug::Logger::LogError("SDL", "Failed to get attribute value. SDL error code: " + StringView(SDL_GetError()));

		const bool coreProfile = attribute & SDL_GL_CONTEXT_PROFILE_CORE;
		const bool compatibilityProfile = attribute & SDL_GL_CONTEXT_PROFILE_COMPATIBILITY;
		const bool ESProfile = attribute & SDL_GL_CONTEXT_PROFILE_ES;		

		if ((int)coreProfile + (int)compatibilityProfile + (int)ESProfile != 1)
			Debug::Logger::LogError("SDL", "Invalid OpenGL profile!");

		const int profile = (int)compatibilityProfile * 1 + (int)ESProfile * 2;

		if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &attribute))
			Debug::Logger::LogError("SDL", "Failed to get attribute value. SDL error code: " + StringView(SDL_GetError()));

		const int majorVersion = attribute;

		if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &attribute))
			Debug::Logger::LogError("SDL", "Failed to get attribute value. SDL error code: " + StringView(SDL_GetError()));

		const int minorVersion = attribute;
		
		if (SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &attribute))
			Debug::Logger::LogError("SDL", "Failed to get attribute value. SDL error code: " + StringView(SDL_GetError()));
		
		const int depthBufferBitCount = attribute;
		
		if (SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &attribute))
			Debug::Logger::LogError("SDL", "Failed to get attribute value. SDL error code: " + StringView(SDL_GetError()));

		const int stencilBufferBitCount = attribute;

		String openGLReport = "OpenGL version " + StringParsing::Convert(majorVersion) + "." + StringParsing::Convert(minorVersion) + " ";

		switch (profile)
		{
		case 0: openGLReport += "core"; break;
		case 1: openGLReport += "compatibility"; break;
		case 2: openGLReport += "ES"; break;
		default: throw; break;
		}

		openGLReport += " profile with a";

		if (debugContext) openGLReport += " debug,";
		if (forwardCompatibleContext) openGLReport += " forwardCompatible,";
		if (robustContext) openGLReport += " robust,";
		if (resetIsolationContext) openGLReport += " reset isolated,";

		openGLReport.Resize(openGLReport.Size() - 1);

		openGLReport += " context. ";
		
		openGLReport += StringParsing::Convert(depthBufferBitCount) + "bit depth, ";
		openGLReport += StringParsing::Convert(stencilBufferBitCount) + "bit stencil ";

		Console::WriteLine(openGLReport);

		return timing.GetTimingResult();
	}
#endif

	TimingResult InitializeSDL()
	{
		Timing timing{ "SDL" };		
		
		timing.AddNode(InitializeCoreSDL());

//		timing.AddNode(CreateInitWindow());
//		
//#ifdef BLAZE_GRAPHICS_OPENGL
//		timing.AddNode(CreateOpenGLContext());		
//#endif

		return timing.GetTimingResult();
	}

	void TerminateSDL()
	{
//#ifdef BLAZE_GRAPHICS_OPENGL
//		SDL_GL_DeleteContext((SDL_GLContext)openGLContext);
//#endif
//
//		if (initWindow != nullptr)
//			SDL_DestroyWindow(initWindow);

		SDL_StopTextInput();
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
	}
}