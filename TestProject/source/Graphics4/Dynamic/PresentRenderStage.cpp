#include "pch.h"
#include "PresentRenderStage.h"
#include "Graphics3/OpenGL/API_OpenGL.h"
#include "Graphics4/OpenGL/PresentRenderStage_OpenGL.h"

namespace Graphics4::Dynamic
{
	PresentRenderStageBase::PresentRenderStageBase(PresentRenderStageBase&& other) noexcept
		: implementation(other.implementation), RenderStage(std::move(other))
	{
		other.implementation = nullptr;
	}
	PresentRenderStageBase::PresentRenderStageBase(Graphics3::Dynamic::RenderWindowBase& window)
		: implementation(nullptr), RenderStage("PresentRenderStageBase", { })
	{
		auto windowOpenGL = window.TryCast< Graphics3::OpenGL::RenderWindow_OpenGL>();
		if (windowOpenGL != nullptr)
		{			
			implementation = DefaultAllocator::Create<Graphics4::OpenGL::PresentRenderStage_OpenGL>(*windowOpenGL);
			return;
		}
		
		Debug::Logger::LogError("Graphics API", "Unsupported graphics API");		
	}
	PresentRenderStageBase::~PresentRenderStageBase()
	{
	}
	void PresentRenderStageBase::Destroy()
	{
		if (implementation != nullptr)
		{
			DefaultAllocator::Destroy(implementation);
			implementation = nullptr;
		}
	}
	Array<Graphics3::Dynamic::SemaphoreBase*> PresentRenderStageBase::PerformStage(ArrayView<Graphics3::Dynamic::SemaphoreBase*> waitSemaphores, Graphics3::Dynamic::FramebufferBase& renderTarget)
	{
		if (implementation != nullptr)
			return implementation->PerformStage(waitSemaphores, renderTarget);
		return { };
	}
	PresentRenderStageBase::PresentRenderStageBase()
		: RenderStage("PresentRenderStageBase", { }), implementation(nullptr)
	{
	}
}
