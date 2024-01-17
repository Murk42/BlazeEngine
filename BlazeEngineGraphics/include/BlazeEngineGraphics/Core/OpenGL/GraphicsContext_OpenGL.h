#pragma once
#include "BlazeEngineGraphics/Core/Dynamic/GraphicsContext.h"

namespace Blaze::Graphics::OpenGL
{	
	enum class ProfileType
	{
		Core, Compatibility, ES
	};
	enum class ContextFlags
	{
		None = 0, Debug = 1, ForwardCompatible = 2, RobustAccess = 4, ResetIsolation = 8, Default = 16
	};
	enum class ReleaseBehaviour
	{
		None, Flush
	};

	ENUM_CLASS_BITWISE_OPERATIONS(ContextFlags)

	struct GraphicsContextProperties_OpenGL
	{		
		uint majorVersion = 4;
		uint minorVersion = 6;
		ProfileType profileType = ProfileType::Core;
		ContextFlags contextFlags = ContextFlags::None;// = ContextFlags::Debug | ContextFlags::RobustAccess | ContextFlags::ForwardCompatible | ContextFlags::ResetIsolation;
		ReleaseBehaviour releaseBehaviour = ReleaseBehaviour::Flush;

		uint depthBufferBitCount = 16;
		
		uint stencilBufferBitCount = 0;
	};

	struct WindowSDLCreateOptions_OpenGL
	{
		StringUTF8 title = StringUTF8();
		Vec2i pos = Vec2i(INT_MAX, INT_MAX);
		Vec2u size = Vec2u(1, 1);
		WindowSDLOpenMode openMode = WindowSDLOpenMode::Normal;
		WindowSDLStyleFlags styleFlags = WindowSDLStyleFlags::Resizable;
	};

	//Not moveable because external references to the object might be invalidated
	class BLAZE_GRAPHICS_API GraphicsContext_OpenGL : public Dynamic::GraphicsContextBase
	{		
	public:
		/*Parity*/GraphicsContext_OpenGL();
		/*Parity*/~GraphicsContext_OpenGL() override;

		/*Parity*/inline String GetImplementationName() const override final { return "OpenGL"; }

		GraphicsContext_OpenGL(const GraphicsContextProperties_OpenGL& properties);				

		WindowSDL CreateWindow(WindowSDLCreateOptions_OpenGL createOptions);
		void DestroyWindow(WindowSDL& window);

		inline void* GetContext() const { return context; }				
	private:
		uint majorVersion;
		uint minorVersion;
		ProfileType profileType;
		ContextFlags contextFlags;
		ReleaseBehaviour releaseBehaviour;
		uint depthBufferBitCount;		
		uint stencilBufferBitCount;

		void* context;

		WindowSDL initWindow;
		WindowSDL::WindowSDLHandle currentWindowHandle;

	};
}