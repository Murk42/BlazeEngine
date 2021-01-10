#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "Vector.h"
#include "Matrix.h"

namespace Blaze
{ 	
	enum class BLAZE_API Type
	{
		Vec2b = 0x8B57,
		Vec2u = 0x8DC6,
		Vec2i = 0x8B53,
		Vec2f = 0x8B50,
		Vec2d = 0x8FFC,

		Vec3b = 0x8B58,
		Vec3u = 0x8DC7,
		Vec3i = 0x8B54,
		Vec3f = 0x8B51,
		Vec3d = 0x8FFD,

		Vec4b = 0x8B59,
		Vec4u = 0x8DC8,
		Vec4i = 0x8B55,
		Vec4f = 0x8B52,
		Vec4d = 0x8FFE,

		Mat2f = 0x8B5A,
		Mat2d = 0x8F46,

		Mat3f = 0x8B5B,
		Mat3d = 0x8F47,

		Mat4f = 0x8B5C,
		Mat4d = 0x8F48,

		Bool = 0x8B56,
		Byte = 0x1400,
		UnsignedByte = 0x1401,
		Short = 0x1402,
		UnsignedShort = 0x1403,
		Int = 0x1404,
		UnsignedInt = 0x1405,
		Float = 0x1406,
		Double = 0x140A,
	};

	constexpr size_t SizeOf(const Type& type)
	{
		switch (type)
		{
		case Type::Vec2b: return 2 * sizeof(bool); break;
		case Type::Vec2u: return 2 * sizeof(unsigned); break;
		case Type::Vec2i: return 2 * sizeof(int); break;
		case Type::Vec2f: return 2 * sizeof(float); break;
		case Type::Vec2d: return 2 * sizeof(double); break;

		case Type::Vec3b: return 3 * sizeof(bool); break;
		case Type::Vec3u: return 3 * sizeof(unsigned); break;
		case Type::Vec3i: return 3 * sizeof(int); break;
		case Type::Vec3f: return 3 * sizeof(float); break;
		case Type::Vec3d: return 3 * sizeof(double); break;

		case Type::Vec4b: return 4 * sizeof(bool); break;
		case Type::Vec4u: return 4 * sizeof(unsigned); break;
		case Type::Vec4i: return 4 * sizeof(int); break;
		case Type::Vec4f: return 4 * sizeof(float); break;
		case Type::Vec4d: return 4 * sizeof(double); break;

		case Type::Mat2f: return 2 * 2 * sizeof(float); break;
		case Type::Mat2d: return 2 * 2 * sizeof(double); break;

		case Type::Mat3f: return 3 * 3 * sizeof(float); break;
		case Type::Mat3d: return 3 * 3 * sizeof(double); break;

		case Type::Mat4f: return 4 * 4 * sizeof(float); break;
		case Type::Mat4d: return 4 * 4 * sizeof(double); break;

		case Type::Bool: return sizeof(bool); break;
		case Type::Byte: return sizeof(byte);  break;
		case Type::UnsignedByte: return sizeof(ubyte);  break;
		case Type::Short: return sizeof(short);  break;
		case Type::UnsignedShort: return sizeof(unsigned short);  break;
		case Type::Int: return sizeof(int);  break;
		case Type::UnsignedInt: return sizeof(unsigned int);  break;
		case Type::Float: return sizeof(float);  break;
		case Type::Double: return sizeof(double);  break;
		}
		return 0;
	}

	template<typename T>
	inline constexpr Type BLAZE_API GetType();

	BLAZE_API Type GetType(const String& s);	
	template<> inline constexpr Type GetType<Vec2i>() { return Type::Vec2i; }
	template<> inline constexpr Type GetType<Vec2f>() { return Type::Vec2f; }
	template<> inline constexpr Type GetType<Vec2d>() { return Type::Vec2d; }
	
	template<> inline constexpr Type GetType<Vec3i>() { return Type::Vec3i; }
	template<> inline constexpr Type GetType<Vec3f>() { return Type::Vec3f; }
	template<> inline constexpr Type GetType<Vec3d>() { return Type::Vec3d; }
	
	template<> inline constexpr Type GetType<Vec4i>() { return Type::Vec4i; }
	template<> inline constexpr Type GetType<Vec4f>() { return Type::Vec4f; }
	template<> inline constexpr Type GetType<Vec4d>() { return Type::Vec4d; }
	
	template<> inline constexpr Type GetType<Mat2f>() { return Type::Mat2f; }
	template<> inline constexpr Type GetType<Mat2d>() { return Type::Mat2d; }
			 
	template<> inline constexpr Type GetType<Mat3f>() { return Type::Mat3f; }
	template<> inline constexpr Type GetType<Mat3d>() { return Type::Mat3d; }
	
	template<> inline constexpr Type GetType<Mat4f>() { return Type::Mat4f; }
	template<> inline constexpr Type GetType<Mat4d>() { return Type::Mat4d; }
	
	template<> inline constexpr Type GetType<bool>() { return Type::Bool; }
	template<> inline constexpr Type GetType<byte>() { return Type::Byte; }
	template<> inline constexpr Type GetType<ubyte>() { return Type::UnsignedByte; }
	template<> inline constexpr Type GetType<short>() { return Type::Short; }
	template<> inline constexpr Type GetType<unsigned short>() { return Type::UnsignedShort; }
	template<> inline constexpr Type GetType<int>() { return Type::Int; }
	template<> inline constexpr Type GetType<unsigned>() { return Type::UnsignedInt; }
	template<> inline constexpr Type GetType<float>() { return Type::Float; }
	template<> inline constexpr Type GetType<double>() { return Type::Float; }

	template<typename T> inline constexpr Type GetType() { return (Type)0; }
}