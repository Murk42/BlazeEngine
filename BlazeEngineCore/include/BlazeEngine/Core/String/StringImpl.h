#pragma once
#include "BlazeEngine/Core/String/StringView.h"

namespace Blaze
{
	template<typename Char, AllocatorType Allocator>
	inline GenericString<Char, Allocator>::GenericString()
		: ptr(nullptr), count(0)
	{
	}
	template<typename Char, AllocatorType Allocator>
	inline GenericString<Char, Allocator>::GenericString(GenericString&& other) noexcept
		: ptr(other.ptr), count(other.count)
	{
		other.ptr = nullptr;
		other.count = 0;
	}
	template<typename Char, AllocatorType Allocator>
	inline GenericString<Char, Allocator>::GenericString(const GenericString& other)
		: ptr(nullptr), count(0)
	{
		AssignUnsafe(other.ptr, other.count);
	}
	template<typename Char, AllocatorType Allocator>
	inline GenericString<Char, Allocator>::GenericString(uintMem size, Char fill)
		: ptr(nullptr), count(0)
	{
		if (size == 0)
			return;

		count = size;
		ptr = (Char*)allocator.Allocate((count + 1)* sizeof(Char));

		for (uintMem i = 0; i < count; ++i)
			ptr[i] = fill;

		//Add a null-terminating character
		ptr[count] = 0;
	}
	template<typename Char, AllocatorType Allocator>
	template<TriviallyConvertibleCharacter<Char> Char2>
	inline GenericString<Char, Allocator>::GenericString(GenericStringIterator<const Char2> begin, GenericStringIterator<const Char2> end)
		: ptr(nullptr), count(0)
	{
		AssignUnsafe(reinterpret_cast<const Char*>(begin.Ptr()), end - begin);
	}
	template<typename Char, AllocatorType Allocator>
	template<TriviallyConvertibleCharacter<Char> Char2>
	inline GenericString<Char, Allocator>::GenericString(const Char2* begin, const Char2* end)
		: ptr(nullptr), count(0)
	{
		AssignUnsafe(reinterpret_cast<const Char*>(begin), end - begin);
	}
	template<typename Char, AllocatorType Allocator>
	template<TriviallyConvertibleCharacter<Char> Char2>
	inline GenericString<Char, Allocator>::GenericString(GenericStringIterator<const Char2> begin, uintMem count)
		: ptr(nullptr), count(0)
	{
		AssignUnsafe(reinterpret_cast<const Char*>(begin.Ptr()), count);
	}
	template<typename Char, AllocatorType Allocator>
	template<TriviallyConvertibleCharacter<Char> Char2>
	inline GenericString<Char, Allocator>::GenericString(const Char2* begin, uintMem count)
		: ptr(nullptr), count(0)
	{
		AssignUnsafe(reinterpret_cast<const Char*>(begin), count);
	}
	template<typename Char, AllocatorType Allocator>
	template<TriviallyConvertibleCharacter<Char> Char2, uintMem Size>
	inline GenericString<Char, Allocator>::GenericString(const Char2(&arr)[Size])
		: ptr(nullptr), count(0)
	{
		AssignUnsafe((const Char*)arr, Size);
	}
	template<typename Char, AllocatorType Allocator>
	template<ContainerType Container> requires TriviallyConvertibleCharacter<ContainerValueType<Container>, Char>
	inline GenericString<Char, Allocator>::GenericString(const Container& container)
		: ptr(nullptr), count(0)
	{
		if (begin(container) == end(container))
			return;

		if constexpr (ContiguousIterator<decltype(begin(std::declval<Container>()))>)
		{
			uintMem containerCount = end(container) - begin(container);
			if (*(end(container) - 1) == '\0')
			{
				if (containerCount == 1)
					return;
				else
					count = containerCount - 1;
			}
			else
				count = containerCount;
		}
		else
			for (auto it = begin(container);;)
			{
				++count;

				auto next = it;
				++next;

				if (next == end(container))
				{
					if (*it != '\0')
						++count;

					break;
				}

				it = next;
			}

		if (count == 0)
			return;

		ptr = reinterpret_cast<Char*>(allocator.Allocate((count + 1) * sizeof(Char)));

		if constexpr (ContiguousIterator<decltype(begin(std::declval<Container>()))>)
		{
			const Char* src = reinterpret_cast<const Char*>(&*begin(container));
			memcpy(ptr, src, count * sizeof(Char));
		}
		else
		{
			auto it2 = ptr;
			for (auto it = begin(container); it != end(container); ++it, ++it2)
				*it2 = (Char)*it;
		}

		//Add a null-terminating character
		ptr[count] = 0;
	}
	template<typename Char, AllocatorType Allocator>
	inline GenericString<Char, Allocator>::~GenericString()
	{
		allocator.Free(ptr);
	}
	template<typename Char, AllocatorType Allocator>
	inline bool GenericString<Char, Allocator>::Empty() const
	{
		return count == 0;
	}
	template<typename Char, AllocatorType Allocator>
	inline uint64 GenericString<Char, Allocator>::Hash() const
	{
		return Blaze::Hash<GenericStringView<Char>>().Compute(*this);
	}
	template<typename Char, AllocatorType Allocator>
	inline void GenericString<Char, Allocator>::Clear()
	{
		allocator.Free(ptr);
		ptr = nullptr;
		count = 0;
	}
	template<typename Char, AllocatorType Allocator>
	inline void GenericString<Char, Allocator>::Resize(uintMem newBufferSize, Char fill)
	{
		if (newBufferSize == count)
			return;

		if (newBufferSize == 0)
		{
			Clear();
			return;
		}

		Char* old = ptr;
		ptr = (Char*)allocator.Allocate((newBufferSize + 1) * sizeof(Char));

		if (count == 0)
		{
			for (uintMem i = 0; i < newBufferSize; ++i)
				ptr[i] = fill;
		}
		else if (count < newBufferSize)
		{
			memcpy(ptr, old, count * sizeof(Char));

			allocator.Free(old);

			for (; count < newBufferSize; ++count)
				ptr[count] = fill;
		}
		else if (count > newBufferSize)
			memcpy(ptr, old, newBufferSize * sizeof(Char));

		allocator.Free(old);

		//Add a null-terminating character
		count = newBufferSize;
		ptr[count] = '\0';
	}
	template<typename Char, AllocatorType Allocator>
	inline void GenericString<Char, Allocator>::Append(GenericStringView<Char> other)
	{
		if (other.Empty())
			return;

		if (count == 0)
		{
			AssignUnsafe(other.Ptr(), other.Count());
			return;
		}

		Char* old = ptr;
		uintMem oldCount = count;

		count += other.Count();
		ptr = (Char*)allocator.Allocate((count + 1) * sizeof(Char));

		memcpy(ptr, old, oldCount * sizeof(Char));
		memcpy(ptr + oldCount, other.Ptr(), other.Count() * sizeof(Char));

		ptr[count] = 0;
		allocator.Free(old);
	}
	template<typename Char, AllocatorType Allocator>
	inline void GenericString<Char, Allocator>::Insert(intMem pos, GenericStringView<Char> other)
	{
		if (other.Empty())
			return;

		if (pos < 0)
			pos += count;

		if (pos < 0 || pos > count)
			BLAZE_LOG_FATAL_BASIC("Invalid index");

		if (count == 0)
		{
			AssignUnsafe(other.Ptr(), other.Count());
			return;
		}

		Char* old = ptr;
		uintMem oldCount = count;

		count += other.Count();
		ptr = (Char*)allocator.Allocate((count + 1) * sizeof(Char));

		memcpy(ptr, old, pos * sizeof(Char));
		memcpy(ptr + pos, other.Ptr(), other.Count() * sizeof(Char));
		memcpy(ptr + pos + other.Count(), old + pos, (oldCount - pos) * sizeof(Char));

		ptr[count] = 0;

		allocator.Free(old);
	}
	template<typename Char, AllocatorType Allocator>
	inline void GenericString<Char, Allocator>::Erase(intMem pos, uintMem countToErase)
	{
		if (countToErase == 0)
			return;

		if (pos < 0)
			pos += count;

		if (pos < 0 || pos >= count)
			BLAZE_LOG_FATAL_BASIC("Invalid index");

		if (countToErase > count - pos)
			BLAZE_LOG_FATAL_BASIC("Accesing string outside its range");

		if (countToErase == count)
		{
			Clear();
			return;
		}

		Char* old = ptr;
		uintMem oldCount = count;

		count -= countToErase;
		ptr = (Char*)allocator.Allocate((count + 1) * sizeof(Char));

		memcpy(ptr, old, pos * sizeof(Char));
		memcpy(ptr + pos, old + pos + countToErase, (oldCount - pos - countToErase) * sizeof(Char));

		ptr[count] = 0;

		allocator.Free(old);
	}
	template<typename Char, AllocatorType Allocator>
	inline void GenericString<Char, Allocator>::Replace(GenericStringView<Char> target, GenericStringView<Char> value)
	{
		if (target.Empty())
			BLAZE_LOG_FATAL_BASIC("Target string cannot be empty");

		if (count == 0)
			return;

		uintMem startPosition = 0;
		bool foundAllTargets = false;
		while (!foundAllTargets)
		{
			uintMem positions[128];
			uintMem foundCount = 0;

			while (foundCount < _countof(positions))
			{
				uintMem pos = Find(target, startPosition);

				if (pos == SIZE_MAX)
				{
					foundAllTargets = true;
					break;
				}

				positions[foundCount] = pos;
				++foundCount;

				startPosition = pos + target.Count();
			}

			if (foundCount == 0)
				break;

			Char* old = ptr;
			uintMem oldCount = count;

			count += intMem(value.Count() - target.Count()) * foundCount;
			ptr = (Char*)allocator.Allocate((count + 1) * sizeof(Char));

			uintMem oldPos = 0;
			uintMem newPos = 0;
			for (uintMem i = 0; i < foundCount; ++i)
			{
				memcpy(ptr + newPos, old + oldPos, (positions[i] - oldPos) * sizeof(Char));
				newPos += positions[i] - oldPos;
				memcpy(ptr + newPos, value.Ptr(), value.Count() * sizeof(Char));
				newPos += value.Count();
				oldPos = positions[i] + target.Count();
			}

			memcpy(ptr + newPos, old + oldPos, (oldCount - oldPos) * sizeof(Char));
			ptr[count] = 0;

			allocator.Free(old);

			startPosition += count - oldCount;
		}
	}
	template<typename Char, AllocatorType Allocator>
	inline GenericStringView<Char> GenericString<Char, Allocator>::SubString(intMem start, intMem end) const
	{
		return GenericStringView<Char>(*this).SubString(start, end);
	}
	template<typename Char, AllocatorType Allocator>
	template<AllocatorType ArrayAllocator>
	inline Array<GenericStringView<Char>, ArrayAllocator> GenericString<Char, Allocator>::Split(GenericStringView<Char> separation, uintMem maxSplit) const
	{
		return GenericStringView<Char>(*this).Split<ArrayAllocator>(separation, maxSplit);
	}
	template<typename Char, AllocatorType Allocator>
	template<AllocatorType ArrayAllocator>
	inline Array<GenericStringView<Char>, ArrayAllocator> GenericString<Char, Allocator>::SplitReverse(GenericStringView<Char> separation, uintMem maxSplit) const
	{
			return GenericStringView<Char>(*this).SplitReverse<ArrayAllocator>(separation, maxSplit);
	}
	template<typename Char, AllocatorType Allocator>
	inline uintMem GenericString<Char, Allocator>::Find(const GenericStringView<Char>& value, intMem start) const
	{
		return GenericStringView<Char>(*this).Find(value, start);
	}
	template<typename Char, AllocatorType Allocator>
	inline uintMem GenericString<Char, Allocator>::FindReverse(const GenericStringView<Char>& value, intMem start) const
	{
		return GenericStringView<Char>(*this).FindReverse(value, start);
	}
	template<typename Char, AllocatorType Allocator>
	inline bool GenericString<Char, Allocator>::Contains(const GenericStringView<Char>& value, intMem start) const
	{
		return GenericStringView<Char>(*this).Contains(value, start);
	}
	template<typename Char, AllocatorType Allocator>
	inline bool GenericString<Char, Allocator>::StartsWith(const GenericStringView<Char>& value) const
	{
		return GenericStringView<Char>(*this).StartsWith(value);
	}
	template<typename Char, AllocatorType Allocator>
	inline bool GenericString<Char, Allocator>::EndsWith(const GenericStringView<Char>& value) const
	{
		return GenericStringView<Char>(*this).EndsWith(value);
	}
	template<typename Char, AllocatorType Allocator>
	inline uintMem GenericString<Char, Allocator>::CountOf(const GenericStringView<Char>& value) const
	{
		return GenericStringView<Char>(*this).CountOf(value);
	}
	template<typename Char, AllocatorType Allocator>
	inline GenericStringView<Char> GenericString<Char, Allocator>::Strip(const GenericStringView<Char>& characters) const
	{
		return GenericStringView<Char>(*this).Strip(characters);
	}
	template<typename Char, AllocatorType Allocator>
	inline GenericStringView<Char> GenericString<Char, Allocator>::StripRight(const GenericStringView<Char>& characters) const
	{
		return GenericStringView<Char>(*this).StripRight(characters);
	}
	template<typename Char, AllocatorType Allocator>
	inline GenericStringView<Char> GenericString<Char, Allocator>::StripLeft(const GenericStringView<Char>& characters) const
	{
		return GenericStringView<Char>(*this).StripLeft(characters);
	}
	template<typename Char, AllocatorType Allocator>
	template<AllocatorType StringAllcoator>
	inline GenericString<Char, StringAllcoator> GenericString<Char, Allocator>::LowerCase() const requires SameAs<Char, char>
	{
		return GenericStringView<Char>(*this).LowerCase<StringAllcoator>();
	}
	template<typename Char, AllocatorType Allocator>
	template<AllocatorType StringAllcoator>
	inline GenericString<Char, StringAllcoator> GenericString<Char, Allocator>::UpperCase() const requires SameAs<Char, char>
	{
		return GenericStringView<Char>(*this).UpperCase<StringAllcoator>();
	}
	template<typename Char, AllocatorType Allocator>
	template<AllocatorType StringAllocator1, ContainerTypeOf<GenericStringView<Char>> Container>
	inline GenericString<Char, StringAllocator1> GenericString<Char, Allocator>::Join(const Container& container)
	{
		return GenericStringView<Char>(*this).Join(container);
	}
	template<typename Char, AllocatorType Allocator>
	template<AllocatorType StringAllocator1, IsConvertibleTo<GenericStringView<Char>> ... Args>
	inline GenericString<Char, StringAllocator1> GenericString<Char, Allocator>::Join(const Args& ... args)
	{
		return GenericStringView<Char>(*this).Join(args...);
	}
	template<typename Char, AllocatorType Allocator>
	template<AllocatorType StringAllocator, ContainerType Container> requires IsConvertibleTo<ContainerValueType<Container>, GenericStringView<Char>>
	inline GenericString<Char, StringAllocator> GenericString<Char, Allocator>::Concat(const Container& container)
	{
		return GenericStringView<Char>().Concat<StringAllocator, Container>(container);
	}
	template<typename Char, AllocatorType Allocator>
	template<AllocatorType StringAllocator, IsConvertibleTo<GenericStringView<Char>> ...Args>
	inline GenericString<Char, StringAllocator> GenericString<Char, Allocator>::Concat(const Args & ...args)
	{
		return GenericStringView<Char>().Concat<StringAllocator, Args...>(args...);
	}
	template<typename Char, AllocatorType Allocator>
	template<AllocatorType StringAllcoator>
	inline GenericString<Char, StringAllcoator> GenericString<Char, Allocator>::AlignRight(uintMem width, Char fill)
	{
		return GenericStringView<Char>(*this).AlignRight<StringAllcoator>(width, fill);
	}
	template<typename Char, AllocatorType Allocator>
	template<AllocatorType StringAllcoator>
	inline GenericString<Char, StringAllcoator> GenericString<Char, Allocator>::AlignLeft(uintMem width, Char fill)
	{
		return GenericStringView<Char>(*this).AlignLeft<StringAllcoator>(width, fill);
	}
	template<typename Char, AllocatorType Allocator>
	template<AllocatorType StringAllcoator>
	inline GenericString<Char, StringAllcoator> GenericString<Char, Allocator>::AlignCenter(uintMem width, Char fill)
	{
		return GenericStringView<Char>(*this).AlignCenter<StringAllcoator>(width, fill);
	}
	template<typename Char, AllocatorType Allocator>
	template<StringTypeConvertible<Char> Char2>
	inline bool GenericString<Char, Allocator>::ConvertibleToStringType() const
	{
		return GenericStringView<Char>(*this).ConvertibleToStringType<Char2>();
	}
	template<typename Char, AllocatorType Allocator>
	template<StringTypeConvertible<Char> Char2, AllocatorType Allocator2>
	inline GenericString<Char2, Allocator2> GenericString<Char, Allocator>::ConvertToStringType() const
	{
		return GenericStringView<Char>(*this).ConvertToStringType<Char2>();

	}
	template<typename Char, AllocatorType Allocator>
	template<IntegerType T>
	inline bool GenericString<Char, Allocator>::ConvertToInteger(T& to, uint base, GenericStringView<Char>* remainingString) const
	{
		return GenericStringView<Char>(*this).ConvertToInteger(to, base, remainingString);
	}
	template<typename Char, AllocatorType Allocator>
	template<DecimalType T>
	inline bool GenericString<Char, Allocator>::ConvertToDecimal(T& to, FloatStringConvertFormat format, GenericStringView<Char>* remainingString) const
	{
		return GenericStringView<Char>(*this).ConvertToDecimal(to, format, remainingString);
	}
	template<typename Char, AllocatorType Allocator>
	template<IntegerType T, AllocatorType StringAllocator>
	inline GenericString<Char, StringAllocator> GenericString<Char, Allocator>::Parse(const T& value, uint base, bool uppercase, bool showPositiveSign)
	{
		return GenericStringView<Char>().Parse<T>(value, base, uppercase, showPositiveSign);
	}
	template<typename Char, AllocatorType Allocator>
	template<DecimalType T, AllocatorType StringAllocator>
	inline GenericString<Char, StringAllocator> GenericString<Char, Allocator>::Parse(const T& value, FloatStringParseFormat format)
	{
		return GenericStringView<Char>().Parse<T>(value, format);
	}
	template<typename Char, AllocatorType Allocator>
	inline Char* GenericString<Char, Allocator>::Ptr()
	{
		return ptr;
	}
	template<typename Char, AllocatorType Allocator>
	inline const Char* GenericString<Char, Allocator>::Ptr() const
	{
		return ptr;
	}
	template<typename Char, AllocatorType Allocator>
	inline uintMem GenericString<Char, Allocator>::Count() const
	{
		return count;
	}
	template<typename Char, AllocatorType Allocator>
	inline Char* GenericString<Char, Allocator>::Begin()
	{
		return ptr;
	}
	template<typename Char, AllocatorType Allocator>
	inline const Char* GenericString<Char, Allocator>::Begin() const
	{
		return ptr;
	}
	template<typename Char, AllocatorType Allocator>
	inline Char* GenericString<Char, Allocator>::End()
	{
		return ptr + count;
	}
	template<typename Char, AllocatorType Allocator>
	inline const Char* GenericString<Char, Allocator>::End() const
	{
		return ptr + count;
	}
	template<typename Char, AllocatorType Allocator>
	const Char& GenericString<Char, Allocator>::First() const
	{
		if (count == 0)
			BLAZE_LOG_FATAL_BASIC("Accesing an empty string");

		return *ptr;
	}
	template<typename Char, AllocatorType Allocator>
	const Char& GenericString<Char, Allocator>::Last() const
	{
		if (count == 0)
			BLAZE_LOG_FATAL_BASIC("Accesing an empty string");

		return *(ptr + count - 1);
	}
	template<typename Char, AllocatorType Allocator>
	auto GenericString<Char, Allocator>::FirstIterator() -> IteratorType
	{
		return { ptr };
	}
	template<typename Char, AllocatorType Allocator>
	auto GenericString<Char, Allocator>::FirstIterator() const -> ConstIteratorType
	{
		return { ptr };
	}
	template<typename Char, AllocatorType Allocator>
	auto GenericString<Char, Allocator>::LastIterator() -> IteratorType
	{
		return { ptr + count - 1 };
	}
	template<typename Char, AllocatorType Allocator>
	auto GenericString<Char, Allocator>::LastIterator() const -> ConstIteratorType
	{
		return { ptr + count - 1 };
	}
	template<typename Char, AllocatorType Allocator>
	auto GenericString<Char, Allocator>::AheadIterator() -> IteratorType
	{
		return { ptr - 1 };
	}
	template<typename Char, AllocatorType Allocator>
	auto GenericString<Char, Allocator>::AheadIterator() const -> ConstIteratorType
	{
		return { ptr - 1 };
	}
	template<typename Char, AllocatorType Allocator>
	auto GenericString<Char, Allocator>::BehindIterator() -> IteratorType
	{
		return { ptr + count };
	}
	template<typename Char, AllocatorType Allocator>
	auto GenericString<Char, Allocator>::BehindIterator() const -> ConstIteratorType
	{
		return { ptr + count };
	}
	template<typename Char, AllocatorType Allocator>
	Char& GenericString<Char, Allocator>::operator[](intMem index)
	{
		if (index < 0)
			index += count;
		else if (static_cast<uintMem>(index) >= count)
			BLAZE_LOG_FATAL_BASIC("Accesing an string outside its bounds");

		return ptr[index];
	}
	template<typename Char, AllocatorType Allocator>
	const Char& GenericString<Char, Allocator>::operator[](intMem index) const
	{
		if (index < 0)
			index += count;

		else if (static_cast<uintMem>(index) >= count)
			BLAZE_LOG_FATAL_BASIC("Accesing an string outside its bounds");

		return ptr[index];
	}
	template<typename Char, AllocatorType Allocator>
	bool GenericString<Char, Allocator>::operator==(const GenericString& other) const
	{
		return GenericStringView<Char>(*this).operator==(other);
	}
	template<typename Char, AllocatorType Allocator>
	bool GenericString<Char, Allocator>::operator!=(const GenericString& other) const
	{
		return GenericStringView<Char>(*this).operator!=(other);
	}

