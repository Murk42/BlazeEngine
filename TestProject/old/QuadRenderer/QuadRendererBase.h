#pragma once
#include "../Graphics3/API/API.h"

namespace Graphics3
{
	class QuadRendererBase
	{
	public:
		virtual ~QuadRendererBase() { }
		virtual void RenderQuad(Dynamic::RenderContextBase& context, Dynamic::FramebufferBase& target, std::initializer_list<Dynamic::Semaphore*> waitSemaphores, std::initializer_list<Dynamic::Semaphore*> signalSemaphores) = 0;
	};
}