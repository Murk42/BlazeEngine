#include "BlazeEngine/DataStructures/Color.h"

namespace Blaze
{
	ColorRGB::ColorRGB(const ColorRGBf& other)
		: r(static_cast<uint8>(other.r * 255)), g(static_cast<uint8>(other.g * 255)), b(static_cast<uint8>(other.b * 255))
	{
	}
	ColorRGB::ColorRGB(const ColorRGBA& other)
		: r(other.r), g(other.g), b(other.b)
	{
	}
	ColorRGB::ColorRGB(const ColorRGBAf& other)
		: r(static_cast<uint8>(other.r * 255)), g(static_cast<uint8>(other.g * 255)), b(static_cast<uint8>(other.b * 255))
	{
	}

	ColorRGB::ColorRGB(uint32 hex)
		: r((hex >> 16) & 255), g((hex >> 8) & 255), b((hex >> 0) & 255)
	{
	}

	ColorRGB::operator Vec3<uint8>() const
	{
		return Vec3<uint8>(r, g, b);
	}	
	
	ColorRGBA::ColorRGBA()
		: r(255), g(255), b(255), a(255)
	{
	}
	ColorRGBA::ColorRGBA(uint8 r, uint8 g, uint8 b)
		: r(r), g(g), b(b), a(255)
	{
	}
	ColorRGBA::ColorRGBA(uint8 r, uint8 g, uint8 b, uint8 a)
		: r(r), g(g), b(b), a(a)
	{
	}
	ColorRGBA::ColorRGBA(const ColorRGB& c)
		: r(c.r), g(c.g), b(c.b), a(255)
	{
	}
	ColorRGBA::ColorRGBA(const ColorRGB& c, uint8 a)
		: r(c.r), g(c.g), b(c.b), a(a)
	{
	}
	ColorRGBA::ColorRGBA(const ColorRGBf& c)
		: r(static_cast<uint8>(c.r * 255)), g(static_cast<uint8>(c.g * 255)), b(static_cast<uint8>(c.b * 255)), a(255)
	{
	}
	ColorRGBA::ColorRGBA(const ColorRGBf& c, uint8 a)
		: r(static_cast<uint8>(c.r * 255)), g(static_cast<uint8>(c.g * 255)), b(static_cast<uint8>(c.b * 255)), a(a)
	{
	}
	ColorRGBA::ColorRGBA(const ColorRGBAf& c)
		: r(static_cast<uint8>(c.r * 255)), g(static_cast<uint8>(c.g * 255)), b(static_cast<uint8>(c.b * 255)), a(static_cast<uint8>(c.a * 255))
	{
	}
	ColorRGBA::ColorRGBA(uint32 hex)
		: r((hex >> 24) & 255), g((hex >> 16) & 255), b((hex >> 8) & 255), a((hex >> 0) & 255)
	{
	}
	
	ColorRGBA::operator Vec4<uint8>() const
	{
		return Vec4<uint8>(r, g, b, a);
	}

	ColorRGBf::ColorRGBf()
		: r(0), g(0), b(0)
	{
	}
	ColorRGBf::ColorRGBf(float r, float g, float b)
		: r(r), g(g), b(b)
	{
	}
	ColorRGBf::ColorRGBf(const ColorRGB& c)
		: r((float)c.r / 255), g((float)c.g / 255), b((float)c.b / 255)
	{
	}
	ColorRGBf::ColorRGBf(const ColorRGBA& c)
		: r((float)c.r / 255), g((float)c.g / 255), b((float)c.b / 255)
	{
	}
	ColorRGBf::ColorRGBf(const ColorRGBAf& c)
		: r(c.r), g(c.g), b(c.b)
	{
	}

	ColorRGBf::ColorRGBf(uint32 hex)
		: r((float)((hex >> 16) & 255) / 255), g((float)((hex >> 8) & 255) / 255), b((float)((hex >> 0) & 255) / 255)
	{
	}

	ColorRGBf::operator Vec3f() const
	{
		return Vec3f(r, g, b);
	}

	ColorRGBAf::ColorRGBAf()
		: r(1), g(1), b(1), a(1)
	{
	}
	ColorRGBAf::ColorRGBAf(float r, float g, float b)
		: r(r), g(g), b(b), a(1)
	{
	}
	ColorRGBAf::ColorRGBAf(float r, float g, float b, float a)
		: r(r), g(g), b(b), a(a)
	{
	}
	ColorRGBAf::ColorRGBAf(const ColorRGBf& c)
		: r(c.r), g(c.g), b(c.b), a(1)
	{
	}
	ColorRGBAf::ColorRGBAf(const ColorRGBf& c, float a)
		: r(c.r), g(c.g), b(c.b), a(a)
	{
	}
	ColorRGBAf::ColorRGBAf(const ColorRGB& c)
		: r((float)c.r / 255), g((float)c.g / 255), b((float)c.b / 255), a(1)
	{
	}
	ColorRGBAf::ColorRGBAf(const ColorRGB& c, float a)
		: r((float)c.r / 255), g((float)c.g / 255), b((float)c.b / 255), a(a)
	{
	}
	ColorRGBAf::ColorRGBAf(const ColorRGBA& c)
		: r((float)c.r / 255), g((float)c.g / 255), b((float)c.b / 255), a((float)c.a / 255)
	{
	}

	ColorRGBAf::ColorRGBAf(uint hex)
		: r((float)((hex >> 24) & 255) / 255), g((float)((hex >> 16) & 255) / 255), b((float)((hex >> 8) & 255) / 255), a((float)((hex >> 0) & 255) / 255)
	{
	}

	ColorRGBAf::operator Vec4f() const
	{
		return Vec4f(r, g, b, a);
	}
}
