#pragma once

namespace Experimental::Graphics
{
	class RenderContext;

	enum class FenceSignalState
	{
		Signaled,
		NotSignaled
	};

	class Fence
	{
	public:
		Fence(RenderContext& renderContext, FenceSignalState signalState);
		~Fence();

		void Wait();
		void Reset();		

		inline void* GetHandle() const { return impl; }
	private:
		void* impl;
	};
}
