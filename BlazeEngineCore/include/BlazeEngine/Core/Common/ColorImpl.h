
namespace Blaze
{
	constexpr ColorRGB::ColorRGB()
		: r(255), g(255), b(255) 
	{
	}
	constexpr ColorRGB::ColorRGB(uint r, uint g, uint b)
		: r(static_cast<uint8>(r > 255 ? 255 : r)), g(static_cast<uint8>(g > 255 ? 255 : g)), b(static_cast<uint8>(b > 255 ? 255 : b))
	{
	}
	constexpr ColorRGB::ColorRGB(const ColorRGBA& other)
		: ColorRGB(other.r, other.g, other.b)
	{
	}
	constexpr ColorRGB::ColorRGB(const ColorRGBf& other)
		: ColorRGB(static_cast<uint>(other.r * 255), static_cast<uint>(other.g * 255), static_cast<uint>(other.b * 255))
	{
	}
	constexpr ColorRGB::ColorRGB(const ColorRGBAf& other)
		: ColorRGB(static_cast<uint>(other.r * 255), static_cast<uint>(other.g * 255), static_cast<uint>(other.b * 255))
	{
	}

	constexpr ColorRGB::operator Vec3<uint8>() const
	{
		return Vec3<uint8>(r, g, b);
	}	
	
	constexpr ColorRGBA::ColorRGBA()
		: r(255), g(255), b(255), a(255)
	{
	}
	constexpr ColorRGBA::ColorRGBA(uint r, uint g, uint b)
		: r(static_cast<uint8>(r > 255 ? 255 : r)), g(static_cast<uint8>(g > 255 ? 255 : g)), b(static_cast<uint8>(b > 255 ? 255 : b)), a(255)
	{
	}
	constexpr ColorRGBA::ColorRGBA(uint r, uint g, uint b, uint a)
		: r(static_cast<uint8>(r > 255 ? 255 : r)), g(static_cast<uint8>(g > 255 ? 255 : g)), b(static_cast<uint8>(b > 255 ? 255 : b)), a(static_cast<uint8>(a > 255 ? 255 : a))
	{
	}
	constexpr ColorRGBA::ColorRGBA(const ColorRGB& other)
		: r(other.r), g(other.g), b(other.b), a(255)
	{
	}
	constexpr ColorRGBA::ColorRGBA(const ColorRGB& other, uint a)
		: r(other.r), g(other.g), b(other.b), a(static_cast<uint8>(a > 255 ? 255 : a))
	{
	}
	constexpr ColorRGBA::ColorRGBA(const ColorRGBf& other)
		: ColorRGBA(static_cast<uint>(other.r * 255), static_cast<uint>(other.g * 255), static_cast<uint>(other.b * 255), 255)
	{
	}
	constexpr ColorRGBA::ColorRGBA(const ColorRGBf& other, uint a)
		: ColorRGBA(static_cast<uint>(other.r * 255), static_cast<uint>(other.g * 255), static_cast<uint>(other.b * 255), a)
	{
	}
	constexpr ColorRGBA::ColorRGBA(const ColorRGBAf& other)
		: ColorRGBA(static_cast<uint>(other.r * 255), static_cast<uint>(other.g * 255), static_cast<uint>(other.b * 255), static_cast<uint>(other.a * 255))
	{
	}
	
	constexpr ColorRGBA::operator Vec4<uint8>() const
	{
		return Vec4<uint8>(r, g, b, a);
	}

	constexpr ColorRGBf::ColorRGBf()
		: r(0), g(0), b(0)
	{
	}
	constexpr ColorRGBf::ColorRGBf(float r, float g, float b)
		: r(r), g(g), b(b)
	{
	}
	constexpr ColorRGBf::ColorRGBf(const ColorRGB& other)
		: r((float)other.r / 255), g((float)other.g / 255), b((float)other.b / 255)
	{
	}
	constexpr ColorRGBf::ColorRGBf(const ColorRGBA& other)
		: r((float)other.r / 255), g((float)other.g / 255), b((float)other.b / 255)
	{
	}
	constexpr ColorRGBf::ColorRGBf(const ColorRGBAf& other)
		: r(other.r), g(other.g), b(other.b)
	{
	}

	constexpr ColorRGBf::operator Vec3f() const
	{
		return Vec3f(r, g, b);
	}

	constexpr ColorRGBAf::ColorRGBAf()
		: r(1), g(1), b(1), a(1)
	{
	}
	constexpr ColorRGBAf::ColorRGBAf(float r, float g, float b)
		: r(r), g(g), b(b), a(1)
	{
	}
	constexpr ColorRGBAf::ColorRGBAf(float r, float g, float b, float a)
		: r(r), g(g), b(b), a(a)
	{
	}
	constexpr ColorRGBAf::ColorRGBAf(const ColorRGBf& other)
		: r(other.r), g(other.g), b(other.b), a(1)
	{
	}
	constexpr ColorRGBAf::ColorRGBAf(const ColorRGBf& other, float a)
		: r(other.r), g(other.g), b(other.b), a(a)
	{
	}
	constexpr ColorRGBAf::ColorRGBAf(const ColorRGB& other)
		: r((float)other.r / 255), g((float)other.g / 255), b((float)other.b / 255), a(1)
	{
	}
	constexpr ColorRGBAf::ColorRGBAf(const ColorRGB& other, float a)
		: r((float)other.r / 255), g((float)other.g / 255), b((float)other.b / 255), a(a)
	{
	}
	constexpr ColorRGBAf::ColorRGBAf(const ColorRGBA& other)
		: r((float)other.r / 255), g((float)other.g / 255), b((float)other.b / 255), a((float)other.a / 255)
	{
	}

	constexpr ColorRGBAf::operator Vec4f() const
	{
		return Vec4f(r, g, b, a);
	}
	
	constexpr ColorRGB operator ""_rgb(unsigned long long hex)
	{
		return ColorRGB((hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF);
	}
	constexpr ColorRGBA operator ""_rgba(unsigned long long hex)
	{
		return ColorRGBA((hex >> 24) & 0xFF, (hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF);
	}
}
