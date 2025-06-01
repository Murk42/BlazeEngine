#pragma once
#include "BlazeEngineCore/DataStructures/String.h"
#include "BlazeEngineCore/DataStructures/StringView.h"
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineGraphics/RenderStructure/RenderStream.h"

namespace Blaze::Graphics
{
	class BLAZE_GRAPHICS_API RenderUnit :
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