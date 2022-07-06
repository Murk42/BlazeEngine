#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/DataStructures/UnicodeChar.h"

namespace Blaze
{	
	class StringViewUTF8;		
	class StringView;	
	
	class BLAZE_API StringUTF8
	{
		void* buffer;	
		size_t bufferSize;
		size_t characterCount;		
	public:
		class BLAZE_API Iterator
		{
			void* ptr;
			Iterator(void*);
		public:
			Iterator();
			Iterator(const Iterator&);

			UnicodeChar ToUnicode() const;

			Iterator operator++();
			Iterator operator--();
			Iterator& operator++(int);
			Iterator& operator--(int);

			bool operator<(const Iterator&) const;
			bool operator>(const Iterator&) const;
			bool operator<=(const Iterator&) const;
			bool operator>=(const Iterator&) const;
			bool operator==(const Iterator&) const;
			bool operator!=(const Iterator&) const;

			Iterator& operator=(const Iterator&);

			friend class StringUTF8;
		};

		StringUTF8();		
		StringUTF8(const StringUTF8& s);
		StringUTF8(StringUTF8&& s) noexcept;
				
		StringUTF8(void* buffer, size_t bufferSize);
		StringUTF8(const char* ptr);
		StringUTF8(const char* ptr, size_t size);
		StringUTF8(const StringViewUTF8&);
		StringUTF8(const StringView&);		

		~StringUTF8();			
		
		void Clear();
		
		const void* Buffer() const { return buffer; }
		size_t BufferSize() const { return bufferSize; }
		size_t CharacterCount() const { return characterCount; }
		
		Iterator begin() const;		
		Iterator end() const;

		StringUTF8& Resize(size_t newCharacterCount, UnicodeChar fill);

		StringUTF8& operator= (const StringViewUTF8& s);		
		StringUTF8& operator= (StringUTF8&& s) noexcept;

		bool operator==(const StringViewUTF8& s) const;
		bool operator!=(const StringViewUTF8& s) const;		

		static StringUTF8 Convert(uint32 value);
		static StringUTF8 Convert(int32 value);
		static StringUTF8 Convert(uint64 value);
		static StringUTF8 Convert(int64 value);
		static StringUTF8 Convert(float value);
		static StringUTF8 Convert(double value);		
		template<typename T>
		static StringUTF8 Convert(Vec2<T> vec)
		{ 
			return "(" + Convert(vec.x) + ", " + Convert(vec.y) + ")";
		}
		template<typename T>
		static StringUTF8 Convert(Vec3<T> vec)
		{
			return "(" + Convert(vec.x) + ", " + Convert(vec.y) + ", " + Convert(vec.z) + ")";
		}
		template<typename T>
		static StringUTF8 Convert(Vec4<T> vec, char seperator = ',')
		{
			return "(" + Convert(vec.x) + ", " + Convert(vec.y) + ", " + Convert(vec.z) + ", " + Convert(vec.w) + ")";
		}

		template<typename T>
		static bool ConvertTo(const StringViewUTF8&, T&);
		template<> static bool ConvertTo<uint32>(const StringViewUTF8&, uint32&);
		template<> static bool ConvertTo<int32 >(const StringViewUTF8&, int32&);
		template<> static bool ConvertTo<uint64>(const StringViewUTF8&, uint64&);
		template<> static bool ConvertTo<int64 >(const StringViewUTF8&, int64&);
		template<> static bool ConvertTo<float >(const StringViewUTF8&, float&);
		template<> static bool ConvertTo<double>(const StringViewUTF8&, double&);

		friend BLAZE_API StringUTF8 operator+(const StringViewUTF8& left, const StringViewUTF8& right);				
	};
	

	BLAZE_API StringUTF8 operator+(const StringViewUTF8& left, const StringViewUTF8& right);
}