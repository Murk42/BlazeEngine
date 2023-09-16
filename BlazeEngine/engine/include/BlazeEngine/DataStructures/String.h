#pragma once

namespace Blaze
{	
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
		String(const char* ptr, size_t size);	
		String(const StringView&);
		String(const char* ptr);		

		~String();

		inline char* Ptr() { return ptr; }
		inline const char* Ptr() const { return ptr; }
		inline size_t Size() const { return size; }		
		
		String SubString(size_t start, size_t size) const;
		void Clear();
		String& Resize(size_t newSize, char fill = ' ');

		inline bool Empty() const { return size == 0; }
		uint32 Hash() const;

		char* begin();
		char* end();

		String& operator= (const StringView& s);
		String& operator= (const String& s);
		String& operator= (String&& s) noexcept;				

		bool operator==(const StringView& s) const;				
		bool operator!=(const StringView& s) const;		

		char& operator[](size_t index) { return ptr[index]; }
		const char& operator[](size_t index) const { return ptr[index]; }		

		String& operator+= (const String&);
		String& operator+= (const StringView&);
		String& operator+= (const char*);
		String& operator+= (const char&);		

		friend BLAZE_API String operator+(const StringView& left, const StringView& right);		
	}; 

	BLAZE_API String operator+(const StringView& left, const StringView& right);

	BLAZE_API String operator+(const StringView& left, const String& right);
	BLAZE_API String operator+(const StringView& left, const char* right);
	BLAZE_API String operator+(const StringView& left, const char& right);

	BLAZE_API String operator+(const String& left, const StringView& right);	
	BLAZE_API String operator+(const char* left, const StringView& right);
	BLAZE_API String operator+(const char& left, const StringView& right);

	BLAZE_API String operator+(const String& left, const String& right);

	BLAZE_API String operator+(const String& left, const char* right);
	BLAZE_API String operator+(const String& left, const char& right);

	BLAZE_API String operator+(const char* left, const String& right);
	BLAZE_API String operator+(const char& left, const String& right);
}