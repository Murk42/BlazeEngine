#pragma once

namespace Experimental::Graphics
{
	class RenderContext;

	class Framebuffer
	{
	public:
		Framebuffer(void* impl);
		Framebuffer(RenderContext& renderContex);
		~Framebuffer();

		inline void* GetHandle() const { return impl; }		
	private:
		void *impl;				
	};
}