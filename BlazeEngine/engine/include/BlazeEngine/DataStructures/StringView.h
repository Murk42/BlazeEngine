#pragma once
#include <vector>
#include "BlazeEngine/Core/EngineCore.h"

namespace Blaze
{
	class String;
	class BLAZE_API StringView
	{
		const char* ptr;
		size_t size;
	public:
		StringView();
		constexpr StringView(const StringView&);						
		StringView(const String&);

		constexpr StringView(const char*);
		constexpr StringView(const char*, size_t count);

		inline const char* Ptr() const { return ptr; }
		inline size_t Size() const { return size; }

		size_t Find(char val) const;
		size_t Find(const std::vector<char>& val);
		size_t Count(char val) const;

		constexpr StringView& operator= (const StringView&);				
		StringView& operator= (const String&);
		constexpr StringView& operator= (const char*);

		constexpr bool operator==(const StringView& s) const;				
		constexpr bool operator!=(const StringView& s) const;		
		
		constexpr inline const char& operator[](int index) const { return ptr[index]; }
	};
}
