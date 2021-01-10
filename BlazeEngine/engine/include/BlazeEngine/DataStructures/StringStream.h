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

		inline ByteStream* GetByteStream() const { return (ByteStream*)this; }

		inline String GetAsString() const;
		inline StringView GetAsStringView() const;

		inline StringStream& Get(void* ptr, uint size) { ByteStream::Get(ptr, size); return *this; }
		inline StringStream& Set(const void* ptr, uint size) { ByteStream::Set(ptr, size); return *this; }

		template<typename T>
		StringStream& GetLine(T&);
		template<> StringStream& GetLine<String>(String& value);
		template<> StringStream& GetLine<StringView>(StringView& value);

		StringStream& SkipUntil(char end);
				
		StringView GetUntil(char end);

		StringStream& GetUntil(String& s, char end);
		StringStream& GetUntil(StringView& s, char end);

		template<typename T>
		StringStream& Get(T&);
		template<typename T>
		StringStream& Set(const T&);

		template<typename T>
		inline T Get()
		{
			T value;
			Get<T>(value);
			return value;
		}


		template<> StringStream& Get<char>(char& value);
		template<> StringStream& Set<char>(const char& value);
		template<> StringStream& Get<String>(String& value);
		template<> StringStream& Set<String>(const String& value);
		template<> StringStream& Get<StringView>(StringView& value);
		template<> StringStream& Set<StringView>(const StringView& value);

		template<> StringStream& Get<uint8>(uint8& value);
		template<> StringStream& Set<uint8>(const uint8& value);
		template<> StringStream& Get<uint16>(uint16& value);
		template<> StringStream& Set<uint16>(const uint16& value);
		template<> StringStream& Get<uint32>(uint32& value);
		template<> StringStream& Set<uint32>(const uint32& value);
		template<> StringStream& Get<uint64>(uint64& value);
		template<> StringStream& Set<uint64>(const uint64& value);

		template<> StringStream& Get<int8>(int8& value);
		template<> StringStream& Set<int8>(const int8& value);
		template<> StringStream& Get<int16>(int16& value);
		template<> StringStream& Set<int16>(const int16& value);
		template<> StringStream& Get<int32>(int32& value);
		template<> StringStream& Set<int32>(const int32& value);
		template<> StringStream& Get<int64>(int64& value);
		template<> StringStream& Set<int64>(const int64& value);
		
		template<> StringStream& Get<float>(float& value);
		template<> StringStream& Set<float>(const float& value);
		template<> StringStream& Get<double>(double& value);
		template<> StringStream& Set<double>(const double& value);
		template<> StringStream& Get<long double>(long double& value);
		template<> StringStream& Set<long double>(const long double& value);

		void operator=(const StringStream&);
		void operator=(StringStream&&) noexcept;

		using ByteStream::GetOffset;		
		using ByteStream::GetSize;
		using ByteStream::GetData;
		using ByteStream::IsEnd;
		using ByteStream::Resize;
		using ByteStream::Skip;
	};
}