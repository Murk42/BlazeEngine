#pragma once
#include "BlazeEngine/Graphics/Core/RendererBase.h"
#include "BlazeEngine/Graphics/Core/OpenGL/RenderContext_OpenGL.h"
#include "BlazeEngine/Graphics/Core/OpenGL/GraphicsContext_OpenGL.h"

namespace Blaze::Graphics::OpenGL
{	
	class BLAZE_API RendererBase_OpenGL : public virtual RendererBase
	{
	public:
		GraphicsContext_OpenGL& graphicsContext;

		RendererBase_OpenGL(GraphicsContext_OpenGL& graphicsContext) : graphicsContext(graphicsContext) { }
			
		void StartRender(const RenderContext& renderContext) override final
		{
			auto renderContext_OpenGL = dynamic_cast<const RenderContext_OpenGL*>(&renderContext);

			if (renderContext_OpenGL == nullptr)
			{
				BLAZE_LOG_ERROR("Trying to render on a OpenGL renderer with a non-OpenGL render context");
				return;
			}

			StartRender(*renderContext_OpenGL);
		}

		GraphicsContext& GetGraphicsContext() const override final { return graphicsContext; }

		virtual void StartRender(const RenderContext_OpenGL& renderContext) = 0;
	};
}