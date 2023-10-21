#include "pch.h"
#include "ClearRenderStage.h"
#include "Graphics3/OpenGL/API_OpenGL.h"
#include "Graphics4/OpenGL/ClearRenderStage_OpenGL.h"

namespace Graphics4::Dynamic
{
	ClearRenderStageBase::ClearRenderStageBase(ClearRenderStageBase&& other) noexcept
		: RenderStage("ClearRenderStage", { }), DynamicImplementation(std::move(other))
	{
	}
	ClearRenderStageBase::ClearRenderStageBase(Graphics3::Dynamic::RenderWindowBase& window)
		: RenderStage("ClearRenderStage", { })
	{
		auto windowOpenGL = window.TryCast<Graphics3::OpenGL::RenderWindow_OpenGL>();
		if (windowOpenGL != nullptr)
		{
			SetImplementation(Graphics4::OpenGL::ClearRenderStage_OpenGL(*windowOpenGL));
			return;
		}

		Debug::Logger::LogError("Graphics API", "Unsupported graphics API");
	}
	
	Array<Graphics3::Dynamic::SemaphoreBase*> ClearRenderStageBase::PerformStage(ArrayView<Graphics3::Dynamic::SemaphoreBase*> waitSemaphores, Graphics3::Dynamic::FramebufferBase& renderTarget)
	{
		if (auto impl = GetImplementation())
			impl->PerformStage(waitSemaphores, renderTarget);
		return Array<Graphics3::Dynamic::SemaphoreBase*>();
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