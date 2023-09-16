#pragma once

namespace Experimental::Graphics
{
	class RenderContext;

	class Semaphore
	{
	public:
		Semaphore(RenderContext& renderContext);
		~Semaphore();

		inline void* GetHandle() const { return impl; }		
	private:
		void* impl;
	};
}