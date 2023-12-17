#include "pch.h"
#include "BlazeEngineGraphics\OpenGL/Core/GraphicsContext_OpenGL.h"

namespace Blaze::Graphics::OpenGL
{
	bool glewInitialized = false;	

	void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) 
	{
		StringView _source, _type, _severity, _message{ message, (uintMem)length};

		switch (source)
		{
		case GL_DEBUG_SOURCE_API:             _source = "API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   _source = "Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: _source = "Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     _source = "Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION:     _source = "Application"; break;
		case GL_DEBUG_SOURCE_OTHER:           _source = "Other"; break;
		default: _source = "Unknown"; break;
		}
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:               _type = "Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: _type = "Depricated behaviour"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  _type = "Undefined behaviour"; break;
		case GL_DEBUG_TYPE_PORTABILITY:         _type = "Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE:         _type = "Performance"; break;
		case GL_DEBUG_TYPE_MARKER:              _type = "Marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP:          _type = "Push group"; break;
		case GL_DEBUG_TYPE_POP_GROUP:           _type = "Pop gorup"; break;
		case GL_DEBUG_TYPE_OTHER:               _type = "Other"; break;
		default: _type = "Unknown"; break;
		}
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_LOW:    _severity = "Low"; break;
		case GL_DEBUG_SEVERITY_MEDIUM: _severity = "Medium"; break;
		case GL_DEBUG_SEVERITY_HIGH:   _severity = "High"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: _severity = "Notification"; break;
		default: _severity = "Unknown"; break;
		}		
		
		decltype(&Debug::Logger::LogError) logFunc;
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:               
		default:
			logFunc = Debug::Logger::LogError; 
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: 
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		case GL_DEBUG_TYPE_PORTABILITY:       
		case GL_DEBUG_TYPE_PERFORMANCE:       
		case GL_DEBUG_TYPE_OTHER:             
		case GL_DEBUG_TYPE_PUSH_GROUP:        
		case GL_DEBUG_TYPE_POP_GROUP:         
			logFunc = Debug::Logger::LogWarning; 
			break;
		case GL_DEBUG_TYPE_MARKER:            
			logFunc = Debug::Logger::LogDebug; 
			break;
		}

		logFunc("OpenGL (" + _source + ")", "Type: " + _type + " | Severity: " + _severity + " | " + _message);		
	}

	GraphicsContext_OpenGL::GraphicsContext_OpenGL()
		: GraphicsContext_OpenGL(GraphicsContextProperties_OpenGL())
	{
	}

	GraphicsContext_OpenGL::GraphicsContext_OpenGL(const GraphicsContextProperties_OpenGL& properties) :
		currentWindowHandle(nullptr),
		majorVersion(properties.majorVersion),
		minorVersion(properties.minorVersion),
		profile(properties.profile),
		depthBufferBitCount(properties.depthBufferBitCount),
		stencilBufferBitCount(properties.stencilBufferBitCount)
	{
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
			SDL_GL_CONTEXT_DEBUG_FLAG |
			SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG
		);

		SDL_GLprofile _profile;
		switch (profile)
		{
		case Blaze::Graphics::OpenGL::GraphicsContextProperties_OpenGL::ProfileType::Core: _profile = SDL_GL_CONTEXT_PROFILE_CORE; break;
		case Blaze::Graphics::OpenGL::GraphicsContextProperties_OpenGL::ProfileType::Compatibility: _profile = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY; break;			
		}

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, _profile);

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, majorVersion);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minorVersion);

		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depthBufferBitCount);

		initWindow = WindowSDL("", Vec2i(INT_MAX), Vec2u(640, 480), WindowHandleGraphicsaAPI::OpenGL);

		context = SDL_GL_CreateContext((SDL_Window*)initWindow.GetHandle());

		if (!glewInitialized)
		{
			glewExperimental = true;
			glewInit();			
			glewInitialized = true;
		}

		glDebugMessageCallback(MessageCallback, nullptr);					
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(MessageCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);		

		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
		glBlendEquation(GL_FUNC_ADD);

		glClearColor(0, 0, 0, 1);

	}		
	GraphicsContext_OpenGL::~GraphicsContext_OpenGL()
	{
		SDL_GL_DeleteContext(context);

		initWindow.Destroy();
		context = NULL;
	}

	WindowSDL GraphicsContext_OpenGL::CreateWindow(String title, Vec2i pos, Vec2u size)
	{
		if (initWindow.IsNullWindow())
			return WindowSDL(title, pos, size, WindowHandleGraphicsaAPI::OpenGL);
		else
			return std::move(initWindow);
	}

	void GraphicsContext_OpenGL::DestroyWindow(WindowSDL& window)
	{
		WindowSDL::WindowSDLHandle currentWindowHandle = SDL_GL_GetCurrentWindow();

		if (currentWindowHandle == window.GetHandle() && initWindow.IsNullWindow())
			initWindow = std::move(window);
		else
			window.Destroy();
	}
}