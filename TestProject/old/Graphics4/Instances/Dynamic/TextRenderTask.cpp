#include "pch.h"
#include "TextRenderTask.h"
#include "Graphics4/OpenGL/API_OpenGL.h"
#include "Graphics4/OpenGL/TextRenderTask_OpenGL.h"

namespace Graphics4::Dynamic
{	
	TextRenderTaskBase::TextRenderTaskBase()
		: RenderStage("TextRenderTask", { })
	{
	}
	TextRenderTaskBase::TextRenderTaskBase(TextRenderTaskBase&& other) noexcept
		: DynamicImplementation(std::move(other)), RenderStage(std::move(other))
	{
	}
	TextRenderTaskBase::TextRenderTaskBase(Graphics4::Dynamic::GraphicsContextBase& graphicsContext)
		: RenderStage("TextRenderTask", { })
	{		
		auto* graphicsContext_OpenGL = graphicsContext.TryCast<Graphics4::OpenGL::GraphicsContext_OpenGL>();

		if (graphicsContext_OpenGL != nullptr)
			SetImplementation<Graphics4::OpenGL::TextRenderTask_OpenGL>();
		else
			Debug::Logger::LogError("Blaze Graphics API", "Unsupported implementation");
	}

	Array<Graphics4::Dynamic::SemaphoreBase*> TextRenderTaskBase::PerformStage(ArrayView<Graphics4::Dynamic::SemaphoreBase*> waitSemaphores, Graphics4::Dynamic::FramebufferBase& renderTarget)
	{
		if (auto implementation = GetImplementation())
			implementation->PerformStage(waitSemaphores, renderTarget);

		return { };
	}

	TextRenderTaskBase& TextRenderTaskBase::operator=(TextRenderTaskBase&& other) noexcept
	{
		RenderStage::operator=(std::move(other));
		DynamicImplementation::operator=(std::move(other));
		return *this;		
	}	
}