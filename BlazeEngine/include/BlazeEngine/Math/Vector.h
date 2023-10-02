#pragma once 
#include "BlazeEngine/Math/Math.h"

namespace Blaze
{	
	template<typename T, size_t S>
	struct Vector;

	template<size_t S>
	using Veci = Vector<int, S>;
	template<size_t S>
	using Vecf = Vector<float, S>;
	template<size_t S>
	using Vecd = Vector<double, S>;

	template<typename T>
	struct Vector<T, 2>
	{		
		T x, y;			

		constexpr Vector()
			: x(), y()
		{

		}		
		constexpr Vector(const Vector<T, 2>& v)
			: x(v.x), y(v.y)
		{
		}
		constexpr Vector(T x, T y)
			: x(x), y(y)
		{
		}
		constexpr explicit Vector(T v) 
			: x(v), y(v) 
		{ 
		}		
		template<typename T2>
		constexpr explicit Vector(Vector<T2, 2> v)
			: x(static_cast<T>(v.x)), y(static_cast<T>(v.y))
		{			
		} 

		constexpr Vector operator-() const { return Vector(-x, -y); }

		constexpr Vector operator+(const Vector<T, 2>& b) const { return Vector(x + b.x, y + b.y); }
		constexpr Vector operator-(const Vector<T, 2>& b) const { return Vector(x - b.x, y - b.y); }
		constexpr Vector operator*(const Vector<T, 2>& v) const { return Vector(x * v.x, y * v.y); }
		constexpr Vector operator/(const Vector<T, 2>& v) const { return Vector(x / v.x, y / v.y); }
		constexpr Vector operator*(const T& v) const { return Vector(x * v, y * v); }
		constexpr Vector operator/(const T& v) const { return Vector(x / v, y / v); }
		 
		constexpr Vector& operator+= (const Vector<T, 2>& v) { x += v.x; y += v.y; return *this; }
		constexpr Vector& operator-= (const Vector<T, 2>& v) { x -= v.x; y -= v.y; return *this; }
		constexpr Vector& operator*= (const Vector<T, 2>& v) { x *= v.x; y *= v.y; return *this; }
		constexpr Vector& operator/= (const Vector<T, 2>& v) { x /= v.x; y /= v.y; return *this; }
		constexpr Vector& operator*= (const T& v) { x *= v; y *= v; return *this; }
		constexpr Vector& operator/= (const T& v) { x /= v; y /= v; return *this; }

		constexpr bool operator== (const Vector<T, 2>& v) const { return x == v.x && y == v.y; }
		constexpr bool operator!= (const Vector<T, 2>& v) const { return x != v.x || y != v.y; }		

		constexpr T& operator[](uint index)
		{
			switch (index)
			{
			case 0: return x;
			case 1: return y;			
			default: throw; return x;
			}
		}
		constexpr const T& operator[](uint index) const
		{
			switch (index)
			{
			case 0: return x;
			case 1: return y;
			default: throw; return x;
			}
		}

		constexpr T Lenght() const { return Math::Sqrt(x * x + y * y); }
		constexpr T SqrLenght() const { return x * x + y * y; }
		constexpr void Normalise()
		{
			const T lenght = Lenght();
			x /= lenght;
			y /= lenght;
		}
		constexpr Vector Normalised() const 
		{
			const T lenght = Lenght();
			return Vector(x / lenght, y / lenght);
		}
		constexpr T DotProduct(const Vector& other) const
		{
			return x * other.x + y * other.y;
		}

		static constexpr T DotProduct(const Vector& a, const Vector& b)
		{
			return a.DotProduct(b);
		}
	};

	template<typename T> 
	using Vec2 = Vector<T, 2>;
	using Vec2i = Vec2<int>;
	using Vec2u = Vec2<unsigned>;
	using Vec2f = Vec2<float>;
	using Vec2d = Vec2<double>;

	template<typename T>
	struct Vector<T, 3>
	{		
		T x, y, z;

		constexpr Vector()		
			: x(), y(), z()
		{
		}

		constexpr Vector(const Vector<T, 3>& v)
			: x(v.x), y(v.y), z(v.z)
		{
		}
		constexpr Vector(T x, T y, T z)
			: x(x), y(y), z(z)
		{
		}
		constexpr explicit Vector(T v) 
			: x(v), y(v), z(v)
		{ 
		}
		constexpr Vector(Vector<T, 2> v, T z)
			: x(v.x), y(v.y), z(z)
		{
		}
		template<typename T2>
		constexpr explicit Vector(Vector<T2, 3> v)
			: x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(static_cast<T>(v.z))
		{
		}

