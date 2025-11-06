#include "pch.h"
#include "BlazeEngine/UI/Common/CharacterSet.h"

namespace Blaze
{
	CharacterSetIterator::CharacterSetIterator()
		: characterSet(nullptr), spanIndex(0), characterIndex(0)
	{
	}
	CharacterSetIterator::CharacterSetIterator(const CharacterSetIterator& other)
		: characterSet(other.characterSet), spanIndex(other.spanIndex), characterIndex(other.characterIndex)
	{
	}
	CharacterSetIterator::~CharacterSetIterator()
	{
	}
	bool CharacterSetIterator::IsNull() const
	{
		return characterSet == nullptr;
	}
	CharacterSetIterator& CharacterSetIterator::operator++()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (characterSet == nullptr)
			BLAZE_LOG_FATAL("Incrementing a null iterator");
#endif

		++characterIndex;

		auto span = characterSet->spans[spanIndex];

		if (characterIndex == span.last.Value() - span.first.Value())
		{
			++spanIndex;
			characterIndex = 0;

			if (spanIndex == characterSet->spans.Count())
			{
				spanIndex = 0;
				characterSet = nullptr;
			}
		}

		return *this;
	}
	CharacterSetIterator CharacterSetIterator::operator++(int)
	{
		CharacterSetIterator copy;
		++copy;
		return copy;
	}
	CharacterSetIterator& CharacterSetIterator::operator--()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (characterSet == nullptr)
			BLAZE_LOG_FATAL("Decrementing a null iterator");
#endif


		if (characterIndex == 0)
			if (spanIndex == 0)
			{
				spanIndex = 0;
				characterSet = nullptr;
			}
			else
			{
				--spanIndex;
				auto span = characterSet->spans[spanIndex];
				characterIndex = span.last.Value() - span.first.Value();
			}
		else
			--characterIndex;

		return *this;
	}
	CharacterSetIterator CharacterSetIterator::operator--(int)
	{
		CharacterSetIterator copy = *this;
		--copy;
		return copy;
	}
	UnicodeChar CharacterSetIterator::operator*() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (characterSet == nullptr)
			BLAZE_LOG_FATAL("Dereferencing a null iterator");
#endif
		return UnicodeChar(static_cast<char32_t>(characterSet->spans[spanIndex].first.Value() + characterIndex));
	}
	bool CharacterSetIterator::operator==(CharacterSetIterator& other) const
	{
		return characterSet == other.characterSet && spanIndex == other.spanIndex && characterIndex == other.characterIndex;
	}
	bool CharacterSetIterator::operator!=(CharacterSetIterator& other) const
	{
		return characterSet != other.characterSet || spanIndex != other.spanIndex || characterIndex != other.characterIndex;
	}
	CharacterSetIterator& CharacterSetIterator::operator=(const CharacterSetIterator& other)
	{
		characterSet = other.characterSet;
		spanIndex = other.spanIndex;
		characterIndex = other.characterIndex;

		return *this;
	}
	CharacterSetIterator::CharacterSetIterator(const CharacterSet* characterSet, uintMem spanIndex, uint32 characterIndex)
		: characterSet(characterSet), spanIndex(spanIndex), characterIndex(characterIndex)
	{
	}

	CharacterSet::CharacterSet()
		: count(0)
	{

	}
	CharacterSet::CharacterSet(CharacterSet&& other) noexcept
		: spans(std::move(other.spans)), count(other.count)
	{
		other.count = 0;
	}
	CharacterSet::CharacterSet(ArrayView<CharacterSpan> spans)
		: spans(std::move(spans)), count(0)
	{
		for (const auto& span : spans)
			count += span.last.Value() - span.first.Value();
	}
	CharacterSet CharacterSet::ASCIICharacterSet()
	{
		CharacterSet cs;
		cs.spans.AddBack(CharacterSpan(char32_t(0), char32_t(127)));
		cs.count = 128;
		return cs;
	}
	uintMem CharacterSet::Count() const
	{
		return count;
	}
	CharacterSetIterator CharacterSet::FirstIterator() const
	{
		return CharacterSetIterator(this, 0, 0);
	}
	CharacterSetIterator CharacterSet::BehindIterator() const
	{
		return CharacterSetIterator(nullptr, 0, 0);
	}
	CharacterSet& CharacterSet::operator=(CharacterSet&& other) noexcept
	{
		spans = std::move(other.spans);
		count = other.count;

		other.count = 0;
		return *this;
	}

	CharacterSetIterator begin(const CharacterSet& characterSet)
	{
		return characterSet.FirstIterator();
	}
	CharacterSetIterator end(const CharacterSet& characterSet)
	{
		return characterSet.BehindIterator();
	}
}