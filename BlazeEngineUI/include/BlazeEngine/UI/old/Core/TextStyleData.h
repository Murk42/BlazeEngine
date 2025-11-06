#pragma once
#include "BlazeEngineCore/Container/Array.h"
#include "BlazeEngineGraphics/Text/Core/TextStyleSpan.h"

namespace Blaze
{	

	class BLAZE_GRAPHICS_API TextStyleData
	{
	public:
		using Iterator = Array<TextStyleSpan>::Iterator;
		using ConstIterator = Array<TextStyleSpan>::ConstIterator;

		struct TextStyleSpanChangedEvent
		{
			enum class Type
			{
				StyleChange,
				SubStringInserted,
				SubStringErased
			};

			Type type;
			uintMem startIndex; //The start of the changed region
			uintMem endIndex; //The end of the changed region, if the type is SubStringErased this is the end index of string before the change
			ResourceRef<TextStyle> style; //The new style if type is StyleChange, if it is SubStringInserted the style is the style applied to the new sub string, and if it is SubStringErased the style is null
			TextStyleData& textStyleData;
		};
		EventDispatcher<TextStyleSpanChangedEvent> textStyleSpanChangedEventDispathcer;

		TextStyleData();
		TextStyleData(TextStyleData&& other) noexcept = default;

		void ClearAllStyles();
		
		void SetStyle(uintMem startIndex, uintMem endIndex, ResourceRef<TextStyle> style);
		void ClearStyle(uintMem startIndex, uintMem endIndex);
		
		void EraseSubString(uintMem startIndex, uintMem endIndex);
		void InsertSubString(uintMem index, uintMem size);

		ConstIterator FindStyleSpanAt(uintMem characterIndex) const;
		ConstIterator FindFirstStyleSpanStartingAt(uintMem characterIndex) const;

		inline ArrayView<TextStyleSpan> GetAllStyleSpans() const { return spans; }		

		bool operator==(const TextStyleData& other) const = default;
		bool operator!=(const TextStyleData& other) const = default;

		TextStyleData& operator=(TextStyleData&& other) noexcept = default;
	private:		
		Array<TextStyleSpan> spans;
	};
}