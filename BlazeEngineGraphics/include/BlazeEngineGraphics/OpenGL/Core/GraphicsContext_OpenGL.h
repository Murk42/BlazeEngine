#pragma once
#include "BlazeEngineGraphics\Dynamic/Core/GraphicsContext.h"
#include <SDL2/SDL.h>

namespace Blaze::Graphics::OpenGL
{	
	struct GraphicsContextProperties_OpenGL
	{
		enum class ProfileType
		{
			Core, Compatibility
		};

		uint majorVersion = 4;
		uint minorVersion = 5;
		ProfileType profile = ProfileType::Core;

		uint depthBufferBitCount = 16;
		
		uint stencilBufferBitCount = 8;
	};

	//Not moveable because external references to the object might be invalidated
	class BLAZE_GRAPHICS_API GraphicsContext_OpenGL : public Dynamic::GraphicsContextBase
	{		
	public:
		/*Parity*/GraphicsContext_OpenGL();
		/*Parity*/~GraphicsContext_OpenGL() override;

		/*Parity*/inline String GetImplementationName() const override final { return "OpenGL"; }

		GraphicsContext_OpenGL(const GraphicsContextProperties_OpenGL& properties);				

		WindowSDL CreateWindow(String title, Vec2i pos, Vec2u size);
		void DestroyWindow(WindowSDL& window);

		inline SDL_GLContext GetContext() const { return context; }				
	private:
		uint majorVersion;
		uint minorVersion;
		GraphicsContextProperties_OpenGL::ProfileType profile;
		uint depthBufferBitCount;		
		uint stencilBufferBitCount;

		SDL_GLContext context;

		WindowSDL initWindow;
		WindowSDL::WindowSDLHandle currentWindowHandle;

	};
}