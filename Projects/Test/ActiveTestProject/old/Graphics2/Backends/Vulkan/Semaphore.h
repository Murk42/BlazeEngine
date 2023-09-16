#pragma once
#include "CustomRenderBackend.h"

namespace Graphics2
{
	class RenderContext;

	class Semaphore : public CustomRenderBackend<Semaphore>
	{
	public:		
		virtual ~Semaphore();			
	};
}