		constexpr Vector operator-() const { return Vector(-x, -y, -z); }

		constexpr Vector operator+(const Vector<T, 3>& b) const { return Vector(x + b.x, y + b.y, z + b.z); }
		constexpr Vector operator-(const Vector<T, 3>& b) const { return Vector(x - b.x, y - b.y, z - b.z); }
		constexpr Vector operator*(const Vector<T, 3>& v) const { return Vector(x * v.x, y * v.y, z * v.z); }
		constexpr Vector operator/(const Vector<T, 3>& v) const { return Vector(x / v.x, y / v.y, z / v.z); }
		constexpr Vector operator*(const T& v) const { return Vector(x * v, y * v, z * v); }
		constexpr Vector operator/(const T& v) const { return Vector(x / v, y / v, z / v); }

		constexpr Vector& operator+= (const Vector<T, 3>& v) { x += v.x; y += v.y; z += v.z; return *this; }
		constexpr Vector& operator-= (const Vector<T, 3>& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
		constexpr Vector& operator*= (const Vector<T, 3>& v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
		constexpr Vector& operator/= (const Vector<T, 3>& v) { x /= v.x; y /= v.y; z /= v.z; return *this; }
		constexpr Vector& operator*= (const T& v) { x *= v; y *= v; z *= v; return *this; }
		constexpr Vector& operator/= (const T& v) { x /= v; y /= v; z /= v; return *this; }

		constexpr bool operator== (const Vector<T, 3>& v) const { return x == v.x && y == v.y && z == v.z; }
		constexpr bool operator!= (const Vector<T, 3>& v) const { return x != v.x || y != v.y || z != v.z; }
				
		constexpr Vector& operator=(const Vector<T, 3>& v) { x = v.x; y = v.y; z = v.z; return *this; }

		constexpr T& operator[](uint index)
		{
			switch (index)
			{
			case 0: return x;
			case 1: return y;
			case 2: return z;			
			default: throw; return x;
			}
		}
		constexpr const T& operator[](uint index) const
		{
			switch (index)
			{
			case 0: return x;
			case 1: return y;
			case 2: return z;
			default: throw; return x;
			}
		}

		constexpr T Lenght() const { return Math::Sqrt(x * x + y * y + z * z); }
		constexpr T SqrLenght() const { return x * x + y * y + z * z; }
		constexpr void Normalise()
		{
			const T lenght = Lenght();
			x /= lenght;
			y /= lenght;
			z /= lenght;
		}
		constexpr Vector Normalised() const
		{
			const T lenght = Lenght();
			return Vector(x / lenght, y / lenght, z / lenght);
		}
		constexpr T DotProduct(const Vector& other) const
		{
			return x * other.x + y * other.y + z * other.z;
		}
		constexpr Vector CrossProduct(const Vector& other) const
		{
			return {
				y * other.z - z * other.y,
				z * other.x - x * other.z,
				x * other.y - y * other.x
			};
		}

		static constexpr T DotProduct(const Vector& a, const Vector& b)
		{
			return a.DotProduct(b);
		}		
		static constexpr T CrossProduct(const Vector& a, const Vector& b)
		{
			return a.CrossProduct(b);
		}

		constexpr Vec2<T> xy() const { return Vec2<T>(x, y); }
		constexpr Vec2<T> xz() const { return Vec2<T>(x, z); }
		constexpr Vec2<T> yz() const { return Vec2<T>(y, z); }
	};

	template<typename T>
	using Vec3 = Vector<T, 3>;
	using Vec3i = Vec3<int>;
	using Vec3u = Vec3<unsigned>;
	using Vec3f = Vec3<float>;
	using Vec3d = Vec3<double>;

	template<typename T>
	struct Vector<T, 4>
	{		
		T x, y, z, w;			

		constexpr Vector()
			: x(), y(), z(), w()
		{
		}		
		constexpr Vector(const Vector<T, 4>& v)
			: x(v.x), y(v.y), z(v.z), w(v.w)
		{
		}
		constexpr Vector(T x, T y, T z, T w)
			: x(x), y(y), z(z), w(w)
		{
		}
		constexpr explicit Vector(T v) 
			: x(v), y(v), z(v), w(v) 
		{ 
		}		
		template<typename T2>
		constexpr explicit Vector(Vector<T2, 4> v)
			: x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(static_cast<T>(v.y)), w(static_cast<T>(v.w))
		{			
		}	
		constexpr Vector(Vector<T, 3> v, T w)
			: x(v.x), y(v.y), z(v.z), w(w)
		{
		}
		constexpr Vector(Vector<T, 2> v, T z, T w)
			: x(v.x), y(v.y), z(z), w(w)
		{
		}		
		constexpr Vector(Vector<T, 2> v, Vector<T, 2> v2)
			: x(v.x), y(v.y), z(v2.x), w(v2.y)
		{
		}

		constexpr Vector operator-() const { return Vector(-x, -y, -z, -w); }

		constexpr Vector operator+(const Vector<T, 4>& b) const { return Vector(x + b.x, y + b.y, z + b.z, w + b.w); }
		constexpr Vector operator-(const Vector<T, 4>& b) const { return Vector(x - b.x, y - b.y, z - b.z, w - b.w); }
		constexpr Vector operator*(const Vector<T, 4>& v) const { return Vector(x * v.x, y * v.y, z * v.z, w * v.w); }
		constexpr Vector operator/(const Vector<T, 4>& v) const { return Vector(x / v.x, y / v.y, z / v.z, w / v.w); }
		constexpr Vector operator*(const T& v) const { return Vector(x * v, y * v, z * v, w * v); }
		constexpr Vector operator/(const T& v) const { return Vector(x / v, y / v, z / v, w / v); }

		constexpr Vector& operator+= (const Vector<T, 4>& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
		constexpr Vector& operator-= (const Vector<T, 4>& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
		constexpr Vector& operator*= (const Vector<T, 4>& v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; return *this; }
		constexpr Vector& operator/= (const Vector<T, 4>& v) { x /= v.x; y /= v.y; z /= v.z; w /= v.w; return *this; }
		constexpr Vector& operator*= (const T& v) { x *= v; y *= v; z *= v; w *= v; return *this; }
		constexpr Vector& operator/= (const T& v) { x /= v; y /= v; z /= v; w /= v; return *this; }

		constexpr bool operator== (const Vector<T, 4>& v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
		constexpr bool operator!= (const Vector<T, 4>& v) const { return x != v.x || y != v.y || z != v.z || w != v.w; }
		
		constexpr Vector<T, 4>& operator=(const Vector<T, 4>& v) { x = v.x; y = v.y; z = v.z; w = v.w; return *this; }

		constexpr T& operator[](uint index)
		{
			switch (index)
			{
			case 0: return x;
			case 1: return y;
			case 2: return z;
			case 3: return w;
			default: throw; return x;
			}
		}
		constexpr const T& operator[](uint index) const
		{
			switch (index)
			{
			case 0: return x;
			case 1: return y;
			case 2: return z;
			case 3: return w;
			default: throw; return x;
			}
		}

		constexpr T Lenght() const { return Math::Sqrt<T>(x * x + y * y + z * z + w * w); }
		constexpr T SqrLenght() const { return x * x + y * y + z * z + w * w; }
		constexpr void Normalise()
		{
			const T lenght = Lenght();
			x /= lenght;
			y /= lenght;
			z /= lenght;
			w /= lenght;
		}
		constexpr Vector Normalised() const 
		{
			const T lenght = Lenght();
			return Vector(x / lenght, y / lenght, z / lenght, w / lenght);
		}
		constexpr T DotProduct(const Vector& other) const 
		{
			return x * other.x + y * other.y + z * other.z + w * other.w;
		}

		static constexpr T DotProduct(const Vector& a, const Vector& b)
		{
			return a.DotProduct(b);
		}

		constexpr Vec2<T> xy() const { return Vec2<T>(x, y); }
		constexpr Vec2<T> xz() const { return Vec2<T>(x, z); }
		constexpr Vec2<T> xw() const { return Vec2<T>(x, w); }
		constexpr Vec2<T> yz() const { return Vec2<T>(y, z); }
		constexpr Vec2<T> yw() const { return Vec2<T>(y, w); }
		constexpr Vec2<T> zw() const { return Vec2<T>(z, w); }

		constexpr Vec3<T> xyz() const { return Vec3<T>(x, y, z); }
		constexpr Vec3<T> xzw() const { return Vec3<T>(x, z, w); }
		constexpr Vec3<T> yzw() const { return Vec3<T>(y, z, w); }
	};

	template<typename T>
	using Vec4 = Vector<T, 4>;
	using Vec4i = Vec4<int>;
	using Vec4u = Vec4<unsigned>;
	using Vec4f = Vec4<float>;
	using Vec4d = Vec4<double>;
}