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

		void Read(bool&);
		void Read(int8& );
		void Read(int16&);
		void Read(int32&);
		void Read(int64&);
		void Read(uint8& );
		void Read(uint16&);
		void Read(uint32&);
		void Read(uint64&);
		void Read(float&);
		void Read(double&);

		template<typename T, size_t S>
		void ReadVector(Vector<T, S>& value)
		{
			for (size_t i = 0; i < S; ++i)
				Read(value.arr[i]);
		}

		void ReadString(String& string);
	};
}