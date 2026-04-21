#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include <type_traits>

namespace Blaze
{
#pragma warning( push )
#pragma warning( disable : 4250)
	class BLAZE_API StreamBase
	{
	public:
		virtual ~StreamBase() { }

		virtual bool MovePosition(intMem offset) = 0;
		virtual bool SetPosition(uintMem offset) = 0;
		virtual bool SetPositionFromEnd(intMem offset) = 0;
		virtual uintMem GetPosition() const = 0;

		virtual uintMem GetSize() const = 0;
	};

	class BLAZE_API WriteStream : virtual public StreamBase
	{
	public:
		virtual uintMem Write(const void* ptr, uintMem byteCount) = 0;
	};

	class BLAZE_API ReadStream : virtual public StreamBase
	{
	public:
		virtual uintMem Read(void* ptr, uintMem byteCount) = 0;
	};

	class BLAZE_API Stream : public ReadStream, public WriteStream
	{
	public:
	};
#pragma warning( pop )

	template<typename T>
	concept Serializable = requires(const T & a, WriteStream & stream) {
		a.Serialize(stream);
	} || IsTriviallyCopyable<T>;
	template<typename T>
	concept Deserializable = requires(T & a, ReadStream & stream) {
		a.Deserialize(stream);
	} || IsTriviallyCopyable<T>;

	template<Serializable T>
	WriteStream& operator<<(WriteStream& stream, const T& value)
	{
		if constexpr (IsTriviallyCopyable<T>)
			stream.Write(&value, sizeof(T));
		else
			value.Serialize(stream);

		return stream;
	}
	template<Deserializable T>
	ReadStream& operator>>(ReadStream& stream, T& value)
	{
		if constexpr (IsTriviallyCopyable<T>)
			stream.Read(&value, sizeof(T));
		else
			value.Deserialize(stream);

		return stream;
	}
	template<Serializable T>
	Stream& operator>>(Stream& stream, T& value)
	{
		((ReadStream&)stream) >> value;

		return stream;
	}
	template<Deserializable T>
	Stream& operator<<(Stream& stream, T& value)
	{
		((WriteStream&)stream) << value;
		return stream;
	}
}