#include "pch.h"
#include "TextRenderStage.h"
#include "Graphics3/OpenGL/API_OpenGL.h"
#include "Graphics4/OpenGL/TextRenderStage_OpenGL.h"

namespace Graphics4::Dynamic
{	
	TextRenderStageBase::TextRenderStageBase(TextRenderStageBase&& other) noexcept
		: DynamicImplementation(std::move(other)), RenderStage("TextRenderStage", { })
	{
	}
	TextRenderStageBase::TextRenderStageBase(Graphics3::Dynamic::GraphicsContextBase& graphicsContext)
		: RenderStage("TextRenderStage", { })
	{		
		auto* graphicsContext_OpenGL = graphicsContext.TryCast<Graphics3::OpenGL::GraphicsContext_OpenGL>();

		if (graphicsContext_OpenGL != nullptr)
			SetImplementation(Graphics4::OpenGL::TextRenderStage_OpenGL(*graphicsContext_OpenGL));
		else
			Debug::Logger::LogError("Blaze Graphics API", "Unsupported implementation");
	}

	Array<Graphics3::Dynamic::SemaphoreBase*> TextRenderStageBase::PerformStage(ArrayView<Graphics3::Dynamic::SemaphoreBase*> waitSemaphores, Graphics3::Dynamic::FramebufferBase& renderTarget)
	{
		if (auto implementation = GetImplementation())
			implementation->PerformStage(waitSemaphores, renderTarget);

		return { };
	}

	TextRenderStageBase& TextRenderStageBase::operator=(TextRenderStageBase&& other) noexcept
	{
		RenderStage::operator=(std::move(other));
		DynamicImplementation::operator=(std::move(other));
		return *this;		
	}

	TextRenderStageBase::TextRenderStageBase()
		: RenderStage("TextRenderStage", { })
	{
	}
}