#pragma once
#include "GraphicsDynamicImplementation.h"

namespace Graphics4::Dynamic
{
	class GraphicsContextBase;	

	class SemaphoreBase : public GraphicsDynamicImplementation<SemaphoreBase>
	{
	public:		
		SemaphoreBase(SemaphoreBase&& other) noexcept;
		SemaphoreBase(GraphicsContextBase& graphicsContext);
		virtual ~SemaphoreBase();
		
		SemaphoreBase& operator=(SemaphoreBase&& other) noexcept;		
	protected:		
		SemaphoreBase();
	};	
}