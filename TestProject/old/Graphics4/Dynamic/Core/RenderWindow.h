#pragma once
#include "GraphicsDynamicImplementation.h"

namespace Graphics4::Dynamic
{
	class GraphicsContextBase;
	class RenderWindowFramebufferBase;
	class SemaphoreBase;

	class RenderWindowBase : public GraphicsDynamicImplementation<RenderWindowBase, StringView, Vec2i, Vec2u>
	{
	public:
		RenderWindowBase(const RenderWindowBase&) = delete;
		RenderWindowBase(RenderWindowBase&& other) noexcept;
		RenderWindowBase(GraphicsContextBase& graphicsContext, StringView title, Vec2i pos, Vec2u size);
		virtual ~RenderWindowBase();

		void Destroy();

		virtual bool GetWindow(WindowBase*& window) const;
		virtual Vec2u GetSize() const;

		virtual bool AcquireNextFramebuffer(RenderWindowFramebufferBase*& framebuffer, SemaphoreBase& semaphore);
		virtual void PresentFramebuffer(RenderWindowFramebufferBase& framebuffer, Array<SemaphoreBase*> semaphores);

		RenderWindowBase& operator=(const RenderWindowBase&) = delete;
		RenderWindowBase& operator=(RenderWindowBase&& other) noexcept;
	protected:
		RenderWindowBase();
	};
}