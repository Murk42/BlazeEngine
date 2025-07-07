#include "pch.h"
#include "BlazeEngineGraphics/Text/Core/TextLayoutCacheBase.h"

namespace Blaze
{
	static bool CheckStyleSpans(ArrayView<TextStyleSpan> styleSpans)
	{
		if (styleSpans.Empty())
			return true;

		FontFaceDirection textDirection = styleSpans[0].style->fontFace->GetDirection();

		uintMem previousStyleSpanEnd = 0;
		for (auto& styleSpan : styleSpans)
		{
			if (styleSpan.style.Get().fontFace.Get().GetDirection() != textDirection)
			{
				BLAZE_ENGINE_GRAPHICS_ERROR("Not all font faces in the font style spans are the same direction");
				return false;
			}

			if (styleSpan.startIndex < previousStyleSpanEnd)
			{
				BLAZE_ENGINE_GRAPHICS_ERROR("Style spans overlap");
				styleSpans = { };
				break;
			}

			previousStyleSpanEnd = styleSpan.endIndex;
		}

		return true;
	}

	TextLayoutCacheBase::TextLayoutCacheBase()
		: dirtyState(DirtyState::NotDirty)
	{
	}
	void TextLayoutCacheBase::SetString(StringViewUTF8 string)
	{
		if (this->string != string)
		{
			this->string = string;
			SetDirtyState(DirtyState::TextShaping);
		}
	}
	void TextLayoutCacheBase::SetStyleSpans(ArrayView<TextStyleSpan> styleSpans)
	{
		if (this->styleSpans != styleSpans)
		{
			if (!CheckStyleSpans(styleSpans))
				return;

			this->styleSpans = styleSpans;
			SetDirtyState(DirtyState::TextShaping);
		}
	}
	void TextLayoutCacheBase::SetDefaultStyle(ResourceRef<TextStyle> defaultTextStyle)
	{
		if (this->defaultTextStyle != defaultTextStyle)
		{
			this->defaultTextStyle = defaultTextStyle;
			SetDirtyState(DirtyState::TextShaping);
		}
	}	
	void TextLayoutCacheBase::CleanCache()
	{
		switch (dirtyState)
		{
		case Blaze::TextLayoutCacheBase::DirtyState::TextShaping:
			textShapingData = ShapeText();
		case Blaze::TextLayoutCacheBase::DirtyState::CalculateClusterBounds:
			clusterBoundsData = CalculateClusterBounds();
		case Blaze::TextLayoutCacheBase::DirtyState::PartitionLines:
			linePartitionData = PartitionLines();
		case Blaze::TextLayoutCacheBase::DirtyState::LayoutText:
			textLayoutData = LayoutText();
		default:
			break;
		}
	}
	void TextLayoutCacheBase::SetDirtyState(DirtyState dirtyState)
	{
		if ((uint)this->dirtyState > (uint)dirtyState)
			this->dirtyState = dirtyState;
	}
}