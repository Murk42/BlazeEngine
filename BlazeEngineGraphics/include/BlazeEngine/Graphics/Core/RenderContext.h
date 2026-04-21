#pragma once
#include "BlazeEngine/Graphics/Core/FramebufferBase.h"

namespace Blaze::Graphics
{
	class RenderContext
	{
	public:
		RenderContext() = default;
		RenderContext(const RenderContext&) = default;
		RenderContext(RenderContext&&) = default;
		virtual ~RenderContext() { };

		virtual FramebufferBase& GetFramebuffer() const = 0;

		RenderContext& operator=(const RenderContext&) = default;
		RenderContext& operator=(RenderContext&&) noexcept = default;
	};
}