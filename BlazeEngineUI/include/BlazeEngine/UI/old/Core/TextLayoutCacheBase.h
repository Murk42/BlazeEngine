#pragma once
#include "BlazeEngineCore/Container/Array.h"
#include "BlazeEngineCore/Container/ArrayView.h"
#include "BlazeEngineCore/String/u8StringView.h"
#include "BlazeEngineCore/Math/Vector.h"

#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineGraphics/Text/Core/TextStyleData.h"

namespace Blaze
{
	class TextLayoutCacheBase
	{	
	public:
		struct GraphemeData
		{
			uintMem textStartIndex;
			uintMem textEndIndex;
			uintMem lineIndex;
			Vec2f separationPosition; //relative to the line
		};
		struct LineData
		{
			uintMem textStart;
			uintMem textEnd;
			uintMem graphemeStart;
			uintMem graphemeEnd;
			uintMem glyphStart;
			uintMem glyphEnd;

			Vec2f position;
			Vec2f size;
			float ascent;
			float descent;
		};
		struct GlyphData
		{
			ResourceRef<TextStyle> style;
			uint32 glyphIndex;
			Vec2u16 size;
			Vec2i16 clusterOffset;
			//4 bytes unused
		};

		struct GlyphShapingData
		{
			uint32 glyphIndex;
			Vec2i16 advance;
			Vec2i16 offset;
			Vec2u16 size;
			uint32 cluster;
		};

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
		};x

		TextLayoutCacheBase();
		~TextLayoutCacheBase();		

		bool HitTestCharacter(Vec2f position, uintMem& characterDataIndex) const;
		bool HitTestSeparation(Vec2f position, uintMem& characterDataSeparationIndex) const;

		void CreateCache(u8StringView textContainer, TextStyleData* textStyleData, ResourceRef<TextStyle> defaultTextStyle);

		inline u8StringView GetText() const { return text; }
		inline TextStyleData* GetTextStyleData() const { return textStyleData; }
		inline ResourceRef<TextStyle> GetDefaultTextStyle() const { return defaultTextStyle; }

		ArrayView<GraphemeData> GetGraphemeData() const { return graphemeData; }
		ArrayView<LineData> GetLineData() const { return lineData; }
		ArrayView<GlyphData> GetGlyphData() const { return glyphData; }	
	private:
		u8StringView text;
		TextStyleData* textStyleData;
		ResourceRef<TextStyle> defaultTextStyle;

		Array<GraphemeData> graphemeData;
		Array<LineData> lineData;
		Array<GlyphData> glyphData;

		void TextStyleSpanChangedEvent(const TextStyleData::TextStyleSpanChangedEvent& event);

		void CreateTextData();
	};
}
