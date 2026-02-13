#include "pch.h"
#include "BlazeEngine/Runtime/BlazeEngineContext.h"
#include "BlazeEngine/Core/Debug/Logger.h"
#include "BlazeEngine/Graphics/Core/OpenGL/GraphicsContext_OpenGL.h"
#include "BlazeEngine/Graphics/Core/OpenGL/RenderWindow_OpenGL.h"
#include "BlazeEngine/External/SDL/SDL.h"

#include "BlazeEngine/Core/Console/Console.h"
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
	std::mutex glewMutex;
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
		case GL_DEBUG_SOURCE_APPLICATION:     _source = "App"; break;
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

		logFunc("OpenGL (" + _source + ")", "Type: " + _type + " | Severity: " + _severity + " | " + _message, false);
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
			out += "robust access";
		}
		if (contextFlags & SDL_GL_CONTEXT_RESET_ISOLATION_FLAG)
		{
			if (!out.Empty()) out += ", ";
			out += "reset isolation";
		}

		if (out.Empty())
			out = "no flags";

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

#define TrySetGLAttributeWithString(ENUM_NAME, VALUE, STRING_VALUE)																																\
		if (!SDL_GL_SetAttribute(ENUM_NAME, VALUE))																											\
		{																																										\
			BLAZE_LOG_ERROR("Failed to set OpenGL context attribute " #ENUM_NAME " to {}. SDL_Error() returnd: \"{}\"", VALUE, SDL_GetError());				\
			return false;																																						\
		}
#define TrySetGLAttribute(ENUM_NAME, VALUE) TrySetGLAttributeWithString(ENUM_NAME, VALUE, #VALUE)
		
		TrySetGLAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, properties.majorVersion);
		TrySetGLAttribute(SDL_GL_CONTEXT_MINOR_VERSION, properties.minorVersion);
		TrySetGLAttribute(SDL_GL_ACCELERATED_VISUAL, properties.acceleratedVisual);
		TrySetGLAttributeWithString(SDL_GL_CONTEXT_FLAGS, _contextFlags, GetOpenGLContextFlagsString(_contextFlags));
		TrySetGLAttributeWithString(SDL_GL_CONTEXT_PROFILE_MASK, _contextFlags, GetOpenGLProfileTypeString(_profileType));
		TrySetGLAttributeWithString(SDL_GL_CONTEXT_RELEASE_BEHAVIOR, _contextFlags, GetOpenGLReleaseBehaviourString(_releaseBehaviour));
		TrySetGLAttribute(SDL_GL_DEPTH_SIZE, properties.minDepthBufferBitCount);
		TrySetGLAttribute(SDL_GL_STENCIL_SIZE, properties.minStencilBufferBitCount);
		TrySetGLAttribute(SDL_GL_RED_SIZE, properties.minRedBitCount);
		TrySetGLAttribute(SDL_GL_GREEN_SIZE, properties.minGreenBitCount);
		TrySetGLAttribute(SDL_GL_BLUE_SIZE, properties.minBlueBitCount);
		TrySetGLAttribute(SDL_GL_ALPHA_SIZE, properties.minAlphaBitCount);
		TrySetGLAttribute(SDL_GL_DOUBLEBUFFER, properties.doubleBuffer);
		TrySetGLAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, properties.framebufferSRGBCapable);

		return true;
	}
	static bool CreateInitWindow(void*& windowHandle)
	{
		SDL_PropertiesID props = SDL_CreateProperties();

		if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, false) == false)
			BLAZE_LOG_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_BORDERLESS_BOOLEAN, true) == false)
			BLAZE_LOG_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_BORDERLESS_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_MOUSE_GRABBED_BOOLEAN, false) == false)
			BLAZE_LOG_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_MOUSE_GRABBED_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_FOCUSABLE_BOOLEAN, false) == false)
			BLAZE_LOG_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_FOCUSABLE_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_HIDDEN_BOOLEAN, true) == false)
			BLAZE_LOG_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_HIDDEN_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_TRANSPARENT_BOOLEAN, false) == false)
			BLAZE_LOG_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_TRANSPARENT_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN, false) == false)
			BLAZE_LOG_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_ALWAYS_ON_TOP_BOOLEAN, false) == false)
			BLAZE_LOG_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_ALWAYS_ON_TOP_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_UTILITY_BOOLEAN, true) == false)
			BLAZE_LOG_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_UTILITY_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");

		if (SDL_SetPointerProperty(props, SDL_PROP_WINDOW_CREATE_PARENT_POINTER, nullptr) == false)
			BLAZE_LOG_ERROR("Calling SDL_SetPointerProperty with SDL_PROP_WINDOW_CREATE_PARENT_POINTER failed. SDL returned error: \"" + SDL_GetError() + "\"");

		if (SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, nullptr) == false)
			BLAZE_LOG_ERROR("Calling SDL_SetStringProperty with SDL_PROP_WINDOW_CREATE_TITLE_STRING failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_CENTERED) == false)
			BLAZE_LOG_ERROR("Calling SDL_SetNumberProperty with SDL_PROP_WINDOW_CREATE_X_NUMBER failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_CENTERED) == false)
			BLAZE_LOG_ERROR("Calling SDL_SetNumberProperty with SDL_PROP_WINDOW_CREATE_Y_NUMBER failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, 0) == false)
			BLAZE_LOG_ERROR("Calling SDL_SetNumberProperty with SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, 0) == false)
			BLAZE_LOG_ERROR("Calling SDL_SetNumberProperty with SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER failed. SDL returned error: \"" + SDL_GetError() + "\"");

		if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN, false) == false)
			BLAZE_LOG_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_MAXIMIZED_BOOLEAN, false) == false)
			BLAZE_LOG_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_MAXIMIZED_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_MINIMIZED_BOOLEAN, true) == false)
			BLAZE_LOG_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_MINIMIZED_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");

		if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_METAL_BOOLEAN, false) == false)
			BLAZE_LOG_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_METAL_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, true) == false)
			BLAZE_LOG_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_VULKAN_BOOLEAN, false) == false)
			BLAZE_LOG_ERROR("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_VULKAN_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");

		struct Parameters {
			SDL_PropertiesID properties;
			void*& windowHandle;
		} parameters{ props, windowHandle };

		if (!SDL_RunOnMainThread([](void* userData) { ((Parameters*)userData)->windowHandle = SDL_CreateWindowWithProperties(((Parameters*)userData)->properties); }, &parameters, true))
			BLAZE_LOG_ERROR("SDL_RunOnMainThread failed. SDL returned error: \"" + SDL_GetError() + "\"");

		SDL_DestroyProperties(props);

		if (windowHandle == nullptr)
		{
			BLAZE_LOG_ERROR("Failed to create a SDL window. SDL returned error: \"" + SDL_GetError() + "\"");
			return false;
		}

		return true;
	}
	static void DestroyInitWindow(void* windowHandle)
	{
		if (!SDL_RunOnMainThread([](void* userData) { SDL_DestroyWindow((SDL_Window*)userData); }, windowHandle, true))
			BLAZE_LOG_ERROR("SDL_RunOnMainThread failed. SDL returned error: \"" + SDL_GetError() + "\"");
	}
	//This funciton should be run on the main thread, but if it is run on the main thread all OpenGL functions should do the same, which is a problem.
	//There seems not to be a problem running this on a non-main thread, so this should be done.
	static bool CreateOpenGLContext(void* windowHandle, void*& context)
	{
		context = SDL_GL_CreateContext((SDL_Window*)windowHandle);

		if (context == nullptr)
		{
			BLAZE_LOG_ERROR("Failed to create OpenGL context. SDL_Error() returnd: \"{}\"", SDL_GetError());
			return false;

		}

		{
			StringView error = SDL_GetError();

			if (!error.Empty())
			{
				if (context != nullptr)
					BLAZE_LOG_WARNING("SDL reported an error while creating a OpenGL context, the returned context wasn't nullptr, trying to continue. SDL_Error() returned: \"" + error + "\"");
			}
		}

		return true;
	}
	static bool InitializeGLEW()
	{
		std::lock_guard lg{ glewMutex };

		if (!glewInitialized)
		{
			glewExperimental = true;
			GLenum err = glewInit();

			if (err != GLEW_OK)
			{
				BLAZE_LOG_ERROR("Failed to initialize GLEW. glewGetErrorString() returned: \"" + GetGlewError(err) + "\"");
				return false;
			}
			else
			{
				glewInitialized = true;
				BLAZE_LOG_INFO("<color=green><color=green>Successfully<color/><color/> initialized glew " + GetGlewString(GLEW_VERSION));
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
				BLAZE_LOG_WARNING("Asked for OpenGL debug context, a non debug one was created");
			if (!bool(_contextFlags & SDL_GL_CONTEXT_DEBUG_FLAG) && bool(finalContextFlags & SDL_GL_CONTEXT_DEBUG_FLAG))
				BLAZE_LOG_WARNING("Asked for OpenGL non debug context, a debug one was created");
			if (bool(_contextFlags & SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG) && !bool(finalContextFlags & SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG))
				BLAZE_LOG_WARNING("Asked for OpenGL forward compatible context, a non forward compatible was created");
			if (!bool(_contextFlags & SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG) && bool(finalContextFlags & SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG))
				BLAZE_LOG_WARNING("Asked for OpenGL non forward compatible context, a forward compatible was created");
			if (bool(_contextFlags & SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG) && !bool(finalContextFlags & SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG))
				BLAZE_LOG_WARNING("Asked for OpenGL robust access context, a non robust access was created");
			if (!bool(_contextFlags & SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG) && bool(finalContextFlags & SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG))
				BLAZE_LOG_WARNING("Asked for OpenGL non robust access context, a robust access was created");
			if (bool(_contextFlags & SDL_GL_CONTEXT_RESET_ISOLATION_FLAG) && !bool(finalContextFlags & SDL_GL_CONTEXT_RESET_ISOLATION_FLAG))
				BLAZE_LOG_WARNING("Asked for OpenGL reset isolation context, a non reset isolation was created");
			if (!bool(_contextFlags & SDL_GL_CONTEXT_RESET_ISOLATION_FLAG) && bool(finalContextFlags & SDL_GL_CONTEXT_RESET_ISOLATION_FLAG))
				BLAZE_LOG_WARNING("Asked for OpenGL non reset isolation context, a reset isolation was created");
			properties.contextFlags = GetContextFlags(finalContextFlags);
		}
		else
			BLAZE_LOG_ERROR("SDL_GL_GetAttribute failed when called with SDL_GL_CONTEXT_FLAGS. SDL returned error: \"" + SDL_GetError() + "\"");

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
						BLAZE_LOG_WARNING("Asked for OpenGL " + name1 + " profile, got an invalid value");
					else
						BLAZE_LOG_WARNING("Asked for OpenGL " + name1 + " profile, got " + name2);
			}
			properties.profileType = GetProfileType(finalProfileType);
		}
		else
			BLAZE_LOG_ERROR("SDL_GL_GetAttribute failed when called with SDL_GL_CONTEXT_PROFILE_MASK. SDL returned error: \"" + SDL_GetError() + "\"");

		int finalMajorVersion = 0;
		if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &finalMajorVersion))
		{
			if (finalMajorVersion != properties.majorVersion)
				BLAZE_LOG_WARNING("Asked for OpenGL Major Version {} got {}", properties.majorVersion, finalMajorVersion);
			properties.majorVersion = finalMajorVersion;
		}
		else
			BLAZE_LOG_ERROR("SDL_GL_GetAttribute failed when called with SDL_GL_CONTEXT_MAJOR_VERSION. SDL returned error: \"" + SDL_GetError() + "\"");

		int finalMinorVersion = 0;
		if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &finalMinorVersion))
		{
			if (finalMinorVersion != properties.minorVersion)
				BLAZE_LOG_WARNING("Asked for OpenGL Minor Version {} got {}", properties.minorVersion, finalMinorVersion);
			properties.minorVersion = finalMinorVersion;
		}
		else
			BLAZE_LOG_ERROR("SDL_GL_GetAttribute failed when called with SDL_GL_CONTEXT_MINOR_VERSION. SDL returned error: \"" + SDL_GetError() + "\"");

