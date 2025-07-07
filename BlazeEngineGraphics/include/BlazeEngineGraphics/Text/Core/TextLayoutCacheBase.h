#pragma once
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineCore/DataStructures/ArrayView.h"
#include "BlazeEngineCore/DataStructures/Array.h"
#include "BlazeEngineCore/Math/Vector.h"
#include "BlazeEngineGraphics/Text/Core/TextStyle.h"

namespace Blaze
{
	class TextLayoutCacheBase
	{	
	public:
		struct TextShapingData
		{
			struct ClusterShapingData
			{
				uintMem textStartIndex;
				uintMem textEndIndex;
				uintMem glyphStartIndex;
				uintMem glyphEndIndex;
				ResourceRef<TextStyle> style;
				Vec2f advance;
			};
			struct GlyphShapingData
			{
				uint32 glyphIndex;
				Vec2u16 size;
				Vec2i16 clusterOffset;
			};

			Array<ClusterShapingData> clusters;
			Array<GlyphShapingData> glyphs;
		};
		struct ClusterBoundsData
		{
			int16 minX;
			int16 maxX;
			int16 minY;
			int16 maxY;
		};
		struct LinePartitionData
		{
			uintMem textStart;
			uintMem textEnd;
			uintMem clusterStart;
			uintMem clusterEnd;
		};		
		struct TextLayoutData
		{
			struct LineLayoutData
			{
				Vec2f position;
				Vec2f size;
				float ascent;
				float descent;
			};
			struct ClusterLayoutData
			{
				uintMem lineIndex;
				Vec2f lineOffset;
			};

			Array<LineLayoutData> lines;
			Array<ClusterLayoutData> clusters;
		};

		TextLayoutCacheBase();

		bool HitTestCharacter(Vec2f position, uintMem& characterDataIndex) const;
		bool HitTestSeparation(Vec2f position, uintMem& characterDataSeparationIndex) const;
		
		void SetString(StringViewUTF8 string);
		void SetStyleSpans(ArrayView<TextStyleSpan> styleSpans);
		void SetDefaultStyle(ResourceRef<TextStyle> defaultTextStyle);

		void CleanCache();

		inline StringViewUTF8 GetString() const { return string; }
		inline ArrayView<TextStyleSpan> GetStyleSpans() const { return styleSpans; }
		inline ResourceRef<TextStyle> GetDefaultStyle() const { return defaultTextStyle; }
		inline const TextShapingData& GetTextShapingData() const { return textShapingData; }
		inline ArrayView<ClusterBoundsData> GetClusterBoundsData() const { return clusterBoundsData; }
		inline ArrayView<LinePartitionData> GetLinePartitionData() const { return linePartitionData; }
		inline const TextLayoutData GetTextLayoutData() const { return textLayoutData; }
	protected:
		enum class DirtyState : uint8
		{
			TextShaping,
			CalculateClusterBounds,
			PartitionLines,
			LayoutText,
			NotDirty
		};

		void SetDirtyState(DirtyState dirtyState);
		inline DirtyState GetDirtyState() const { return dirtyState; }

		virtual TextShapingData ShapeText() = 0;
		virtual Array<ClusterBoundsData> CalculateClusterBounds() = 0;
		virtual Array<LinePartitionData> PartitionLines() = 0;
		virtual TextLayoutData LayoutText() = 0;
	private:
		StringViewUTF8 string;
		ArrayView<TextStyleSpan> styleSpans;
		ResourceRef<TextStyle> defaultTextStyle;
		DirtyState dirtyState;

		TextShapingData textShapingData;
		Array<ClusterBoundsData> clusterBoundsData;
		Array<LinePartitionData> linePartitionData;
		TextLayoutData textLayoutData;
	};
}