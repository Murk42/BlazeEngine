#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/Buffer.h"
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/DataStructures/String.h"

namespace Blaze
{
	class BLAZE_API BinaryInputStream
	{
		size_t offset;
		Buffer buffer;
	public:
		BinaryInputStream();
		
		void SetOffset(size_t offset);
		void TakeBuffer(Buffer& buffer);
		void ReturnBuffer(Buffer& buffer);

		template<typename T>
		void Read(T&);

		template<> void Read<bool >(bool&);
		template<> void Read<int8 >(int8& );
		template<> void Read<int16>(int16&);
		template<> void Read<int32>(int32&);
		template<> void Read<int64>(int64&);
		template<> void Read<uint8 >(uint8& );
		template<> void Read<uint16>(uint16&);
		template<> void Read<uint32>(uint32&);
		template<> void Read<uint64>(uint64&);
		template<> void Read<float>(float&);
		template<> void Read<double>(double&);

		template<typename T, size_t S>
		void ReadVector(Vector<T, S>& value)
		{
			for (size_t i = 0; i < S; ++i)
				Read<T>(value.arr[i]);
		}

		void ReadString(String& string);
	};
}