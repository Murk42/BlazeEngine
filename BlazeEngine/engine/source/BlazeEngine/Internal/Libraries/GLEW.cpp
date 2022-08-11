#include "source/BlazeEngine/Internal/Libraries/GLEW.h"
#include "BlazeEngine/Utilities/Time.h"

#include "GL/glew.h"
#include "SDL/SDL.h"

namespace Blaze
{
	namespace Logger
	{
		void OpenGLCallbackFunc(unsigned source, unsigned type, int id, unsigned severity, unsigned lenght, const char* message);
	}

	static void* openGLInitWindow;
	static void* openGLContext;

	void* GetOpenGLInitWindow()
	{
		return openGLInitWindow;
	}
	void* GetOpenGLContext()
	{
		return openGLContext;
	}

	Startup::BlazeLibrariesInitInfo::GLEWInitInfo InitializeGLEW()
	{
		Startup::BlazeLibrariesInitInfo::GLEWInitInfo initInfo;
		TimePoint startTimePoint = TimePoint::GetWorldTime();

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);		

		TimePoint otherTimePoint = TimePoint::GetWorldTime();
		const Vec2i winSize = { 640, 360 };
		openGLInitWindow = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, winSize.x, winSize.y, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);				
		initInfo.windowInitTime = TimePoint::GetWorldTime() - otherTimePoint;

		if (openGLInitWindow == nullptr)
			throw
			"Failed to create initialization window!\n"
			"SDL error code: " + StringView(SDL_GetError());

		otherTimePoint = TimePoint::GetWorldTime();
		openGLContext = SDL_GL_CreateContext((SDL_Window*)openGLInitWindow);
		initInfo.contextInitTime = TimePoint::GetWorldTime() - otherTimePoint;

		if (openGLContext == nullptr)
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
		
		
		glDepthFunc(GL_LEQUAL);
		//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
		
		initInfo.initTime = TimePoint::GetWorldTime() - startTimePoint;
		return initInfo;

	}
	void TerminateGLEW()
	{
		SDL_GL_DeleteContext((SDL_GLContext)openGLContext);
		if (openGLInitWindow != nullptr)
			SDL_DestroyWindow((SDL_Window*)openGLInitWindow);
	}
}