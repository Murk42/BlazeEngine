#pragma once
#include "CustomRenderBackend.h"

namespace Graphics2
{
	class RenderContext;

	enum class FenceSignalState
	{
		Signaled,
		NotSignaled
	};

	class Fence : public CustomRenderBackend<Fence>
	{
	public:		
		virtual ~Fence();

		virtual void Create(FenceSignalState signalState) = 0;
		virtual void Destroy() = 0;	

		virtual void Wait() = 0;
		virtual void Reset() = 0;
	};
}
