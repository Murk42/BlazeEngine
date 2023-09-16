#pragma once
#include "CustomRenderBackend.h"

namespace Graphics2
{
	class Framebuffer : public CustomRenderBackend<Framebuffer>
	{
	public:
		virtual ~Framebuffer();
	};
}