#include "pch.h"
#include "TextRenderTask_OpenGL.h"
#include "TextRenderer_OpenGL.h"

namespace Graphics4::OpenGL
{
    TextRenderTask_OpenGL::TextRenderTask_OpenGL()
		: renderer(nullptr)
    {		
    }

	TextRenderTask_OpenGL::TextRenderTask_OpenGL(TextRenderTask_OpenGL&& other) noexcept
		: TextRenderTaskBase(std::move(other)), renderer(other.renderer), renderCache(std::move(other.renderCache))
	{
        other.renderer = nullptr;
	}

    Array<Graphics4::Dynamic::SemaphoreBase*> TextRenderTask_OpenGL::PerformStage(ArrayView<Graphics4::Dynamic::SemaphoreBase*> waitSemaphores, Graphics4::Dynamic::FramebufferBase& renderTarget)
    {
        PerformStage({ }, (Graphics4::OpenGL::Framebuffer_OpenGL&)renderTarget);
        return { };        
    }
    Array<Graphics4::OpenGL::Semaphore_OpenGL*> TextRenderTask_OpenGL::PerformStage(ArrayView<Graphics4::OpenGL::Semaphore_OpenGL*> waitSemaphores, Graphics4::OpenGL::Framebuffer_OpenGL& renderTarget)
    {
        if (renderer != nullptr)
            renderer->Render(renderCache, renderTarget);
        return { };
    }
	TextRenderTask_OpenGL& TextRenderTask_OpenGL::operator=(TextRenderTask_OpenGL&& other) noexcept
	{
        renderCache = std::move(other.renderCache);
        renderer = other.renderer;
        other.renderer = nullptr;
		TextRenderTaskBase::operator=(std::move(other));
		return *this;		
	}
}
