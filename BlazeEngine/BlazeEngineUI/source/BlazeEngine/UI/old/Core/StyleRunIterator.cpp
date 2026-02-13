#include "pch.h"
#include "BlazeEngineGraphics/Text/Core/StyleRunIterator.h"

namespace Blaze
{
	StyleRunIterator::StyleRunIterator(const TextStyleData* textStyleData, uintMem textStartIndex, uintMem textEndIndex, ResourceRef<TextStyle> defaultTextStyle) :
		spanIterator(), endSpanIterator(), textEndIndex(textEndIndex), defaultTextStyle(defaultTextStyle)
	{
		if (textStyleData != nullptr)
		{
			spanIterator = textStyleData->FindFirstStyleSpanStartingAt(textStartIndex);
			endSpanIterator = textStyleData->FindFirstStyleSpanStartingAt(textEndIndex);
		}

		runData.startIndex = textStartIndex;
		runData.endIndex = textStartIndex;

		if (spanIterator == endSpanIterator || spanIterator->startIndex != textStartIndex)
			CopyStyleToRun(runData, *defaultTextStyle);
		else
			CopyStyleToRun(runData, *spanIterator->style);

		TryExpandingRun();
	}
	bool StyleRunIterator::IsValid() const
	{
		return runData.startIndex != runData.endIndex;
	} 
	StyleRunIterator& StyleRunIterator::operator++()
	{
		FindNextStyleRun();		
		return *this;
	}
	const StyleRunData* StyleRunIterator::operator->() const 
	{ 
		if (!IsValid())
			BLAZE_ENGINE_GRAPHICS_FATAL("Accessing an invalid style run iterator");

		return &runData; 
	}
	const StyleRunData& StyleRunIterator::operator*() const 
	{
		if (!IsValid())
			BLAZE_ENGINE_GRAPHICS_FATAL("Accessing an invalid style run iterator");

		return runData; 
	}
	bool StyleRunIterator::CanTextStylesBeCombined(const TextStyle& a, const TextStyle& b)
	{
		return a.GetFontFace() == b.GetFontFace() && a.GetFontSize() == b.GetFontSize();
	}
	bool StyleRunIterator::CanTextStylesBeCombined(const StyleRunData& runData, const TextStyle& style)
	{
		return runData.fontFace == style.GetFontFace() && runData.fontSize == style.GetFontSize();
	}
	void StyleRunIterator::CopyStyleToRun(StyleRunData& runData, const TextStyle& style)
	{
		runData.fontFace = style.GetFontFace();
		runData.fontSize = style.GetFontSize();
	}
	void StyleRunIterator::FindNextStyleRun()
	{
		if (!IsValid())
		{
			BLAZE_ENGINE_GRAPHICS_ERROR("Increasing a style run iterator that has reached the end");
			return;
		}

		if (spanIterator == endSpanIterator)
		{
			runData.startIndex = runData.endIndex;
			return;
		}

		if (runData.endIndex != spanIterator->endIndex)
		{
			//Last style run ended at spanIterator.startIndex, between style spans

			runData.startIndex = spanIterator->startIndex;
			runData.endIndex = spanIterator->startIndex;
			CopyStyleToRun(runData, *spanIterator->style);
		}
		else
		{
			++spanIterator;

			runData.startIndex = spanIterator->endIndex;
			runData.endIndex = spanIterator->endIndex;			
			
			CopyStyleToRun(runData, spanIterator == endSpanIterator || spanIterator->startIndex != runData.endIndex ? *defaultTextStyle : *spanIterator->style);
		}	

		TryExpandingRun();
	}	
	void StyleRunIterator::TryExpandingRun()
	{
		while (true)
		{
			if (spanIterator == endSpanIterator)
			{
				runData.endIndex = textEndIndex;
				return;
			}

			ArrayView<TextStyleSpan>::Iterator nextStyleSpanIterator = spanIterator;
			ResourceRef<TextStyle> nextSubRunTextStyle;						
			uintMem nextSubRunEnd;

			if (runData.endIndex != spanIterator->endIndex)
			{
				//Last style run ended at spanIterator.startIndex, between style spans

				nextSubRunTextStyle = spanIterator->style;
				nextSubRunEnd = spanIterator->endIndex;
			}
			else
			{
				//Last style run ended at spanIterator.endIndex, inside a span								

				++nextStyleSpanIterator;

				if (nextStyleSpanIterator == endSpanIterator)
				{
					nextSubRunTextStyle = defaultTextStyle;
					nextSubRunEnd = textEndIndex;					
				}
				else if (runData.endIndex == nextStyleSpanIterator->startIndex)
				{
					nextSubRunTextStyle = nextStyleSpanIterator->style;
					nextSubRunEnd = nextStyleSpanIterator->endIndex;
				}
				else
				{
					nextSubRunTextStyle = defaultTextStyle;
					nextSubRunEnd = nextStyleSpanIterator->startIndex;
				}
			}

			if (!CanTextStylesBeCombined(runData, *nextSubRunTextStyle))
				return;

			spanIterator = nextStyleSpanIterator;
			runData.endIndex = nextSubRunEnd;
		}
	}
}