	template<typename Char, AllocatorType Allocator>
	template<AllocatorType OtherAllocator>
	inline GenericString<Char, Allocator>& GenericString<Char, Allocator>::operator+=(const GenericString<Char, OtherAllocator>& other)
	{
		*this = GenericStringView<Char>(*this) + GenericStringView<Char>(other);
		return *this;
	}
	template<typename Char, AllocatorType Allocator>
	inline GenericString<Char, Allocator>& GenericString<Char, Allocator>::operator+=(const GenericStringView<Char>& other)
	{
		*this = GenericStringView<Char>(*this) + GenericStringView<Char>(other);
		return *this;
	}
	template<typename Char, AllocatorType Allocator>
	template<uintMem Size>
	inline GenericString<Char, Allocator>& GenericString<Char, Allocator>::operator+=(const Char(&other)[Size])
	{
		*this = GenericStringView<Char>(*this) + GenericStringView<Char>(other);
		return *this;
	}
	template<typename Char, AllocatorType Allocator>
	inline GenericString<Char, Allocator>& GenericString<Char, Allocator>::operator+=(const Char& other)
	{
		*this = GenericStringView<Char>(*this) + GenericStringView<Char>(&other, 1);
		return *this;
	}
	template<typename Char, AllocatorType Allocator>
	inline GenericString<Char, Allocator>& GenericString<Char, Allocator>::operator=(GenericString&& other) noexcept
	{
		allocator.Free(ptr);
		ptr = other.ptr;
		count = other.count;
		other.ptr = nullptr;
		other.count = 0;
		return *this;
	}
	template<typename Char, AllocatorType Allocator>
	GenericString<Char, Allocator>& GenericString<Char, Allocator>::operator=(const GenericString& other)
	{
		allocator.Free(ptr);
		ptr = nullptr;
		count = 0;
		AssignUnsafe(other.ptr, other.count);
		return *this;
	}
	template<typename Char, AllocatorType Allocator>
	template<TriviallyConvertibleCharacter<Char> Char2, uintMem Size>
	GenericString<Char, Allocator>& GenericString<Char, Allocator>::operator=(const Char2(&arr)[Size])
	{
		allocator.Free(ptr);
		ptr = nullptr;
		count = 0;
		AssignUnsafe((Char*)arr, Size);
		return *this;
	}
	template<typename Char, AllocatorType Allocator>
	template<ContainerConvertibleToString<const Char> Container>
	GenericString<Char, Allocator>& GenericString<Char, Allocator>::operator=(const Container& container)
	{
		allocator.Free(ptr);
		ptr = nullptr;
		count = 0;
		AssignUnsafe((Char*)container.Ptr(), container.Count());
		return *this;
	}
	template<typename Char, AllocatorType Allocator>
	inline void GenericString<Char, Allocator>::AssignUnsafe(const Char* ptr, uintMem bufferSize)
	{
		if (bufferSize == 0)
			return;

		if (ptr[bufferSize - 1] == '\0')
		{
			if (bufferSize == 1)
				return;

			count = bufferSize - 1;
		}
		else
			count = bufferSize;

		this->ptr = (Char*)allocator.Allocate(sizeof(Char) * (bufferSize + 1));
		memcpy(this->ptr, ptr, sizeof(Char) * count);
		this->ptr[count] = 0;
	}

