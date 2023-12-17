#pragma once
#include "DynamicImplementation.h"

namespace Graphics4::Dynamic
{
	class FramebufferBase;
	class RenderWindowBase;

	class RenderWindowFramebufferBase : public DynamicImplementation<RenderWindowFramebufferBase>
	{
	public:
		RenderWindowFramebufferBase(const RenderWindowFramebufferBase&) = delete;
		RenderWindowFramebufferBase(RenderWindowFramebufferBase&& other) noexcept;
		virtual ~RenderWindowFramebufferBase();

		virtual bool GetFramebuffer(FramebufferBase*& framebuffer);
		virtual bool GetRenderWindow(RenderWindowBase*& renderWindow) const;

		RenderWindowFramebufferBase& operator=(const RenderWindowFramebufferBase&) = delete;
		RenderWindowFramebufferBase& operator=(RenderWindowFramebufferBase&& other) noexcept;
	protected:
		RenderWindowFramebufferBase();
	};
}