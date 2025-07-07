#pragma once
#include "BlazeEngineCore/DataStructures/Color.h"
#include "BlazeEngineCore/DataStructures/Array.h"
#include "BlazeEngineCore/DataStructures/ArrayView.h"
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineGraphics/Text/Core/TextStyle.h"
#include "BlazeEngineGraphics/Text/TextContainer/TextContainerReference.h"

namespace Blaze
{
	namespace Graphics
	{
		class FontAtlas;
	}	

	class StyledTextContainer : public TextContainerReference
	{
	public:
		struct TextStyleChangedEvent
		{
			uintMem startIndex;
			uintMem endIndex;
			TextStyle* style;
		};
		EventDispatcher<TextStyleChangedEvent> textStyleChangedEventDispatcher;

		StyledTextContainer(TextContainerBase& container);
		~StyledTextContainer();

		void AddStyleSpan(uintMem startIndex, uintMem endIndex, ResourceRef<TextStyle> style);

		Array<TextStyleSpan>::ConstIterator FindStyleSpanAt(uintMem characterIndex) const;
		Array<TextStyleSpan>::ConstIterator FindFirstStyleSpanStartingAt(uintMem characterIndex) const;
		ArrayView<TextStyleSpan> GetAllStyleSpans() const;
	private:
		Array<TextStyleSpan> spans;

		Array<TextStyleSpan>::Iterator FindStyleSpanAt(uintMem characterIndex);
		Array<TextStyleSpan>::Iterator FindFirstStyleSpanStartingAt(uintMem characterIndex);

		void TextUpdatedEvent(const TextContainerBase::TextUpdatedEvent&);
	};
}