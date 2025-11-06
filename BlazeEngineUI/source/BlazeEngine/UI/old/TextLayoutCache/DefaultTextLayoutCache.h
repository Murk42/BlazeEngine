#pragma once
#include "BlazeEngineGraphics/Text/Core/TextLayoutCacheBase.h"
#include "BlazeEngineGraphics/Text/Core/FontAtlas.h"

namespace Blaze
{		
	class BLAZE_GRAPHICS_API DefaultTextLayoutCache : public TextLayoutCacheBase
	{
	public:
		DefaultTextLayoutCache();
				
		void SetLineWrapLength(float lineWrapLength);
		
		inline float GetLineWrapLength() const { return lineWrapLength; }
	private:
		TextShapingData ShapeText() override;
		Array<ClusterBoundsData> CalculateClusterBounds() override;
		Array<LinePartitionData> PartitionLines() override;
		TextLayoutData LayoutText() override;		
		
		float lineWrapLength;
	};
}