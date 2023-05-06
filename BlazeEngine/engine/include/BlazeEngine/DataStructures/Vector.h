#pragma once 
#include <array>

namespace Blaze
{
	namespace Math
	{
		inline float Sqrt(float value);
		inline double Sqrt(double value);
	}

	template<typename T, size_t S>
	struct BLAZE_API Vector;

	template<size_t S>
	using Veci = Vector<int, S>;
	template<size_t S>
	using Vecf = Vector<float, S>;
	template<size_t S>
	using Vecd = Vector<double, S>;

	template<typename T>
	struct BLAZE_API Vector<T, 2>
	{
		union {
			std::array<T, 2> arr;
			struct {
				T x, y;
			};
		};

		constexpr Vector()
			: x(), y()
		{

		}		
		constexpr Vector(const Vector<T, 2>& v)
			: arr(v.arr)
		{
		}
		constexpr Vector(T x, T y)
			: arr({ x, y })
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

		constexpr void operator+= (const Vector<T, 2>& v) { x += v.x; y += v.y; }
		constexpr void operator-= (const Vector<T, 2>& v) { x -= v.x; y -= v.y; }
		constexpr void operator*= (const Vector<T, 2>& v) { x *= v.x; y *= v.y; }
		constexpr void operator/= (const Vector<T, 2>& v) { x /= v.x; y /= v.y; }
		constexpr void operator*= (const T& v) { x *= v; y *= v; }
		constexpr void operator/= (const T& v) { x /= v; y /= v; }

		constexpr bool operator== (const Vector<T, 2>& v) const { return x == v.x && y == v.y; }
		constexpr bool operator!= (const Vector<T, 2>& v) const { return x != v.x || y != v.y; }
		
		constexpr Vector<T, 2>& operator=(const Vector<T, 2>& v) { x = v.x; y = v.y; return* this; }

		constexpr T Lenght() const { return Math::Sqrt(x * x + y * y); }
		constexpr T SqrLenght() const { return x * x + y * y; }
		constexpr void Normalise()
		{
			const T lenght = Lenght();
			x /= lenght;
			y /= lenght;
		}
		constexpr Vector Normalised()
		{
			const T lenght = Lenght();
			return Vector(x / lenght, y / lenght);
		}
	};

	template<typename T> 
	using Vec2 = Vector<T, 2>;
	using Vec2i = Vec2<int>;
	using Vec2u = Vec2<unsigned>;
	using Vec2f = Vec2<float>;
	using Vec2d = Vec2<double>;

	template<typename T>
	struct BLAZE_API Vector<T, 3>
	{
		union {
			struct {
				T x, y, z;
			};
			struct {
				Vec2<T> xy;
			};
			T arr[3]{ 0, 0, 0 };
		};

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
		template<typename T2>
		constexpr explicit Vector(Vector<T2, 3> v)
			: x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(static_cast<T>(v.z))
		{
		}
		constexpr Vector(Vector<T, 2> v, T z)
			: x(v.x), y(v.y), z(z)
		{
		}

		constexpr Vector operator-() const { return Vector(-x, -y, -z); }

		constexpr Vector operator+(const Vector<T, 3>& b) const { return Vector(x + b.x, y + b.y, z + b.z); }
		constexpr Vector operator-(const Vector<T, 3>& b) const { return Vector(x - b.x, y - b.y, z - b.z); }
		constexpr Vector operator*(const Vector<T, 3>& v) const { return Vector(x * v.x, y * v.y, z * v.z); }
		constexpr Vector operator/(const Vector<T, 3>& v) const { return Vector(x / v.x, y / v.y, z / v.z); }
		constexpr Vector operator*(const T& v) const { return Vector(x * v, y * v, z * v); }
		constexpr Vector operator/(const T& v) const { return Vector(x / v, y / v, z / v); }

		constexpr void operator+= (const Vector<T, 3>& v) { x += v.x; y += v.y; z += v.z; }
		constexpr void operator-= (const Vector<T, 3>& v) { x -= v.x; y -= v.y; z -= v.z; }
		constexpr void operator*= (const Vector<T, 3>& v) { x *= v.x; y *= v.y; z *= v.z; }
		constexpr void operator/= (const Vector<T, 3>& v) { x /= v.x; y /= v.y; z /= v.z; }
		constexpr void operator*= (const T& v) { x *= v; y *= v; z *= v; }
		constexpr void operator/= (const T& v) { x /= v; y /= v; z /= v; }

		constexpr bool operator== (const Vector<T, 3>& v) const { return x == v.x && y == v.y && z == v.z; }
		constexpr bool operator!= (const Vector<T, 3>& v) const { return x != v.x || y != v.y || z != v.z; }
				
		constexpr Vector<T, 3>& operator=(const Vector<T, 3>& v) { x = v.x; y = v.y; z = v.z; return *this; }

		constexpr T Lenght() const { return Math::Sqrt(x * x + y * y + z * z); }
		constexpr T SqrLenght() const { return x * x + y * y + z * z; }
		constexpr void Normalise()
		{
			const T lenght = Lenght();
			x /= lenght;
			y /= lenght;
			z /= lenght;
		}
		constexpr Vector Normalised()
		{
			const T lenght = Lenght();
			return Vector(x / lenght, y / lenght, z / lenght);
		}
	};

	template<typename T>
	using Vec3 = Vector<T, 3>;
	using Vec3i = Vec3<int>;
	using Vec3u = Vec3<unsigned>;
	using Vec3f = Vec3<float>;
	using Vec3d = Vec3<double>;

	template<typename T>
	struct BLAZE_API Vector<T, 4>
	{
		union {
			struct {
				T x, y, z, w;
			};
			struct {
				Vec2<T> xy;
			};
			struct {
				Vec3<T> xyz;
			};
			T arr[4]{ 0, 0, 0, 0 };
		};

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

		constexpr void operator+= (const Vector<T, 4>& v) { x += v.x; y += v.y; z += v.z; w += v.w; }
		constexpr void operator-= (const Vector<T, 4>& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; }
		constexpr void operator*= (const Vector<T, 4>& v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; }
		constexpr void operator/= (const Vector<T, 4>& v) { x /= v.x; y /= v.y; z /= v.z; w /= v.w; }
		constexpr void operator*= (const T& v) { x *= v; y *= v; z *= v; w *= v; }
		constexpr void operator/= (const T& v) { x /= v; y /= v; z /= v; w /= v; }

		constexpr bool operator== (const Vector<T, 4>& v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
		constexpr bool operator!= (const Vector<T, 4>& v) const { return x != v.x || y != v.y || z != v.z || w != v.w; }
		
		constexpr Vector<T, 4>& operator=(const Vector<T, 4>& v) { x = v.x; y = v.y; z = v.z; w = v.w; return *this; }

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
		constexpr Vector Normalised()
		{
			const T lenght = Lenght();
			return Vector(x / lenght, y / lenght, z / lenght, w / lenght);
		}
	};

	template<typename T>
	using Vec4 = Vector<T, 4>;
	using Vec4i = Vec4<int>;
	using Vec4u = Vec4<unsigned>;
	using Vec4f = Vec4<float>;
	using Vec4d = Vec4<double>;
}