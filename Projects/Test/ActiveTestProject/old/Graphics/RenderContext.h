#pragma once
#include "VertexLayout.h"

namespace Experimental::Graphics
{		
	void InitializeGraphics();
	void DeinitializeGraphics();

	class RenderContext
	{
	public:
		RenderContext();
		~RenderContext();		

		inline void* GetHandle() const { return impl; }
	private:
		void* impl;			
	};
}