#pragma once
#include "BlazeEngine/Core/Type/TypeTraits.h"

namespace Blaze
{
	template<typename T>
	concept DecimalType = OneOf<T, float, double, long double>;

	template<typename T>
	concept IntegerType = OneOf<T, int8, uint8, int16, uint16, int32, uint32, int64, uint64, long, unsigned long>;

	template<typename T>
	concept UnsignedIntegerType = OneOf<T, uint8, uint16, uint32, uint64>;

	template<typename T>
	concept SignedIntegerType = OneOf<T, int8, int16, int32, int64>;

	template<uintMem byteCount>
	struct UintWithByteSize_s {};
	template<> struct UintWithByteSize_s<1> { using type = uint8; };
	template<> struct UintWithByteSize_s<2> { using type = uint16; };
	template<> struct UintWithByteSize_s<4> { using type = uint32; };
	template<> struct UintWithByteSize_s<8> { using type = uint64; };
	template<uintMem byteCount>
	using UintWithByteSize = typename UintWithByteSize_s<byteCount>::type;

	template<uintMem byteCount>
	struct IntWithByteSize_s {};
	template<> struct IntWithByteSize_s<1> { using type = int8; };
	template<> struct IntWithByteSize_s<2> { using type = int16; };
	template<> struct IntWithByteSize_s<4> { using type = int32; };
	template<> struct IntWithByteSize_s<8> { using type = int64; };
	template<uintMem byteCount>
	using IntWithByteSize = typename IntWithByteSize_s<byteCount>::type;

	template<typename T>
	using MakeUnsigned = UintWithByteSize<sizeof(T)>;
	template<typename T>
	using MakeSigned = IntWithByteSize<sizeof(T)>;

	template<auto MaxValue>
	using FittedUint =
		Conditional<MaxValue <= ((1Ui64 << 8Ui64) - 1Ui64), uint8,
		Conditional<MaxValue <= ((1Ui64 << 16Ui64) - 1Ui64), uint16,
		Conditional<MaxValue <= ((1Ui64 << 32Ui64) - 1Ui64), uint32,
		uint64>>>;
}