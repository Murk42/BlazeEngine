#include "pch.h"
#include "BlazeEngineGraphics/Core/OpenGL/GraphicsContext_OpenGL.h"
#include "BlazeEngineCore/Debug/Logger.h"
#include "BlazeEngine/ExecuteOnMainThread.h"
#include "BlazeEngineGraphics/Core/OpenGL/RenderWindow_OpenGL.h"

#include "BlazeEngine/Console/Console.h"
#include <GL/glew.h>
#include <SDL3/SDL_video.h>

#include <Windows.h>
#undef max
#undef CreateWindow

namespace Blaze::Windows
{
	String GetErrorString(DWORD error, uint recursionIndex = 0);
}

namespace Blaze::Graphics::OpenGL
{
	bool glewInitialized = false;	

	static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) 
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
		
		decltype(&Debug::Logger::LogError<>) logFunc;
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:               
		default:
			logFunc = Debug::Logger::LogError<>; 
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: 
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		case GL_DEBUG_TYPE_PORTABILITY:       
		case GL_DEBUG_TYPE_PERFORMANCE:       
		case GL_DEBUG_TYPE_OTHER:             
		case GL_DEBUG_TYPE_PUSH_GROUP:        
		case GL_DEBUG_TYPE_POP_GROUP:         
			return;
			logFunc = Debug::Logger::LogWarning<>; 
			break;
		case GL_DEBUG_TYPE_MARKER:            
			logFunc = Debug::Logger::LogDebug<>; 
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

	static bool SetOpenGLContextAttributes(const GraphicsContextProperties_OpenGL& properties)
	{
		GLenum _contextFlags = GetOpenGLContextFlags(properties.contextFlags);
		GLenum _profileType = GetOpenGLProfileType(properties.profileType);
		GLenum _releaseBehaviour = GetOpenGLReleaseBehaviour(properties.releaseBehaviour);

		SDL_GL_ResetAttributes();

		if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, properties.majorVersion))
		{
			BLAZE_ENGINE_GRAPHICS_FATAL("Failed to set OpenGL context attribute SDL_GL_CONTEXT_MAJOR_VERSION to {}. SDL_Error() returnd: \"{}\"", properties.majorVersion, GetSDLError());
			return false;
		}

		if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, properties.minorVersion))
		{
			BLAZE_ENGINE_GRAPHICS_FATAL("Failed to set OpenGL context attribute SDL_GL_CONTEXT_MINOR_VERSION to {}. SDL_Error() returnd: \"{}\"", properties.minorVersion, GetSDLError());
			return false;
		}

		if (!SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1))
		{
			BLAZE_ENGINE_GRAPHICS_FATAL("Failed to set OpenGL context attribute SDL_GL_ACCELERTED_VISUAL to 1. SDL_Error() returnd: \"{}\"", GetSDLError());
			return false;
		}

		if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, _contextFlags))
		{
			BLAZE_ENGINE_GRAPHICS_FATAL("Failed to set OpenGL context attribute SDL_GL_CONTEXT_FLAGS to {}.SDL_Error() returnd: \"{}\"", GetOpenGLContextFlagsString(_contextFlags), GetSDLError());
			return false;
		}

		if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, _profileType))
		{
			BLAZE_ENGINE_GRAPHICS_FATAL("Failed to set OpenGL context attribute SDL_GL_CONTEXT_PROFILE_MASK to {}.SDL_Error() returnd: \"{}\"", GetOpenGLProfileTypeString(_profileType), GetSDLError());
			return false;
		}

		if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_RELEASE_BEHAVIOR, _releaseBehaviour))
		{
			BLAZE_ENGINE_GRAPHICS_FATAL("Failed to set OpenGL context attribute SDL_GL_CONTEXT_RELEASE_BEHAVIOR to {}.SDL_Error() returnd: \"{}\"", GetOpenGLReleaseBehaviourString(_releaseBehaviour), GetSDLError());
			return false;
		}

		if (!SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, properties.depthBufferBitCount))
		{
			BLAZE_ENGINE_GRAPHICS_FATAL("Failed to set OpenGL context attribute SDL_GL_DEPTH_SIZE to {}. SDL_Error() returnd: \"{}\"", properties.depthBufferBitCount, GetSDLError());
			return false;
		}
		if (!SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, properties.stencilBufferBitCount))
		{
			BLAZE_ENGINE_GRAPHICS_FATAL("Failed to set OpenGL context attribute SDL_GL_STENCIL_SIZE to . SDL_Error() returnd: \"{}\"", properties.stencilBufferBitCount, GetSDLError());
			return false;
		}

		return true;
	}
	static bool CreateInitWindow(void*& windowHandle)
	{
		struct Parameters {
			void*& windowHandle;
		} parameters{ windowHandle };
		ExecuteOnMainThread([](void* userData) {
			Parameters* params = (Parameters*)userData;									

			SDL_PropertiesID props = SDL_CreateProperties();

			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, false) == false)
				BLAZE_ENGINE_GRAPHICS_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_BORDERLESS_BOOLEAN, true) == false)
				BLAZE_ENGINE_GRAPHICS_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_BORDERLESS_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_MOUSE_GRABBED_BOOLEAN, false) == false)
				BLAZE_ENGINE_GRAPHICS_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_MOUSE_GRABBED_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_FOCUSABLE_BOOLEAN, false) == false)
				BLAZE_ENGINE_GRAPHICS_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_FOCUSABLE_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_HIDDEN_BOOLEAN, true) == false)
				BLAZE_ENGINE_GRAPHICS_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_HIDDEN_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_TRANSPARENT_BOOLEAN, false) == false)
				BLAZE_ENGINE_GRAPHICS_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_TRANSPARENT_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN, false) == false)
				BLAZE_ENGINE_GRAPHICS_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_ALWAYS_ON_TOP_BOOLEAN, false) == false)
				BLAZE_ENGINE_GRAPHICS_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_ALWAYS_ON_TOP_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_UTILITY_BOOLEAN, true) == false)
				BLAZE_ENGINE_GRAPHICS_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_UTILITY_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");

			if (SDL_SetPointerProperty(props, SDL_PROP_WINDOW_CREATE_PARENT_POINTER, nullptr) == false)
				BLAZE_ENGINE_GRAPHICS_ERROR("Calling SDL_SetPointerProperty with SDL_PROP_WINDOW_CREATE_PARENT_POINTER failed. SDL returned error: \"" + GetSDLError() + "\"");

			if (SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, nullptr) == false)
				BLAZE_ENGINE_GRAPHICS_ERROR("Calling SDL_SetStringProperty with SDL_PROP_WINDOW_CREATE_TITLE_STRING failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_CENTERED) == false)
				BLAZE_ENGINE_GRAPHICS_ERROR("Calling SDL_SetNumberProperty with SDL_PROP_WINDOW_CREATE_X_NUMBER failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_CENTERED) == false)
				BLAZE_ENGINE_GRAPHICS_ERROR("Calling SDL_SetNumberProperty with SDL_PROP_WINDOW_CREATE_Y_NUMBER failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, 0) == false)
				BLAZE_ENGINE_GRAPHICS_ERROR("Calling SDL_SetNumberProperty with SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, 0) == false)
				BLAZE_ENGINE_GRAPHICS_ERROR("Calling SDL_SetNumberProperty with SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER failed. SDL returned error: \"" + GetSDLError() + "\"");

			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN, false) == false)
				BLAZE_ENGINE_GRAPHICS_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_MAXIMIZED_BOOLEAN, false) == false)
				BLAZE_ENGINE_GRAPHICS_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_MAXIMIZED_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_MINIMIZED_BOOLEAN, true) == false)
				BLAZE_ENGINE_GRAPHICS_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_MINIMIZED_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");

			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_METAL_BOOLEAN, false) == false)
				BLAZE_ENGINE_GRAPHICS_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_METAL_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, true) == false)
				BLAZE_ENGINE_GRAPHICS_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_VULKAN_BOOLEAN, false) == false)
				BLAZE_ENGINE_GRAPHICS_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_VULKAN_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");			
			
			params->windowHandle = SDL_CreateWindowWithProperties(props);

			if (params->windowHandle == nullptr)
			{
				BLAZE_ENGINE_CORE_FATAL("Failed to create a SDL window. SDL returned error: \"" + GetSDLError() + "\"");
				return;
			}

			SDL_DestroyProperties(props);

			}, &parameters);

		if (windowHandle == nullptr)
			return false;

		return true;
	}
	static void DestroyInitWindow(void* windowHandle)
	{
		ExecuteOnMainThread([](void* userData) { SDL_DestroyWindow((SDL_Window*)userData); }, windowHandle);
	}
	//This funciton should be run on the main thread, but if it is run on the main thread all OpenGL functions should do the same, which is a problem.
	//There seems not to be a problem running this on a non-main thread, so this should be done.
	static bool CreateOpenGLContext(void* windowHandle, void*& context)
	{
		context = SDL_GL_CreateContext((SDL_Window*)windowHandle);

		if (context == nullptr)
		{
			BLAZE_ENGINE_GRAPHICS_FATAL("Failed to create OpenGL context. SDL_Error() returnd: \"{}\"", GetSDLError());
			return false;

		}

		{
			StringView error = GetSDLError();

			if (!error.Empty())
			{
				if (context != nullptr)
					BLAZE_ENGINE_GRAPHICS_WARNING("SDL reported an error while creating a OpenGL context, the returned context wasn't nullptr, trying to continue. SDL_Error() returned: \"" + error + "\"");
			}
		}

		return true;
	}
	static bool InitializeGLEW()
	{
		if (!glewInitialized)
		{
			glewExperimental = true;
			GLenum err = glewInit();

			if (err != GLEW_OK)
			{
				BLAZE_ENGINE_GRAPHICS_FATAL("Failed to initialize GLEW. glewGetErrorString() returned: \"" + GetGlewError(err) + "\"");
				return false;
			}
			else
			{
				glewInitialized = true;
				BLAZE_ENGINE_GRAPHICS_INFO("Successfully initialized glew " + GetGlewString(GLEW_VERSION));
			}
		}

		return true;
	}
	static bool ValidateContextAttributes(GraphicsContextProperties_OpenGL& properties)
	{
		GLenum _contextFlags = GetOpenGLContextFlags(properties.contextFlags);
		GLenum _profileType = GetOpenGLProfileType(properties.profileType);
		GLenum _releaseBehaviour = GetOpenGLReleaseBehaviour(properties.releaseBehaviour);

		int finalContextFlags = 0;
		if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &finalContextFlags))		
		{
			if (bool(_contextFlags & SDL_GL_CONTEXT_DEBUG_FLAG) && !bool(finalContextFlags & SDL_GL_CONTEXT_DEBUG_FLAG))
				BLAZE_ENGINE_GRAPHICS_WARNING("Asked for OpenGL debug context, a non debug one was created");
			if (!bool(_contextFlags & SDL_GL_CONTEXT_DEBUG_FLAG) && bool(finalContextFlags & SDL_GL_CONTEXT_DEBUG_FLAG))
				BLAZE_ENGINE_GRAPHICS_WARNING("Asked for OpenGL non debug context, a debug one was created");
			if (bool(_contextFlags & SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG) && !bool(finalContextFlags & SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG))
				BLAZE_ENGINE_GRAPHICS_WARNING("Asked for OpenGL forward compatible context, a non forward compatible was created");
			if (!bool(_contextFlags & SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG) && bool(finalContextFlags & SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG))
				BLAZE_ENGINE_GRAPHICS_WARNING("Asked for OpenGL non forward compatible context, a forward compatible was created");
			if (bool(_contextFlags & SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG) && !bool(finalContextFlags & SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG))
				BLAZE_ENGINE_GRAPHICS_WARNING("Asked for OpenGL robust access context, a non robust access was created");
			if (!bool(_contextFlags & SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG) && bool(finalContextFlags & SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG))
				BLAZE_ENGINE_GRAPHICS_WARNING("Asked for OpenGL non robust access context, a robust access was created");
			if (bool(_contextFlags & SDL_GL_CONTEXT_RESET_ISOLATION_FLAG) && !bool(finalContextFlags & SDL_GL_CONTEXT_RESET_ISOLATION_FLAG))
				BLAZE_ENGINE_GRAPHICS_WARNING("Asked for OpenGL reset isolation context, a non reset isolation was created");
			if (!bool(_contextFlags & SDL_GL_CONTEXT_RESET_ISOLATION_FLAG) && bool(finalContextFlags & SDL_GL_CONTEXT_RESET_ISOLATION_FLAG))
				BLAZE_ENGINE_GRAPHICS_WARNING("Asked for OpenGL non reset isolation context, a reset isolation was created");
			properties.contextFlags = GetContextFlags(finalContextFlags);
		}
		else
			BLAZE_ENGINE_GRAPHICS_ERROR("SDL_GL_GetAttribute failed when called with SDL_GL_CONTEXT_FLAGS. SDL returned error: \"" + GetSDLError() + "\"");

		String profileName = "invalid";
		int finalProfileType = 0;
		if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &finalProfileType))
		{
			profileName = GetProfileTypeName(finalProfileType);
			if (finalProfileType != _profileType)
			{
				StringView name1 = GetProfileTypeName(_profileType);
				StringView name2 = profileName;

				if (!name1.Empty())
					if (name2.Empty())
						BLAZE_ENGINE_GRAPHICS_WARNING("Asked for OpenGL " + name1 + " profile, got an invalid value");
					else
						BLAZE_ENGINE_GRAPHICS_WARNING("Asked for OpenGL " + name1 + " profile, got " + name2);
			}
			properties.profileType = GetProfileType(finalProfileType);
		}
		else
			BLAZE_ENGINE_GRAPHICS_ERROR("SDL_GL_GetAttribute failed when called with SDL_GL_CONTEXT_PROFILE_MASK. SDL returned error: \"" + GetSDLError() + "\"");

		int finalMajorVersion = 0;
		if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &finalMajorVersion))
		{
			if (finalMajorVersion != properties.majorVersion)
				BLAZE_ENGINE_GRAPHICS_WARNING("Asked for OpenGL Major Version {} got {}", properties.majorVersion, finalMajorVersion);
			properties.majorVersion = finalMajorVersion;
		}
		else
			BLAZE_ENGINE_GRAPHICS_ERROR("SDL_GL_GetAttribute failed when called with SDL_GL_CONTEXT_MAJOR_VERSION. SDL returned error: \"" + GetSDLError() + "\"");

		int finalMinorVersion = 0;
		if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &finalMinorVersion))
		{
			if (finalMinorVersion != properties.minorVersion)
				BLAZE_ENGINE_GRAPHICS_WARNING("Asked for OpenGL Minor Version {} got {}", properties.minorVersion, finalMinorVersion);
			properties.minorVersion = finalMinorVersion;
		}
		else
			BLAZE_ENGINE_GRAPHICS_ERROR("SDL_GL_GetAttribute failed when called with SDL_GL_CONTEXT_MINOR_VERSION. SDL returned error: \"" + GetSDLError() + "\"");

		int finalDepthSize = 0;
		if (SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &finalDepthSize))
		{
			if (finalDepthSize != properties.depthBufferBitCount)
				if (finalDepthSize == 0)
					BLAZE_ENGINE_GRAPHICS_WARNING("Asked for {}-bit depth buffer, got no depth buffer", properties.depthBufferBitCount);
				else if (properties.depthBufferBitCount == 0)
					BLAZE_ENGINE_GRAPHICS_WARNING("Asked for no depth buffer, got {}-bit depth buffer", finalDepthSize);
				else
					BLAZE_ENGINE_GRAPHICS_WARNING("Asked for {}-bit depth buffer, got {}-bit", properties.depthBufferBitCount, finalDepthSize);
			properties.depthBufferBitCount = finalDepthSize;
		}
		else
			BLAZE_ENGINE_GRAPHICS_ERROR("SDL_GL_GetAttribute failed when called with SDL_GL_DEPTH_SIZE. SDL returned error: \"" + GetSDLError() + "\"");

		int finalStencilSize = 0;
		if (SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &finalStencilSize))
		{
			if (finalStencilSize != properties.stencilBufferBitCount)
				if (finalStencilSize == 0)
					BLAZE_ENGINE_GRAPHICS_WARNING("Asked for {}-bit stencil buffer, got no stencil buffer", properties.stencilBufferBitCount);
				else if (properties.stencilBufferBitCount == 0)
					BLAZE_ENGINE_GRAPHICS_WARNING("Asked for no stencil buffer, got {}-bit stencil buffer", finalStencilSize);
				else
					BLAZE_ENGINE_GRAPHICS_WARNING("Asked for {}-bit stencil buffer, got {}-bit", properties.stencilBufferBitCount, finalStencilSize);
			properties.stencilBufferBitCount = finalStencilSize;
		}
		else
			BLAZE_ENGINE_GRAPHICS_ERROR("SDL_GL_GetAttribute failed when called with SDL_GL_STENCIL_SIZE. SDL returned error: \"{}\"", GetSDLError());

		String contextFlagsText = GetContextFlagsText(finalContextFlags);
		if (!contextFlagsText.Empty()) contextFlagsText = " (" + contextFlagsText + ")";
		BLAZE_ENGINE_GRAPHICS_INFO("Created OpenGL context {}.{} {} profile ", properties.majorVersion, properties.minorVersion, profileName, contextFlagsText);

		return true;
	}

	GraphicsContext_OpenGL::GraphicsContext_OpenGL()
		: GraphicsContext_OpenGL(GraphicsContextProperties_OpenGL())
	{
	}
	GraphicsContext_OpenGL::GraphicsContext_OpenGL(const GraphicsContextProperties_OpenGL& _properties) :
		activeWindowHandle(nullptr), properties(_properties), SDLOpenGLContext(nullptr), initWindow(nullptr)
	{
		if (!SetOpenGLContextAttributes(_properties))
			return;
		
		void* initWindow_ = nullptr;

		if (!CreateInitWindow(initWindow_))
			return;
		
		if (!CreateOpenGLContext(initWindow_, SDLOpenGLContext))					
			return;		
		
		initWindow = std::move(initWindow_);
		activeWindowHandle = initWindow;

		if (!InitializeGLEW())
			return;

		ValidateContextAttributes(properties);

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
		SDL_GL_DestroyContext((SDL_GLContext)SDLOpenGLContext);
		DestroyInitWindow(initWindow);
		SDLOpenGLContext = nullptr;
	}
	void GraphicsContext_OpenGL::SetActiveRenderWindow(RenderWindow_OpenGL& renderWindow)
	{
		if (activeWindowHandle != nullptr)
		{
			Window* activeWindow = Window::GetWindowFromHandle(activeWindowHandle);

			if (activeWindow != nullptr)			
				activeWindow->windowDestroyedEventDispatcher.RemoveHandler<&GraphicsContext_OpenGL::ActiveWindowDestroyed>(*this);						
		}

		activeWindowHandle = renderWindow.GetWindow().GetHandle();

		renderWindow.GetWindow().windowDestroyedEventDispatcher.AddHandler<&GraphicsContext_OpenGL::ActiveWindowDestroyed>(*this);

		if (SDL_GL_MakeCurrent((SDL_Window*)activeWindowHandle, (SDL_GLContext)SDLOpenGLContext) == false)
			BLAZE_ENGINE_GRAPHICS_FATAL("SDL_GL_MakeCurrent() failed. SDL_Error() returnd: \"{}\"", GetSDLError());
	}
	void GraphicsContext_OpenGL::ActiveWindowDestroyed(const Window::WindowDestroyedEvent& event)
	{
		activeWindowHandle = initWindow;
		if (SDL_GL_MakeCurrent((SDL_Window*)activeWindowHandle, (SDL_GLContext)SDLOpenGLContext) == false)
			BLAZE_ENGINE_GRAPHICS_FATAL("SDL_GL_MakeCurrent() failed. SDL_Error() returnd: \"{}\"", GetSDLError());
	}
}