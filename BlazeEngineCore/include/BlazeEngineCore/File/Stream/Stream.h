#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"

namespace Blaze
{
#pragma warning( push )
#pragma warning( disable : 4250)	
	class StreamBase
	{
	public:
		virtual bool MovePosition(intMem offset) = 0;
		virtual bool SetPosition(uintMem offset) = 0;
		virtual bool SetPositionFromEnd(intMem offset) = 0;
		virtual uintMem GetPosition() const = 0;

		virtual uintMem GetSize() const = 0;
	};

	class WriteStream : virtual public StreamBase
	{
	public:
		virtual uintMem Write(const void* ptr, uintMem byteCount) = 0;
	};

	class ReadStream : virtual public StreamBase
	{
	public:
		virtual uintMem Read(void* ptr, uintMem byteCount) = 0;
	};	

	class Stream : public ReadStream, public WriteStream
	{
	public:				
	};
#pragma warning( pop ) 

	template<typename T>
	concept Serializable = requires(const T & a, WriteStream & stream) {
		a.Serialize(stream);
	} || requires(const T & a, WriteStream & stream) {
		stream << a;
	} || std::is_trivially_copy_assignable_v<T>;
	template<typename T>
	concept Deserializable = requires(T & a, ReadStream & stream) {
		a.Deserialize(stream);
	} || requires(T & a, ReadStream & stream) {
		stream >> a;
	} || std::is_trivially_copy_assignable_v<T>;

	template<typename T> requires Serializable<T>
	WriteStream& operator<<(WriteStream& stream, const T& value)
	{
		if constexpr (std::is_trivially_copy_assignable_v<T>)
			stream.Write(&value, sizeof(T));
		else
			value.Serialize(stream);

		return stream;
	}
	template<Deserializable T>
	ReadStream& operator>>(ReadStream& stream, T& value)
	{
		if constexpr (std::is_trivially_copy_assignable_v<T>)
			stream.Read(&value, sizeof(T));
		else
			value.Deserialize(stream);

		return stream;
	}
	template<typename T> requires std::is_trivially_copy_assignable_v<T>
	Stream& operator>>(Stream& stream, T& value)
	{
		stream.Read(&value, sizeof(T));
		return stream;
	}
	template<typename T> requires std::is_trivially_copy_assignable_v<T>
	Stream& operator<<(Stream& stream, T& value)
	{
		stream.Write(&value, sizeof(T));
		return stream;
	}
}