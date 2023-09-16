#pragma once
#include "../Graphics3/API/API.h"

namespace Graphics3
{
	class EmptyRendererBase
	{
	public:
		virtual ~EmptyRendererBase() { }
		virtual void RenderEmpty(Dynamic::RenderContextBase& context, Dynamic::FramebufferBase& target, Recti renderArea, std::initializer_list<Dynamic::Semaphore*> waitSemaphores, std::initializer_list<Dynamic::Semaphore*> signalSemaphores) = 0;
	};
}