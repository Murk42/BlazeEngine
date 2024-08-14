#pragma once
#include "BlazeEngineGraphics/RenderStructure/RenderStream.h"

namespace Blaze::Graphics
{
	class RenderUnit : 
		public RenderStream
	{
	public:
		RenderUnit(StringView rendererName)
			: rendererName(rendererName)
		{

		}

		inline StringView GetRendererName() const { return rendererName; }
	private:
		String rendererName;
	};
}