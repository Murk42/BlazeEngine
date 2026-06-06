#pragma once

namespace Blaze
{
	template<typename Char>
	constexpr GenericStringCharIterator<Char>::GenericStringCharIterator(GenericStringIterator<const Char> it, GenericStringView<Char> string)
		: ptr(it.Ptr()), begin(string.Begin()), end(string.End())
	{		
		if (begin != nullptr && (ptr < begin - 1 || ptr > end))
		{
			BLAZE_LOG_FATAL_BASIC("Invalid iterator");
			return;
		}

		if (ptr >= begin && ptr < end)
		{
			u8StringView subString = { string.FirstIterator(), it + 1 };

			uintMem lastCharIndex = UnicodeChar::FindStartOfLastCharacter(subString);

			if (lastCharIndex == subString.Count())
			{
				BLAZE_LOG_FATAL_BASIC("Invalid iterator");
				return;
			}
		}
	}
	template<typename Char>
	constexpr const Char* GenericStringCharIterator<Char>::Ptr() const
	{
		return ptr;
	}
	template<typename Char>
	constexpr bool GenericStringCharIterator<Char>::IsNull() const
	{
		return ptr == nullptr;
	}
	template<typename Char>
	constexpr UnicodeChar GenericStringCharIterator<Char>::operator*() const
	{
		UnicodeChar ch;
		uintMem count = ch.FromFirstCodePoints(GenericStringView<Char>(ptr, end));

		if (count == 0)
		{
			BLAZE_LOG_FATAL_BASIC("Invalid UTF-8 sequence");
			return ch;
		}

		return ch;
	}
	template<typename Char>
	constexpr GenericStringCharIterator<Char>& GenericStringCharIterator<Char>::operator++()
	{
		if (ptr == nullptr || ptr >= end)
		{
			BLAZE_LOG_FATAL_BASIC("Incrementing invalid iterator");
			return *this;
		}

		UnicodeChar ch;
		uintMem count = ch.FromFirstCodePoints(GenericStringView<Char>(ptr, end));

		if (count == 0)
		{
			BLAZE_LOG_FATAL_BASIC("Invalid UTF-8 sequence");
			return *this;
		}

		ptr += count;

		return *this;
	}
	template<typename Char>
	constexpr GenericStringCharIterator<Char> GenericStringCharIterator<Char>::operator++(int)
	{
		GenericStringCharIterator out{ *this };
		++(*this);
		return out;
	}
	template<typename Char>
	constexpr GenericStringCharIterator<Char>& GenericStringCharIterator<Char>::operator--()
	{
		if (ptr == nullptr || ptr < begin)
		{
			BLAZE_LOG_FATAL_BASIC("Decrementing invalid iterator");
			return *this;
		}

		if (ptr == begin)
		{
			ptr = begin - 1;
			return *this;
		}

		UnicodeChar ch;
		uintMem count = ch.FromLastCodePoints(GenericStringView<Char>(begin, ptr));

		if (count == 0)
		{
			BLAZE_LOG_FATAL_BASIC("Invalid UTF-8 sequence");
			return *this;
		}

		ptr -= count;

		return *this;
	}
	template<typename Char>
	constexpr GenericStringCharIterator<Char> GenericStringCharIterator<Char>::operator--(int)
	{
		GenericStringCharIterator out{ *this };
		--(*this);
		return out;
	}
	template<typename Char>
	constexpr bool GenericStringCharIterator<Char>::operator>(const GenericStringCharIterator& other) const
	{
		return ptr > other.ptr;
	}
	template<typename Char>
	constexpr bool GenericStringCharIterator<Char>::operator>=(const GenericStringCharIterator& other) const
	{
		return ptr >= other.ptr;
	}
	template<typename Char>
	constexpr bool GenericStringCharIterator<Char>::operator<(const GenericStringCharIterator& other) const
	{
		return ptr < other.ptr;
	}
	template<typename Char>
	constexpr bool GenericStringCharIterator<Char>::operator<=(const GenericStringCharIterator& other) const
	{
		return ptr <= other.ptr;
	}
	template<typename Char>
	constexpr GenericStringCharIterator<Char>::operator const Char* () const
	{
		return ptr;
	}
	template<typename Char>
	constexpr GenericStringCharIterator<Char>::operator GenericStringIterator<const Char>() const
	{
		return GenericStringIterator<const Char>(ptr);
	}
	template<typename Char>
	constexpr GenericStringCharIterator<Char>& GenericStringCharIterator<Char>::operator=(const GenericStringIterator<Char>& other)
	{
		ptr = other.ptr;
		begin = other.begin;
		end = other.end;

		return *this;
	}
	template<typename Char>
	constexpr GenericStringCharIterator<Char> GenericStringCharIterator<Char>::AheadIterator(const GenericStringView<Char>& string)
	{
		GenericStringCharIterator out;
		out.begin = string.Begin();
		out.end = string.End();
		out.ptr = out.begin - 1;

		return out;
	}
	template<typename Char>
	constexpr GenericStringCharIterator<Char> GenericStringCharIterator<Char>::FirstIterator(const GenericStringView<Char>& string)
	{
		GenericStringCharIterator out;
		out.begin = string.Begin();
		out.end = string.End();
		out.ptr = out.begin;

		return out;
	}
	template<typename Char>
	constexpr GenericStringCharIterator<Char> GenericStringCharIterator<Char>::LastIterator(const GenericStringView<Char>& string)
	{
		GenericStringCharIterator out;
		out.begin = string.Begin();
		out.end = string.End();
		out.ptr = out.end;

		UnicodeChar ch;
		uintMem count = ch.FromLastCodePoints(string);

		if (count == 0)
		{
			BLAZE_LOG_FATAL_BASIC("Invalid UTF-8 sequence");
			return out;
		}

		out.ptr -= count;

		return out;
	}
	template<typename Char>
	constexpr GenericStringCharIterator<Char> GenericStringCharIterator<Char>::BehindIterator(const GenericStringView<Char>& string)
	{
		GenericStringCharIterator out;
		out.begin = string.Begin();
		out.end = string.End();
		out.ptr = out.end;

		return out;
	}
}