#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/StringView.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/Vector.h"

namespace Blaze
{	
	struct StringView;

	BLAZE_API size_t UTF8Size(const char* ptr);
	BLAZE_API size_t UTF8ToUnicode(const char* ptr, uint32& character);

	//thread-safe
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
		String(const char* ptr, size_t size);	
		String(const StringView&);

		~String();

		inline char* Ptr() { return ptr; }
		inline const char* Ptr() const { return ptr; }
		inline size_t Size() const { return size; }		

		void Resize(size_t newSize);		
		void Clear();						

		String& operator= (const StringView& s);
		String& operator= (const String& s);
		String& operator= (String&& s) noexcept;				

		bool operator==(const StringView& s) const;				
		bool operator!=(const StringView& s) const;		

		char& operator[](size_t index) { return ptr[index]; }
		const char& operator[](size_t index) const { return ptr[index]; }		

		static String Convert(uint32 value);
		static String Convert(int32 value);
		static String Convert(uint64 value);
		static String Convert(int64 value);
		static String Convert(float value);
		static String Convert(double value);		
		template<typename T>
		static String Convert(Vec2<T> vec)
		{
			return "(" + Convert(vec.x) + ", " + Convert(vec.y) + ")";
		}
		template<typename T>
		static String Convert(Vec3<T> vec)
		{
			return "(" + Convert(vec.x) + ", " + Convert(vec.y) + ", " + Convert(vec.z) + ")";
		}
		template<typename T>
		static String Convert(Vec4<T> vec, char seperator = ',')
		{
			return "(" + Convert(vec.x) + ", " + Convert(vec.y) + ", " + Convert(vec.z) + ", " + Convert(vec.w) + ")";
		}

		template<typename T>
		static bool ConvertTo(const StringView&, T&);
		template<> static bool ConvertTo<uint32>(const StringView&, uint32&);
		template<> static bool ConvertTo<int32 >(const StringView&, int32&);
		template<> static bool ConvertTo<uint64>(const StringView&, uint64&);
		template<> static bool ConvertTo<int64 >(const StringView&, int64&);
		template<> static bool ConvertTo<float >(const StringView&, float&);
		template<> static bool ConvertTo<double>(const StringView&, double&);

		friend BLAZE_API String operator+(const StringView& left, const StringView& right);
	}; 

	BLAZE_API String operator+(const StringView& left, const StringView& right);
}