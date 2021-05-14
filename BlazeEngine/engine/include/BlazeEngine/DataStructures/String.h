#pragma once
#include <vector>
#include <type_traits>
#include "BlazeEngine/Core/EngineCore.h"

namespace Blaze
{
	constexpr struct format_string_t { } format_string;
	class StringView;

	class BLAZE_API String
	{
		char* ptr;
		size_t size;
	public:
		String();		
		String(const String& s);
		String(String&& s) noexcept;

		String(size_t size);
		String(const char* ptr);
		String(char* ptr, size_t size, bool copy = true);
		String(const char& c, size_t size);
		String(const StringView& c);

		template<typename ... T>
		String(format_string_t, const char* format, T ... args)
			: ptr(nullptr), size(snprintf(nullptr, 0, format, args...))
		{
			ptr = new char[size + 1];
			snprintf(ptr, size + 1, format, args...);
		}

		~String();

		inline char* Ptr() { return ptr; }
		inline const char* Ptr() const { return ptr; }
		inline size_t Size() const { return size; }		

		void Clear();
		void Resize(size_t newSize, char def = ' ');		

		void Append(const String& s);
		void Append(const char* _ptr);
		void Append(char* _ptr, size_t _size);
		void Append(const char& c);

		void Replace(char oldVal, char newVal);
		void Reverse();
		size_t Find(char val) const;
		size_t Find(const std::vector<char>& val);
		size_t Count(char val) const;

		void operator+= (const String& s) ;
		void operator+= (const char* _ptr);
		void operator+= (const char& c);

		String operator+ (const String& s) const;
		String operator+ (const char* _ptr) const;
		String operator+ (const char& c) const;

		String& operator= (const String& s);
		String& operator= (String&& s) noexcept;
		String& operator= (const char* _ptr);		

		bool operator==(const String& s) const;
		bool operator==(const char* _ptr) const;				
		bool operator!=(const String& s) const;
		bool operator!=(const char* _ptr) const;		

		char& operator[](int index);
		const char& operator[](int index) const;

		friend String BLAZE_API operator+ (const char* ptr, const String& s);
		friend String BLAZE_API operator+ (const char& c, const String& s);
	};

	String BLAZE_API operator+ (const char* _ptr, const String& s);
	String BLAZE_API operator+ (const char& c, const String& s);
	bool BLAZE_API operator==(const char* _ptr, const String& s);
	bool BLAZE_API operator!=(const char* _ptr, const String& s);
}