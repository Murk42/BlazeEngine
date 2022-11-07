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

		template<typename T>
		void Write(const T&);

		template<> void Write<bool>(const bool&);
		template<> void Write<int8 >(const int8&);
		template<> void Write<int16>(const int16&);
		template<> void Write<int32>(const int32&);
		template<> void Write<int64>(const int64&);
		template<> void Write<uint8 >(const uint8&);
		template<> void Write<uint16>(const uint16&);
		template<> void Write<uint32>(const uint32&);
		template<> void Write<uint64>(const uint64&);
		template<> void Write<float>(const float&);
		template<> void Write<double>(const double&);		

		template<typename T, size_t S>
		void WriteVector(const Vector<T, S>& value)
		{
			for (size_t i = 0; i < S; ++i)
				Write<T>(value.arr[i]);
		}

		void WriteString(StringView string);
	};
}