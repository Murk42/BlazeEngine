#pragma once

namespace Blaze
{
	template<typename Char>
	template<TriviallyConvertibleCharacter<Char> Char2>
	constexpr GenericStringIterator<Char>::GenericStringIterator(Char2* ptr)
		: ptr(ptr)
	{
	}
	template<typename Char>
	inline constexpr bool GenericStringIterator<Char>::IsNull() const
	{
		return ptr == nullptr;
	}
	template<typename Char>
	inline constexpr Char* GenericStringIterator<Char>::Ptr() const
	{
		return ptr;
	}
	template<typename Char>
	inline constexpr Char& GenericStringIterator<Char>::operator*() const
	{
		if (ptr == nullptr)
			BLAZE_LOG_FATAL_BASIC("Dereferencing a null iterator");
		return *ptr;
	}
	template<typename Char>
	inline constexpr GenericStringIterator<Char>& GenericStringIterator<Char>::operator++()
	{
		++ptr;
		return *this;
	}
	template<typename Char>
	inline constexpr GenericStringIterator<Char> GenericStringIterator<Char>::operator++(int)
	{
		GenericStringIterator old = *this;
		++ptr;
		return old;
	}
	template<typename Char>
	inline constexpr GenericStringIterator<Char>& GenericStringIterator<Char>::operator--()
	{
		--ptr;
		return *this;
	}
	template<typename Char>
	inline constexpr GenericStringIterator<Char> GenericStringIterator<Char>::operator--(int)
	{
		GenericStringIterator old = *this;
		--ptr;
		return old;
	}
	template<typename Char>
	inline constexpr GenericStringIterator<Char> GenericStringIterator<Char>::operator+(intMem value) const
	{
		return GenericStringIterator(ptr + value);
	}
	template<typename Char>
	inline constexpr GenericStringIterator<Char> GenericStringIterator<Char>::operator-(intMem value) const
	{
		return GenericStringIterator(ptr - value);
	}
	template<typename Char>
	inline constexpr uintMem GenericStringIterator<Char>::operator-(const GenericStringIterator& other) const
	{
		return ptr - other.ptr;
	}
	template<typename Char>
	inline constexpr GenericStringIterator<Char>& GenericStringIterator<Char>::operator+=(intMem offset)
	{
		ptr += offset;
		return *this;
	}
	template<typename Char>
	inline constexpr GenericStringIterator<Char>& GenericStringIterator<Char>::operator-=(intMem offset)
	{
		ptr -= offset;
		return *this;
	}
	template<typename Char>
	inline constexpr bool GenericStringIterator<Char>::operator>(const GenericStringIterator& other) const
	{
		return ptr > other.ptr;
	}
	template<typename Char>
	inline constexpr bool GenericStringIterator<Char>::operator>=(const GenericStringIterator& other) const
	{
		return ptr >= other.ptr;
	}
	template<typename Char>
	inline constexpr bool GenericStringIterator<Char>::operator<(const GenericStringIterator& other) const
	{
		return ptr < other.ptr;
	}
	template<typename Char>
	inline constexpr bool GenericStringIterator<Char>::operator<=(const GenericStringIterator& other) const
	{
		return ptr <= other.ptr;
	}
	template<typename Char>
	inline constexpr GenericStringIterator<Char>::operator Char* () const
	{
		return ptr;
	}
	template<typename Char>
	inline constexpr GenericStringIterator<Char>::operator GenericStringIterator<const Char>() const
	{
		return GenericStringIterator<const Char>(ptr);
	}
}