#pragma once
#include "DynamicImplementation.h"
#include "Graphics4/Dynamic/API_dynamic.h"

namespace Graphics4
{
	class RenderPipelineBase;

	class RenderStageBase : public DynamicImplementation<RenderStageBase>
	{
	public:		
		RenderStageBase(Graphics4::Dynamic::GraphicsContextBase& graphicsContext);	

		//template<ImplementationOf<RenderStageBase> T, ImplementationOf<Graphics4::Dynamic::GraphicsContextBase>>
		//static void RegisterImplementation();
	private:				
		UnorderedArray<RenderPipelineBase*> pipelines;
	};
}