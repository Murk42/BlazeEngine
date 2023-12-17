#include "pch.h"
#include "ClearRenderStage.h"
#include "Graphics4/OpenGL/API_OpenGL.h"
#include "Graphics4/OpenGL/ClearRenderStage_OpenGL.h"

namespace Graphics4::Dynamic
{
	ClearRenderStageBase::ClearRenderStageBase(ClearRenderStageBase&& other) noexcept
		: RenderStage("ClearRenderStage", { }), DynamicImplementation(std::move(other))
	{
	}
	ClearRenderStageBase::ClearRenderStageBase(Graphics4::Dynamic::RenderWindowBase& window)
		: RenderStage("ClearRenderStage", { })
	{
		auto windowOpenGL = window.TryCast<Graphics4::OpenGL::RenderWindow_OpenGL>();
		if (windowOpenGL != nullptr)
		{
			SetImplementation<Graphics4::OpenGL::ClearRenderStage_OpenGL>(*windowOpenGL);
			return;
		}

		Debug::Logger::LogError("Graphics API", "Unsupported graphics API");
	}
	
	Array<Graphics4::Dynamic::SemaphoreBase*> ClearRenderStageBase::PerformStage(ArrayView<Graphics4::Dynamic::SemaphoreBase*> waitSemaphores, Graphics4::Dynamic::FramebufferBase& renderTarget)
	{
		if (auto impl = GetImplementation())
			impl->PerformStage(waitSemaphores, renderTarget);
		return Array<Graphics4::Dynamic::SemaphoreBase*>();
	}
	ClearRenderStageBase& ClearRenderStageBase::operator=(ClearRenderStageBase&& other) noexcept
	{
		RenderStage::operator=(std::move(other));
		DynamicImplementation::operator=(std::move(other));
		return *this;
	}
	ClearRenderStageBase::ClearRenderStageBase()
		: RenderStage("ClearRenderStage", { })
	{
	}
}