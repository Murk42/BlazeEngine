#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/Buffer.h"
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/DataStructures/StringView.h"

namespace Blaze
{
	class BLAZE_API BinaryOutputStream
	{
		size_t offset;
		Buffer buffer;
	public:
		BinaryOutputStream();
				
		void SetOffset(size_t offset);
		const Buffer& GetBuffer();

		void Write(const bool&);
		void Write(const int8&);
		void Write(const int16&);
		void Write(const int32&);
		void Write(const int64&);
		void Write(const uint8&);
		void Write(const uint16&);
		void Write(const uint32&);
		void Write(const uint64&);
		void Write(const float&);
		void Write(const double&);

		template<typename T, size_t S>
		void WriteVector(const Vector<T, S>& value)
		{
			for (size_t i = 0; i < S; ++i)
				Write(value.arr[i]);
		}

		void WriteString(StringView string);
	};
}