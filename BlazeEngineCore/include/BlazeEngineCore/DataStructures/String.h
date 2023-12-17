#pragma once

namespace Blaze
{	
	class StringView;	
	
	class BLAZE_CORE_API String
	{
		char* ptr;
		uintMem count;
	public:
		String();		
		String(const String& s);
		String(String&& s) noexcept;		

		String(uintMem size);	
		String(const char* ptr, uintMem count);
		String(const StringView&);
		String(const char* ptr);		

		~String();

		inline char* Ptr() { return ptr; }
		inline const char* Ptr() const { return ptr; }
		inline uintMem Count() const { return count; }
		
		String SubString(uintMem start, uintMem count) const;
		void Clear();
		String& Resize(uintMem newCount, char fill = ' ');

		inline bool Empty() const { return count == 0; }
		uint32 Hash() const;

		char* begin();
		char* end();

		String& operator= (const StringView& s);
		String& operator= (const String& s);
		String& operator= (String&& s) noexcept;				

		bool operator==(const StringView& s) const;				
		bool operator!=(const StringView& s) const;		

		char& operator[](uintMem index) { return ptr[index]; }
		const char& operator[](uintMem index) const { return ptr[index]; }		

		String& operator+= (const String&);
		String& operator+= (const StringView&);
		String& operator+= (const char*);
		String& operator+= (const char&);		

		friend BLAZE_CORE_API String operator+(const StringView& left, const StringView& right);		
	}; 

	BLAZE_CORE_API String operator+(const StringView& left, const StringView& right);

	BLAZE_CORE_API String operator+(const StringView& left, const String& right);
	BLAZE_CORE_API String operator+(const StringView& left, const char* right);
	BLAZE_CORE_API String operator+(const StringView& left, const char& right);

	BLAZE_CORE_API String operator+(const String& left, const StringView& right);	
	BLAZE_CORE_API String operator+(const char* left, const StringView& right);
	BLAZE_CORE_API String operator+(const char& left, const StringView& right);

	BLAZE_CORE_API String operator+(const String& left, const String& right);

	BLAZE_CORE_API String operator+(const String& left, const char* right);
	BLAZE_CORE_API String operator+(const String& left, const char& right);

	BLAZE_CORE_API String operator+(const char* left, const String& right);
	BLAZE_CORE_API String operator+(const char& left, const String& right);
}