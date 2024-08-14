#include "pch.h"
#include "BlazeEngineGraphics/Core/OpenGL/GraphicsContext_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/RenderWindow_OpenGL.h"

#include "BlazeEngine/Console/Console.h"

namespace Blaze::Graphics::OpenGL
{
	bool glewInitialized = false;	

	void UnsetActiveOpenGLGraphicsContext(GraphicsContext_OpenGL& graphicsContext);

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
		case GL_DEBUG_SEVERITY_NOTIFICATION: _severity = "Notification"; return; break;
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
			return;
			logFunc = Debug::Logger::LogWarning; 
			break;
		case GL_DEBUG_TYPE_MARKER:            
			logFunc = Debug::Logger::LogDebug; 
			break;
		}

		logFunc("OpenGL (" + _source + ")", "Type: " + _type + " | Severity: " + _severity + " | " + _message);		
	}

	static StringView GetProfileTypeName(GLenum profile)
	{
		switch (profile)
		{
		case SDL_GL_CONTEXT_PROFILE_COMPATIBILITY:
			return "compatibility";
			break;
		case SDL_GL_CONTEXT_PROFILE_CORE:
			return "core";
			break;
		case SDL_GL_CONTEXT_PROFILE_ES:
			return "ES";
		default:
			return StringView();
		}	
	}
	static ProfileType GetProfileType(GLenum profileType)
	{
		switch (profileType)
		{
		case SDL_GL_CONTEXT_PROFILE_CORE: return Blaze::Graphics::OpenGL::ProfileType::Core;
		case SDL_GL_CONTEXT_PROFILE_COMPATIBILITY: return Blaze::Graphics::OpenGL::ProfileType::Compatibility;
		case SDL_GL_CONTEXT_PROFILE_ES: return Blaze::Graphics::OpenGL::ProfileType::ES;
		default: return std::numeric_limits<ProfileType>::max();
		}
	}
	static StringView GetOpenGLProfileTypeString(GLenum profile)
	{
		switch (profile)
		{
		case SDL_GL_CONTEXT_PROFILE_COMPATIBILITY:
			return "SDL_GL_CONTEXT_PROFILE_COMPATIBILITY";
			break;
		case SDL_GL_CONTEXT_PROFILE_CORE:
			return "SDL_GL_CONTEXT_PROFILE_CORE";
			break;
		case SDL_GL_CONTEXT_PROFILE_ES:
			return "SDL_GL_CONTEXT_PROFILE_ES";
		default:
			return StringView();
		}
	}
	static GLenum GetOpenGLProfileType(ProfileType profileType)
	{
		switch (profileType)
		{
		case Blaze::Graphics::OpenGL::ProfileType::Core: return SDL_GL_CONTEXT_PROFILE_CORE;
		case Blaze::Graphics::OpenGL::ProfileType::Compatibility: return SDL_GL_CONTEXT_PROFILE_COMPATIBILITY;
		case Blaze::Graphics::OpenGL::ProfileType::ES: return SDL_GL_CONTEXT_PROFILE_ES;
		default: return 0;
		}
	}

	static String GetContextFlagsText(GLenum contextFlags)
	{
		String out;

		if (contextFlags & SDL_GL_CONTEXT_DEBUG_FLAG)
		{
			if (!out.Empty()) out += ", ";
			out += "debug";
		}
		if (contextFlags & SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG)
		{
			if (!out.Empty()) out += ", ";
			out += "forward compatible";
		}
		if (contextFlags & SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG)
		{
			if (!out.Empty()) out += ", ";
			out = "robust access";
		}
		if (contextFlags & SDL_GL_CONTEXT_RESET_ISOLATION_FLAG)
		{
			if (!out.Empty()) out += ", ";
			out = "reset isolation";
		}

		return out;
	}
	static ContextFlags GetContextFlags(GLenum contextFlags)
	{
		ContextFlags out = ContextFlags::None;

		if (bool(contextFlags & SDL_GL_CONTEXT_DEBUG_FLAG))
			out |= ContextFlags::Debug;
		if (bool(contextFlags & SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG))
			out |= ContextFlags::ForwardCompatible;
		if (bool(contextFlags & SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG))
			out |= ContextFlags::RobustAccess;
		if (bool(contextFlags & SDL_GL_CONTEXT_RESET_ISOLATION_FLAG))
			out |= ContextFlags::ResetIsolation;

		return out;
	}
	static String GetOpenGLContextFlagsString(GLenum contextFlags)
	{
		String out;

		if (contextFlags & SDL_GL_CONTEXT_DEBUG_FLAG)
		{
			if (!out.Empty()) out += " | ";
			out += "SDL_GL_CONTEXT_DEBUG_FLAG";
		}
		if (contextFlags & SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG)
		{
			if (!out.Empty()) out += " | ";
			out += "SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG";
		}
		if (contextFlags & SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG)
		{
			if (!out.Empty()) out += " | ";
			out = "SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG";
		}
		if (contextFlags & SDL_GL_CONTEXT_RESET_ISOLATION_FLAG)
		{
			if (!out.Empty()) out += " | ";
			out = "SDL_GL_CONTEXT_RESET_ISOLATION_FLAG";
		}

		return out;
	}
	static GLenum GetOpenGLContextFlags(ContextFlags contextFlags)
	{
		GLenum out = 0;

		if (bool(contextFlags & Blaze::Graphics::OpenGL::ContextFlags::Debug))
			out |= SDL_GL_CONTEXT_DEBUG_FLAG;
		if (bool(contextFlags & Blaze::Graphics::OpenGL::ContextFlags::ForwardCompatible))
			out |= SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
		if (bool(contextFlags & Blaze::Graphics::OpenGL::ContextFlags::RobustAccess))
			out |= SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG;
		if (bool(contextFlags & Blaze::Graphics::OpenGL::ContextFlags::ResetIsolation))
			out |= SDL_GL_CONTEXT_RESET_ISOLATION_FLAG;				
		
		return out;
	}

	static StringView GetOpenGLReleaseBehaviourString(GLenum releaseBehaviour)
	{
		switch (releaseBehaviour)
		{
		case SDL_GL_CONTEXT_RELEASE_BEHAVIOR_NONE:
			return "SDL_GL_CONTEXT_RELEASE_BEHAVIOR_NONE";
		case SDL_GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH:
			return "SDL_GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH";
		default:
			return "Invalid value";
		}
	}
	static GLenum GetOpenGLReleaseBehaviour(ReleaseBehaviour releaseBehaviour)
	{
		switch (releaseBehaviour)
		{
		case Blaze::Graphics::OpenGL::ReleaseBehaviour::None:
			return SDL_GL_CONTEXT_RELEASE_BEHAVIOR_NONE;			
		case Blaze::Graphics::OpenGL::ReleaseBehaviour::Flush:
			return SDL_GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH;
		default:
			return 0;			
		}
	}

	static StringView GetSDLError()
	{
		const char* ptr = SDL_GetError();
		uintMem len = strlen(ptr);
		return StringView(ptr, len);
	}
	static StringView GetGlewError(GLenum error)
	{
		const char* ptr = (const char*)glewGetErrorString(error);
		uintMem len = strlen(ptr);
		return StringView(ptr, len);
	}
	static StringView GetGlewString(GLenum value)
	{
		const char* ptr = (const char*)glewGetString(value);
		uintMem len = strlen(ptr);
		return StringView(ptr, len);
	}

	GraphicsContext_OpenGL::GraphicsContext_OpenGL()
		: GraphicsContext_OpenGL(GraphicsContextProperties_OpenGL())
	{
	}
	GraphicsContext_OpenGL::GraphicsContext_OpenGL(const GraphicsContextProperties_OpenGL& properties) :
		activeWindowSDLHandle(nullptr),
		majorVersion(properties.majorVersion),
		minorVersion(properties.minorVersion),
		profileType(properties.profileType),
		contextFlags(properties.contextFlags),
		releaseBehaviour(properties.releaseBehaviour),
		depthBufferBitCount(properties.depthBufferBitCount),
		stencilBufferBitCount(properties.stencilBufferBitCount)
	{
		SDL_GL_ResetAttributes();

		if (SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 0) < 0)
		{
			Debug::Logger::LogFatal("Blaze Engine Graphics", "Failed to set OpenGL context attribute SDL_GL_ACCELERTED_VISUAL to 1. SDL_Error() returned: \"" + GetSDLError() + "\"");
			return;
		}
		
		GLenum _contextFlags = GetOpenGLContextFlags(contextFlags);
		if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, _contextFlags))
		{
			Debug::Logger::LogFatal("Blaze Engine Graphics", "Failed to set OpenGL context attribute SDL_GL_CONTEXT_FLAGS to " + GetOpenGLContextFlagsString(_contextFlags) + ".SDL_Error() returned: \"" + GetSDLError() + "\"");
			return;
		}

		GLenum _profileType = GetOpenGLProfileType(profileType);
		if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, _profileType) < 0)
		{
			Debug::Logger::LogFatal("Blaze Engine Graphics", "Failed to set OpenGL context attribute SDL_GL_CONTEXT_PROFILE_MASK to " + GetOpenGLProfileTypeString(_profileType) + ".SDL_Error() returned: \"" + GetSDLError() + "\"");
			return;
		}

		GLenum _releaseBehaviour = GetOpenGLReleaseBehaviour(releaseBehaviour);
		if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_RELEASE_BEHAVIOR, _releaseBehaviour) < 0)
		{
			Debug::Logger::LogFatal("Blaze Engine Graphics", "Failed to set OpenGL context attribute SDL_GL_CONTEXT_RELEASE_BEHAVIOR to " + GetOpenGLReleaseBehaviourString(_releaseBehaviour) + ".SDL_Error() returned: \"" + GetSDLError() + "\"");
			return;
		}

		if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, majorVersion) < 0)
		{
			Debug::Logger::LogFatal("Blaze Engine Graphics", "Failed to set OpenGL context attribute SDL_GL_CONTEXT_MAJOR_VERSION to " + StringParsing::Convert(majorVersion) + ". SDL_Error() returned: \"" + GetSDLError() + "\"");
			return;
		}

		if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minorVersion) < 0)
		{
			Debug::Logger::LogFatal("Blaze Engine Graphics", "Failed to set OpenGL context attribute SDL_GL_CONTEXT_MINOR_VERSION to " + StringParsing::Convert(minorVersion) + ". SDL_Error() returned: \"" + GetSDLError() + "\"");
			return;
		}

		if (SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depthBufferBitCount) < 0)
		{
			Debug::Logger::LogFatal("Blaze Engine Graphics", "Failed to set OpenGL context attribute SDL_GL_DEPTH_SIZE to " + StringParsing::Convert(depthBufferBitCount) + ". SDL_Error() returned: \"" + GetSDLError() + "\"");
			return;
		}
		if (SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, stencilBufferBitCount) < 0)
		{
			Debug::Logger::LogFatal("Blaze Engine Graphics", "Failed to set OpenGL context attribute SDL_GL_STENCIL_SIZE to " + StringParsing::Convert(stencilBufferBitCount) + ". SDL_Error() returned: \"" + GetSDLError() + "\"");
			return;
		}
		
		WindowSDLCreateOptions windowCreateOptions{
			.graphicsAPI = WindowSDLGraphicsAPI::OpenGL
		};
		WindowSDL initWindowSDL_{ windowCreateOptions };				

		SDLOpenGLContext_OpenGL SDLOpenGLContext_{
			SDL_GL_CreateContext((SDL_Window*)initWindowSDL_.GetHandle())
		};		

		if (SDLOpenGLContext_.handle == nullptr)
		{			
			Debug::Logger::LogFatal("Blaze Engine Graphics", "Failed to create OpenGL context. SDL_Error() returned: \"" + GetSDLError() + "\"");
			return;
		}

		if (SDL_GL_MakeCurrent((SDL_Window*)initWindowSDL_.GetHandle(), SDLOpenGLContext_.handle) != 0)
		{
			Debug::Logger::LogFatal("Blaze Engine Graphics", "Failed to make the OpenGL context current. SDL_Error() returned: \"" + GetSDLError() + "\"");
			return;
		}

		//Set the handles after creation was successfull
		initWindowSDL = std::move(initWindowSDL_);
		SDLOpenGLContext = SDLOpenGLContext_;

		if (!glewInitialized)
		{
			glewExperimental = true;
			GLenum err = glewInit();

			if (err != GLEW_OK)
			{
				Debug::Logger::LogFatal("Blaze Engine Graphics", "Failed to initialize GLEW. glewGetErrorString() returned: \"" + GetGlewError(err) + "\"");
				return;
			}
			else
			{
				glewInitialized = true;
				Debug::Logger::LogInfo("Blaze Engine Graphics", "Successfully initialized glew " + GetGlewString(GLEW_VERSION));
			}
		}

		int finalContextFlags;
		glGetIntegerv(GL_CONTEXT_FLAGS, &finalContextFlags);
		if (bool(_contextFlags & SDL_GL_CONTEXT_DEBUG_FLAG) && !bool(finalContextFlags & SDL_GL_CONTEXT_DEBUG_FLAG))
			Debug::Logger::LogWarning("Blaze Engine Graphics", "Asked for OpenGL debug context, a non debug one was created");
		if (!bool(_contextFlags & SDL_GL_CONTEXT_DEBUG_FLAG) && bool(finalContextFlags & SDL_GL_CONTEXT_DEBUG_FLAG))
			Debug::Logger::LogWarning("Blaze Engine Graphics", "Asked for OpenGL non debug context, a debug one was created");
		if (bool(_contextFlags & SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG) && !bool(finalContextFlags & SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG))
			Debug::Logger::LogWarning("Blaze Engine Graphics", "Asked for OpenGL forward compatible context, a non forward compatible was created");
		if (!bool(_contextFlags & SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG) && bool(finalContextFlags & SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG))
			Debug::Logger::LogWarning("Blaze Engine Graphics", "Asked for OpenGL non forward compatible context, a forward compatible was created");
		if (bool(_contextFlags & SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG) && !bool(finalContextFlags & SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG))
			Debug::Logger::LogWarning("Blaze Engine Graphics", "Asked for OpenGL robust access context, a non robust access was created");
		if (!bool(_contextFlags & SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG) && bool(finalContextFlags & SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG))
			Debug::Logger::LogWarning("Blaze Engine Graphics", "Asked for OpenGL non robust access context, a robust access was created");
		if (bool(_contextFlags & SDL_GL_CONTEXT_RESET_ISOLATION_FLAG) && !bool(finalContextFlags & SDL_GL_CONTEXT_RESET_ISOLATION_FLAG))
			Debug::Logger::LogWarning("Blaze Engine Graphics", "Asked for OpenGL reset isolation context, a non reset isolation was created");
		if (!bool(_contextFlags & SDL_GL_CONTEXT_RESET_ISOLATION_FLAG) && bool(finalContextFlags & SDL_GL_CONTEXT_RESET_ISOLATION_FLAG))
			Debug::Logger::LogWarning("Blaze Engine Graphics", "Asked for OpenGL non reset isolation context, a reset isolation was created");
		contextFlags = GetContextFlags(finalContextFlags);

		int finalProfileType;
		glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &finalProfileType);
		String profileName = GetProfileTypeName(finalProfileType);
		if (finalProfileType != _profileType)
		{
			StringView name1 = GetProfileTypeName(_profileType);
			StringView name2 = profileName;

			if (!name1.Empty())
				if (name2.Empty())
					Debug::Logger::LogWarning("Blaze Engine Graphics", "Asked for OpenGL " + name1 + " profile, got an invalid value");
				else
					Debug::Logger::LogWarning("Blaze Engine Graphics", "Asked for OpenGL " + name1 + " profile, got " + name2);
		}
		profileType = GetProfileType(finalProfileType);

		int finalMajorVersion;
		glGetIntegerv(GL_MAJOR_VERSION, &finalMajorVersion);
		if (finalMajorVersion != majorVersion)
			Debug::Logger::LogWarning("Blaze Engine Graphics", "Asked for OpenGL Major Version " + StringParsing::Convert(majorVersion) + " got " + StringParsing::Convert(finalMajorVersion));
		majorVersion = finalMajorVersion;

		int finalMinorVersion;
		glGetIntegerv(GL_MINOR_VERSION, &finalMinorVersion);
		if (finalMinorVersion != minorVersion)
			Debug::Logger::LogWarning("Blaze Engine Graphics", "Asked for OpenGL Minor Version " + StringParsing::Convert(minorVersion) + " got " + StringParsing::Convert(finalMinorVersion));
		minorVersion = finalMinorVersion;

		int finalDepthSize;
		glGetIntegerv(GL_DEPTH_BITS, &finalDepthSize);
		if (finalDepthSize != depthBufferBitCount)
			if (finalDepthSize == 0)
				Debug::Logger::LogWarning("Blaze Engine Graphics", "Asked for " + StringParsing::Convert(depthBufferBitCount) + "-bit depth buffer, got no depth buffer");
			else if (depthBufferBitCount == 0)
				Debug::Logger::LogWarning("Blaze Engine Graphics", "Asked for no depth buffer, got " + StringParsing::Convert(finalDepthSize) + "-bit depth buffer");
			else
				Debug::Logger::LogWarning("Blaze Engine Graphics", "Asked for " + StringParsing::Convert(depthBufferBitCount) + "-bit depth buffer, got " + StringParsing::Convert(finalDepthSize) + "-bit");
		depthBufferBitCount = finalDepthSize;

		int finalStencilSize;
		glGetIntegerv(GL_STENCIL_BITS, &finalStencilSize);
		if (finalStencilSize != stencilBufferBitCount)
			if (finalStencilSize == 0)
				Debug::Logger::LogWarning("Blaze Engine Graphics", "Asked for " + StringParsing::Convert(stencilBufferBitCount) + "-bit stencil buffer, got no stencil buffer");
			else if (stencilBufferBitCount == 0)
				Debug::Logger::LogWarning("Blaze Engine Graphics", "Asked for no stencil buffer, got " + StringParsing::Convert(finalStencilSize) + "-bit stencil buffer");
			else
				Debug::Logger::LogWarning("Blaze Engine Graphics", "Asked for " + StringParsing::Convert(stencilBufferBitCount) + "-bit stencil buffer, got " + StringParsing::Convert(finalStencilSize) + "-bit");
		stencilBufferBitCount = finalStencilSize;

		String contextFlagsText = GetContextFlagsText(finalContextFlags);
		if (!contextFlagsText.Empty()) contextFlagsText = " (" + contextFlagsText + ")";
		Debug::Logger::LogInfo("Blaze Engine Graphics", "Created OpenGL context " + StringParsing::Convert(majorVersion) + "." + StringParsing::Convert(minorVersion) + " " + profileName + " profile" + contextFlagsText);	

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
		UnsetActiveOpenGLGraphicsContext(*this);

		SDL_GL_DeleteContext(SDLOpenGLContext.handle);

		initWindowSDL.Destroy();
		SDLOpenGLContext.handle = nullptr;
	}
	void GraphicsContext_OpenGL::SetActiveRenderWindow(RenderWindow_OpenGL& renderWindow)
	{
		activeWindowSDLHandle = renderWindow.GetWindowSDL().GetHandle();

		//If the context is already active, make it active again so the new window will be also active. If not, do nothing
		if (GetActiveOpenGLGraphicsContext() == this)
			SetActiveOpenGLGraphicsContextForced(*this);
	}
	WindowSDL GraphicsContext_OpenGL::CreateWindowSDL(WindowSDLCreateOptions_OpenGL createOptions)
	{
		if (initWindowSDL.IsNullWindow())
		{
			WindowSDLCreateOptions _createOptions{
				.graphicsAPI = WindowSDLGraphicsAPI::OpenGL,
				.title = std::move(createOptions.title),
				.pos = createOptions.pos,
				.size = createOptions.size,
				.openMode = createOptions.openMode,
				.styleFlags = createOptions.styleFlags,
			};

			return WindowSDL(_createOptions);
		}
		else
		{
			initWindowSDL.SetTitle(createOptions.title);
	
			if (bool(createOptions.styleFlags & WindowSDLStyleFlags::Borderless))
				initWindowSDL.SetWindowBorderFlag(true);
			if (bool(createOptions.styleFlags & WindowSDLStyleFlags::LockMouse))
				initWindowSDL.SetWindowLockMouseFlag(true);
			if (bool(createOptions.styleFlags & WindowSDLStyleFlags::Resizable))
				initWindowSDL.SetWindowResizableFlag(true);

			switch (createOptions.openMode)
			{
			case WindowSDLOpenMode::Normal:
				initWindowSDL.SetSize(createOptions.size);
				break;
			case WindowSDLOpenMode::Fullscreen:
				initWindowSDL.SetWindowFullscreenMode(true);
				break;
			case WindowSDLOpenMode::Maximized:
				initWindowSDL.Maximize();
				break;
			case WindowSDLOpenMode::Minimized:
				initWindowSDL.Minimize();
				break;
			default:
				break;
			}		

			initWindowSDL.SetPos(createOptions.pos);

			return std::move(initWindowSDL);
		}
	}
	void GraphicsContext_OpenGL::DestroyWindowSDL(WindowSDL& window)
	{
		WindowSDL::WindowSDLHandle currentWindowHandle = SDL_GL_GetCurrentWindow();

		if (currentWindowHandle == window.GetHandle() && initWindowSDL.IsNullWindow())
		{
			initWindowSDL = std::move(window);
			initWindowSDL.ShowWindow(false);
		}
		else
			window.Destroy();
	}
}