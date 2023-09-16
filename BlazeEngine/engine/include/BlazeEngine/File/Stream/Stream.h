#pragma once

namespace Blaze
{
	class StreamBase
	{
	public:
		virtual bool MovePosition(int64 offset) = 0;
		virtual bool SetPosition(uint64 offset) = 0;
		virtual bool SetPositionFromEnd(int64 offset) = 0;
		virtual uint64 GetPosition() const = 0;

		virtual uint64 GetSize() const = 0;
	};

	class WriteStream : virtual public StreamBase
	{
	public:
		virtual uint64 Write(const void* ptr, uint64 byteCount) = 0;
	};

	class ReadStream : virtual public StreamBase
	{
	public:
		virtual uint64 Read(void* ptr, uint64 byteCount) = 0;
	};	

	class Stream : public ReadStream, public WriteStream
	{
	public:				
	};

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