#define ValidateMinBitInBuffer(ENUM_NAME, PROPERTY_NAME, MESSAGE_NAME) {																			\
			int value = 0;																															\
			if (SDL_GL_GetAttribute(ENUM_NAME, &value))																								\
			{																																		\
				if (value < properties.PROPERTY_NAME)																								\
					if (value == 0)																													\
						BLAZE_LOG_WARNING("Asked for at least {}-bit " MESSAGE_NAME ", got no " MESSAGE_NAME, properties.PROPERTY_NAME);			\
					else																															\
						BLAZE_LOG_WARNING("Asked for at least {}-bit " MESSAGE_NAME ", got{} - bit", properties.PROPERTY_NAME, value);				\
																																					\
				properties.PROPERTY_NAME = value;																									\
			}																																		\
			else																																	\
				BLAZE_LOG_ERROR("SDL_GL_GetAttribute failed when called with " #ENUM_NAME ".SDL returned error : \"" + SDL_GetError() + "\"");	\
		}
	
		ValidateMinBitInBuffer(SDL_GL_DEPTH_SIZE, minDepthBufferBitCount, "depth buffer");
		ValidateMinBitInBuffer(SDL_GL_STENCIL_SIZE, minStencilBufferBitCount, "stencil buffer");
		ValidateMinBitInBuffer(SDL_GL_RED_SIZE, minRedBitCount, "red channel");
		ValidateMinBitInBuffer(SDL_GL_GREEN_SIZE, minGreenBitCount, "green channel");
		ValidateMinBitInBuffer(SDL_GL_BLUE_SIZE, minBlueBitCount, "blue channel");
		ValidateMinBitInBuffer(SDL_GL_ALPHA_SIZE, minAlphaBitCount, "alpha channel");

		int doublebuffer = false;
		if (SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &doublebuffer))
		{
			if (doublebuffer != static_cast<int>(properties.doubleBuffer))
				if (properties.doubleBuffer)
					BLAZE_LOG_WARNING("Asked for double buffering got no double buffering");
				else
					BLAZE_LOG_WARNING("Asked for no double buffering got double buffering");

			properties.doubleBuffer = doublebuffer;
		}
		else
			BLAZE_LOG_ERROR("SDL_GL_GetAttribute failed when called with SDL_GL_DOUBLEBUFFER. SDL returned error: \"" + SDL_GetError() + "\"");

		int framebufferSRGBCapable = false;
		if (SDL_GL_GetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, &framebufferSRGBCapable))
		{
			if (framebufferSRGBCapable != int(properties.framebufferSRGBCapable))
				if (properties.framebufferSRGBCapable)
					BLAZE_LOG_WARNING("Asked for SRGB capable framebuffer, but it wasn't supplied");

			properties.framebufferSRGBCapable = framebufferSRGBCapable;
		}
		else
			BLAZE_LOG_ERROR("SDL_GL_GetAttribute failed when called with SDL_GL_FRAMEBUFFER_SRGB_CAPABLE. SDL returned error: \"" + SDL_GetError() + "\"");

		BLAZE_LOG_INFO("<color=green>Successfully<color/> created OpenGL context {}.{} {} profile ({} R{}/G{}/B{}/A{} D{}/S{} DB{} SRGB{})",
			properties.majorVersion, properties.minorVersion, profileName, GetContextFlagsText(finalContextFlags),
			properties.minRedBitCount, properties.minGreenBitCount, properties.minBlueBitCount, properties.minAlphaBitCount, properties.minDepthBufferBitCount, properties.minStencilBufferBitCount,
			int(properties.doubleBuffer), int(properties.framebufferSRGBCapable)
		);

		return true;
	}

	GraphicsContext_OpenGL::GraphicsContext_OpenGL()
		: GraphicsContext_OpenGL(GraphicsContextProperties_OpenGL())
	{
	}
	GraphicsContext_OpenGL::GraphicsContext_OpenGL(const GraphicsContextProperties_OpenGL& _properties) :
		activeWindowHandle(nullptr), properties(_properties), SDLOpenGLContext(nullptr), initWindow()
	{
		WindowCreateOptions initWindowCreateOptions = _properties.initWindowCreateOptions;
		initWindowCreateOptions.hidden = true;
		initWindowCreateOptions.graphicsAPI = WindowGraphicsAPI::OpenGL;
		initWindow = Window(initWindowCreateOptions);
		activeWindowHandle = initWindow.GetHandle();

		if (!SetOpenGLContextAttributes(_properties))
			return;

		if (!CreateOpenGLContext(initWindow.GetHandle(), SDLOpenGLContext))
			return;

		if (!InitializeGLEW())
			return;

		ValidateContextAttributes(properties);

		glDebugMessageCallback(MessageCallback, nullptr);
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(MessageCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

		glEnable(GL_BLEND);
		glEnable(GL_FRAMEBUFFER_SRGB);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
		glBlendEquation(GL_FUNC_ADD);

		glClearColor(0, 0, 0, 1);
	}
	GraphicsContext_OpenGL::~GraphicsContext_OpenGL()
	{
		SDL_GL_DestroyContext((SDL_GLContext)SDLOpenGLContext);
		SDLOpenGLContext = nullptr;

		initWindow.Destroy();
	}
	void GraphicsContext_OpenGL::MakeContextActive()
	{
		if (SDL_GL_MakeCurrent((SDL_Window*)activeWindowHandle, (SDL_GLContext)SDLOpenGLContext) == false)
			BLAZE_LOG_ERROR("SDL_GL_MakeCurrent() failed. SDL_Error() returnd: \"{}\"", SDL_GetError());
	}
	void GraphicsContext_OpenGL::SetActiveRenderWindow(RenderWindow_OpenGL& renderWindow)
	{
		if (activeWindowHandle == renderWindow.GetHandle())
			return;

		if (activeWindowHandle != nullptr)
		{
			auto context = BlazeEngineContext::GetEngineContext();
			if (context == nullptr)
			{
				BLAZE_LOG_ERROR("Calling SetActiveRenderWindow but the engine context is not valid");
			}
			else
			{
				Window* activeWindow = context->GetWindowFromHandle(activeWindowHandle);

				if (activeWindow != nullptr)
					activeWindow->destructionEventDispatcher.RemoveHandler<&GraphicsContext_OpenGL::ActiveWindowDestroyed>(*this);
			}
		}

		activeWindowHandle = renderWindow.GetHandle();

		renderWindow.destructionEventDispatcher.AddHandler<&GraphicsContext_OpenGL::ActiveWindowDestroyed>(*this);

		if (SDL_GL_MakeCurrent((SDL_Window*)activeWindowHandle, (SDL_GLContext)SDLOpenGLContext) == false)
			BLAZE_LOG_ERROR("SDL_GL_MakeCurrent() failed. SDL_Error() returnd: \"{}\"", SDL_GetError());
	}
	Window GraphicsContext_OpenGL::CreateWindow(const WindowCreateOptions& options)
	{
		if (initWindow.GetHandle() == nullptr || properties.useSeparateInitWindow)
		{
			WindowCreateOptions _options = options;
			_options.graphicsAPI = WindowGraphicsAPI::OpenGL;
			return Window(_options);
		}

		Rectf displayRect = Display::GetDisplayRect(initWindow.GetDisplayIndex());
		Vec2i pos{
			options.pos.x == INT_MAX ? static_cast<int>(displayRect.x) + (static_cast<int>(displayRect.w) - static_cast<int>(options.size.x)) / 2 : options.pos.x,
			options.pos.y == INT_MAX ? static_cast<int>(displayRect.y) + (static_cast<int>(displayRect.h) - static_cast<int>(options.size.y)) / 2 : options.pos.y
		};

		initWindow.SetResizableFlag(options.resizable);
		initWindow.SetBorderlessFlag(options.borderless);
		initWindow.SetMouseGrabbedFlag(options.mouseGrabbed);
		initWindow.SetFocusableFlag(options.focusable);
		initWindow.SetHiddenFlag(options.hidden);
		initWindow.SetAlwaysOnTopFlag(options.alwaysOnTop);
		initWindow.SetTitle(options.title);
		initWindow.SetPos(pos);
		initWindow.SetSize(options.size);
		initWindow.SetWindowPresentMode(options.presentMode);

		return std::move(initWindow);
	}
	void GraphicsContext_OpenGL::RetrieveWindow(Window& window)
	{
		if (!properties.useSeparateInitWindow && initWindow.GetHandle() == nullptr)
			initWindow = std::move(window);
	}
	void GraphicsContext_OpenGL::ActiveWindowDestroyed(const Window::DestructionEvent& event)
	{
		activeWindowHandle = initWindow.GetHandle();
		if (SDL_GL_MakeCurrent((SDL_Window*)activeWindowHandle, (SDL_GLContext)SDLOpenGLContext) == false)
			BLAZE_LOG_ERROR("SDL_GL_MakeCurrent() failed. SDL_Error() returnd: \"{}\"", SDL_GetError());
	}
}