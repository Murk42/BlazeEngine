#pragma once
#include "BlazeEngineCore/DataStructures/UnicodeChar.h"

namespace Blaze
{
	struct CharacterSpan
	{
		UnicodeChar first;
		UnicodeChar last;
	};

	class CharacterSet;

	class BLAZE_API CharacterSetIterator
	{
	public:
		CharacterSetIterator();
		CharacterSetIterator(const CharacterSetIterator& other);
		~CharacterSetIterator();

		bool IsNull() const;

		CharacterSetIterator& operator++();
		CharacterSetIterator operator++(int);
		CharacterSetIterator& operator--();
		CharacterSetIterator operator--(int);

		UnicodeChar operator*() const;

		bool operator==(CharacterSetIterator&) const;
		bool operator!=(CharacterSetIterator&) const;

		CharacterSetIterator& operator=(const CharacterSetIterator& other);
	private:
		CharacterSetIterator(const CharacterSet* characterSet, uintMem spanIndex, uintMem characterIndex);

		const CharacterSet* characterSet;
		uintMem spanIndex;
		uintMem characterIndex;

		friend class CharacterSet;
	};

	class BLAZE_API CharacterSet
	{
	public:
		using Iterator = CharacterSetIterator;

		CharacterSet();
		CharacterSet(CharacterSet&& other) noexcept;
		CharacterSet(ArrayView<CharacterSpan> spans);

		static CharacterSet ASCIICharacterSet();

		uintMem Count() const;

		CharacterSetIterator FirstIterator() const;
		CharacterSetIterator BehindIterator() const;

		CharacterSet& operator=(CharacterSet&& other) noexcept;
	private:
		Array<CharacterSpan> spans;
		uintMem count;

		friend class CharacterSetIterator;
	};

	CharacterSetIterator begin(const CharacterSet& characterSet);
	CharacterSetIterator end(const CharacterSet& characterSet);
}