#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/ByteStream.h"

namespace Blaze
{
	class String;
	class StringView;
	class BLAZE_API StringStream : private ByteStream
	{
		inline void ResizeToFit(size_t size);		
	public:
		StringStream();
		StringStream(const StringStream&);
		StringStream(StringStream&&) noexcept;		
		StringStream(ByteStream&&) noexcept;
		StringStream(const String&);
		StringStream(const StringView&);
		~StringStream();

		inline ByteStream& GetByteStream() const { return (ByteStream&)*this; }

		inline bool IsEndOfLine() const;

		inline String GetAsString() const;
		inline StringView GetAsStringView() const;

		inline StringStream& Get(void* ptr, uint size) { ByteStream::Get(ptr, size); return *this; }
		inline StringStream& Set(const void* ptr, uint size) { ByteStream::Set(ptr, size); return *this; }
		
		/*
		Returns a sub string from the current position to the next "new line" character
		*/
		StringView GetLine();
		StringView GetWord();

		StringStream& SkipUntil(char end);
				
		StringView GetUntil(char end);

		StringStream& GetUntil(String& s, char end);
		StringStream& GetUntil(StringView& s, char end);

		template<typename T>
		inline T Get();
		template<typename T>
		inline void Set(const T&);

		template<> char Get<char>();
		template<> void Set<char>(const char& value);				
		
		template<> uint Get<uint>();
		template<> void Set<uint>(const uint& value);
		template<> int Get<int>();
		template<> void Set<int>(const int& value);		
		
		template<> float Get<float>();
		template<> void Set<float>(const float& value);
		template<> double Get<double>();
		template<> void Set<double>(const double& value);
		template<> long double Get<long double>();
		template<> void Set<long double>(const long double& value);

		void operator=(const StringStream&);
		void operator=(StringStream&&) noexcept;

		using ByteStream::GetOffset;		
		using ByteStream::SetOffset;
		using ByteStream::GetSize;
		using ByteStream::GetData;
		using ByteStream::IsEnd;
		using ByteStream::Resize;
		using ByteStream::Skip;
	};
}