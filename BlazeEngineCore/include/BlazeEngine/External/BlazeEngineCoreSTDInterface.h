#pragma once
#include "BlazeEngine/Core/Common/Hash.h"
#include "BlazeEngine/Core/String/String.h"
#include <thread>
#include <string_view>

namespace Blaze
{
	template<>
	class Hash<std::thread::id>
	{
	public:
		using Type = std::thread::id;

		uint64 Compute(const std::thread::id& value) const
		{
			return std::hash<std::thread::id>()(value);
		}
	};
}

namespace std
{
	template <typename Char, Blaze::AllocatorType Allocator>
	struct hash<Blaze::GenericString<Char, Allocator>>
	{
		inline size_t operator()(const Blaze::GenericString<Char, Allocator>& k) const
		{
			return hash<std::basic_string_view<Char>>()(std::basic_string_view<Char>(k.Ptr(), k.Count()));
		}
	};

	template <typename Char>
	struct hash<Blaze::GenericStringView<Char>>
	{
		inline size_t operator()(const Blaze::GenericStringView<Char>& k) const
		{
			return hash<std::basic_string_view<Char>>()(std::basic_string_view<Char>(k.Ptr(), k.Count()));
		}
	};
}