	template<typename Char, AllocatorType Allocator1, AllocatorType Allocator2>
	GenericString<Char, Allocator1> operator+(const GenericString<Char, Allocator1>& a, const GenericString<Char, Allocator2>& b)
	{
		return operator+<Char, Allocator1>(GenericStringView<Char>(a), GenericStringView<Char>(b));
	}
	template<typename Char, AllocatorType Allocator>
	GenericString<Char, Allocator> operator+(const GenericString<Char, Allocator>& a, const GenericStringView<Char>& b)
	{
		return operator+<Char, Allocator>(GenericStringView<Char>(a), GenericStringView<Char>(b));
	}
	template<typename Char, AllocatorType Allocator, TriviallyConvertibleCharacter<Char> Char2, uintMem Size>
	GenericString<Char, Allocator> operator+(const GenericString<Char, Allocator>& a, const Char2(&b)[Size])
	{
		return operator+<Char, Allocator>(GenericStringView<Char>(a), GenericStringView<Char>(b));
	}
	template<typename Char, AllocatorType Allocator, TriviallyConvertibleCharacter<Char> Char2, uintMem Size>
	GenericString<Char, Allocator> operator+(const Char2(&a)[Size], const GenericString<Char, Allocator>& b)
	{
		return operator+<Char, Allocator>(GenericStringView<Char>(a), GenericStringView<Char>(b));
	}

	template<typename Char, AllocatorType Allocator>
	auto begin(GenericString<Char, Allocator>& string)
	{
		return string.FirstIterator();
	}
	template<typename Char, AllocatorType Allocator>
	auto begin(const GenericString<Char, Allocator>& string)
	{
		return string.FirstIterator();
	}
	template<typename Char, AllocatorType Allocator>
	auto end(GenericString<Char, Allocator>& string)
	{
		return string.BehindIterator();
	}
	template<typename Char, AllocatorType Allocator>
	auto end(const GenericString<Char, Allocator>& string)
	{
		return string.BehindIterator();
	}

	template<typename Char, AllocatorType Allocator>
	class Hash<GenericString<Char, Allocator>>
	{
	public:
		using Type = GenericString<Char, Allocator>;

		uint64 Compute(const GenericString<Char, Allocator>& string)
		{
			return GenericStringView<Char>(string).Hash();
		}
	};
}