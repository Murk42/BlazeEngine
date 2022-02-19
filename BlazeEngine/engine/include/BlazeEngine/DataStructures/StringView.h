#pragma once
#include "BlazeEngine/Core/EngineCore.h"

namespace Blaze
{
	class String;
	class StringViewUTF8;
	class BLAZE_API StringView
	{
		const char* ptr;
		size_t size;
	public:
		StringView();
		StringView(const StringView&);
		
		StringView(const char*);
		StringView(const char*, size_t count);
		StringView(const char* begin, const char* end);
		StringView(const String&);		

		inline const char* Ptr() const { return ptr; }
		inline size_t Size() const { return size; }	

		size_t Count(char ch) const;

		StringView& operator= (const StringView&);						

		bool operator==(const StringView& s) const;				
		bool operator!=(const StringView& s) const;		
		
		inline const char& operator[](int index) const { return ptr[index]; }
	};
}
