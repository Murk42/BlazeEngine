#pragma once

template<uintMem maxCount> requires (maxCount > 0)
class LocalString
{
	using SizeType =
		std::conditional_t<maxCount <= std::numeric_limits<uint8>::max(), uint8,
		std::conditional_t<maxCount <= std::numeric_limits<uint16>::max(), uint16,
		std::conditional_t<maxCount <= std::numeric_limits<uint32>::max(), uint32, uint64		
		>>>;
	SizeType count;
	char arr[maxCount + 1];
public:
	constexpr LocalString()
		: count(0)
	{
		for (uintMem i = 0; i < maxCount + 1; ++i)
			arr[i] = '\0';
	}
	template<uintMem maxCount2>
	constexpr LocalString(const LocalString<maxCount2>& other) requires (maxCount2 <= maxCount)
		: count(maxCount2)
	{
		for (uintMem i = 0; i < count; ++i)
			arr[i] = other.arr[i];
		for (uintMem i = count; i < maxCount + 1; ++i)
			arr[i] = '\0';
	}
	template<uintMem count2>
	constexpr LocalString(char const (&ptr)[count2]) requires (count2 - 1 <= maxCount)
		: count(std::min(count2 - 1, maxCount))
	{
		for (uintMem i = 0; i < count; ++i)
			arr[i] = ptr[i];
		for (uintMem i = count; i < maxCount + 1; ++i)
			arr[i] = '\0';
	}	
	constexpr LocalString(StringView string)
		: count(std::min(string.Count(), maxCount))
	{
		for (uintMem i = 0; i < count; ++i)
			arr[i] = string[i];
		arr[count] = '\0';
	}

	constexpr uint32 Hash() const
	{
		return std::hash<std::string_view>{ }(std::string_view(arr, maxCount));
	}

	constexpr uintMem Count() const { return static_cast<SizeType>(count); }

	operator String() const
	{
		return String(arr, count);
	}
	constexpr operator StringView() const
	{
		return StringView(arr, count);
	}

	constexpr char& operator[] (uintMem i)
	{
		return arr[i];
	}
	constexpr const char& operator[] (uintMem i) const
	{
		return arr[i];
	}

	template<uintMem maxCount2>
	constexpr bool operator==(const LocalString<maxCount2>& other) const
	{
		if (count != other.count)
			return false;

		for (uint i = 0; i < count; ++i)
			if (arr[i] != other.arr[i])
				return false;

		return true;
	}
	template<uintMem maxCount2>
	constexpr bool operator!=(const LocalString<maxCount2>& other) const
	{
		if (count != other.count)
			return true;

		for (uint i = 0; i < count; ++i)
			if (arr[i] != other.arr[i])
				return true;

		return false;
	}
	template<uintMem count2>
	constexpr bool operator==(char const (&arr)[count2]) const
	{
		if (count != count2)
			return false;

		for (uint i = 0; i < count; ++i)
			if (arr[i] != arr[i])
				return false;

		return true;
	}	
	constexpr bool operator==(const StringView& other) const
	{
		if (count != other.Count())
			return false;

		for (uint i = 0; i < count; ++i)
			if (arr[i] != other[i])
				return false;

		return true;
	}
	constexpr bool operator==(const String& other) const
	{
		if (count != other.Count())
			return false;

		for (uint i = 0; i < count; ++i)
			if (arr[i] != other[i])
				return false;

		return true;
	}
};

template<unsigned N>
LocalString(char const (&)[N]) -> LocalString<N - 1>;