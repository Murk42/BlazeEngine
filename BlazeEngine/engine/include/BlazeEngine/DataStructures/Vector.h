#pragma once 

namespace Blaze
{
	namespace Math
	{
		template<typename T>
		constexpr T Sqrt(const T&);
	}

	template<typename T, size_t S>
	struct Vector
	{
		T arr[S]{};

		constexpr Vector() = default;
		template<typename T2, size_t S2>
		constexpr Vector(const Vector<T2, S2>& v)
		{
			for (size_t i = 0; i != (S2 < S ? S2 : S); ++i)
				arr[i] = v.arr[i];
			for (size_t i = (S2 < S ? S2 : S); i < S; ++i)
				arr[i] = T(0);
		}
		template<typename ... Ts>
		constexpr Vector(Ts ... values)
			: arr{ T(values)... }
		{
		}
		template<typename T2>
		constexpr explicit Vector(const T2& v)
		{
			for (size_t i = 0; i != S; ++i)
				arr = v;
		}
		constexpr Vector(T* ptr)
		{
			for (size_t i = 0; i != S; ++i)
				arr = ptr[i];
		}

		constexpr Vector operator-() const
		{
			Vector vec; for (int i = 0; i < S; ++i) vec.arr[i] = -arr[i]; return vec;
		}
		
		template<typename T2> constexpr Vector operator+(const Vector<T2, S>& vec) const { Vector out; for (int i = 0; i < S; ++i) out.arr[i] = arr[i] + vec.arr[i]; return out; }
		template<typename T2> constexpr Vector operator-(const Vector<T2, S>& vec) const { Vector out; for (int i = 0; i < S; ++i) out.arr[i] = arr[i] - vec.arr[i]; return out; }
		template<typename T2> constexpr Vector operator*(const Vector<T2, S>& vec) const { Vector out; for (int i = 0; i < S; ++i) out.arr[i] = arr[i] * vec.arr[i]; return out; }
		template<typename T2> constexpr Vector operator/(const Vector<T2, S>& vec) const { Vector out; for (int i = 0; i < S; ++i) out.arr[i] = arr[i] / vec.arr[i]; return out; }
		template<typename T2> constexpr Vector operator*(const T2& v) const { Vector out; for (int i = 0; i < S; ++i) out.arr[i] = arr[i] * v; return out; }
		template<typename T2> constexpr Vector operator/(const T2& v) const { Vector out; for (int i = 0; i < S; ++i) out.arr[i] = arr[i] / v; return out; }

		template<typename T2> constexpr void operator+= (const Vector<T2, S>& vec) { for (int i = 0; i < S; ++i) arr[i] += vec.arr[i]; }
		template<typename T2> constexpr void operator-= (const Vector<T2, S>& vec) { for (int i = 0; i < S; ++i) arr[i] -= vec.arr[i]; }
		template<typename T2> constexpr void operator*= (const Vector<T2, S>& vec) { for (int i = 0; i < S; ++i) arr[i] *= vec.arr[i]; }
		template<typename T2> constexpr void operator/= (const Vector<T2, S>& vec) { for (int i = 0; i < S; ++i) arr[i] /= vec.arr[i]; }
		template<typename T2> constexpr void operator*= (const T2& v) { for (int i = 0; i < S; ++i) arr[i] *= v; }
		template<typename T2> constexpr void operator/= (const T2& v) { for (int i = 0; i < S; ++i) arr[i] /= v; }

		template<typename T2> constexpr bool operator== (const Vector<T2, S>& vec) const { bool value = true; for (int i = 0; i < S; ++i) value &= arr[i] == vec.arr[i]; return value; }
		template<typename T2> constexpr bool operator!= (const Vector<T2, S>& vec) const { bool value = false; for (int i = 0; i < S; ++i) value |= arr[i] != vec.arr[i]; return value; }

		template<typename T2, size_t S2> constexpr void operator= (const Vector<T2, S2>& v)
		{
			for (size_t i = 0; i != (S2 < S ? S2 : S); ++i)
				arr[i] = v.arr[i];
		}

		constexpr inline T Lenght() const
		{
			T sum;
			for (size_t i = 0; i != S; ++i)
				sum += arr[i] * arr[i];
			return Math::Sqrt<T>(sum);
		}
		constexpr inline T SqrLenght() const
		{
			T sum;
			for (size_t i = 0; i != S; ++i)
				sum += arr[i] * arr[i];
			return sum;
		}
		constexpr inline void Normalise()
		{
			const T lenght = Lenght();
			for (int i = 0; i < S; ++i)
				arr[i] /= lenght;
		}
		constexpr inline Vector Normalised()
		{
			Vector out = *this;
			out.Normalise();
			return out;
		}
	};

	template<size_t S>
	using Veci = Vector<int, S>;
	template<size_t S>
	using Vecf = Vector<float, S>;
	template<size_t S>
	using Vecd = Vector<double, S>;

	template<typename T>
	struct Vector<T, 2>
	{
		union {
			struct {
				T x, y;
			};
			T arr[2]{ };
		};

		constexpr Vector() = default;
		template<typename T2, size_t S>
		constexpr Vector(const Vector<T2, S>& v)
		{
			for (size_t i = 0; i != (S < 2 ? S : 2); ++i)
				arr[i] = v.arr[i];
			for (size_t i = (S < 2 ? S : 2); i < 2; ++i)
				arr[i] = 0;
		}
		template<typename T2>
		constexpr explicit Vector(const T2& v) : x(v), y(v) { }
		template<typename ... Ts>
		constexpr Vector(Ts ... values)
			: arr{ T(values)... }
		{
		}
		constexpr Vector(T* ptr)
		{
			for (size_t i = 0; i != 2; ++i)
				arr[i] = ptr[i];
		}

		constexpr Vector operator-() const { return Vector(-x, -y); }

		template<typename T2> constexpr Vector operator+(const Vector<T2, 2>& b) const { return Vector(x + b.x, y + b.y); }
		template<typename T2> constexpr Vector operator-(const Vector<T2, 2>& b) const { return Vector(x - b.x, y - b.y); }
		template<typename T2> constexpr Vector operator*(const Vector<T2, 2>& v) const { return Vector(x * v.x, y * v.y); }
		template<typename T2> constexpr Vector operator/(const Vector<T2, 2>& v) const { return Vector(x / v.x, y / v.y); }
		template<typename T2> constexpr Vector operator*(const T2& v) const { return Vector(x * v, y * v); }
		template<typename T2> constexpr Vector operator/(const T2& v) const { return Vector(x / v, y / v); }

		template<typename T2> constexpr void operator+= (const Vector<T2, 2>& v) { x += v.x; y += v.y; }
		template<typename T2> constexpr void operator-= (const Vector<T2, 2>& v) { x -= v.x; y -= v.y; }
		template<typename T2> constexpr void operator*= (const Vector<T2, 2>& v) { x *= v.x; y *= v.y; }
		template<typename T2> constexpr void operator/= (const Vector<T2, 2>& v) { x /= v.x; y /= v.y; }
		template<typename T2> constexpr void operator*= (const T2& v) { x *= v; y *= v; }
		template<typename T2> constexpr void operator/= (const T2& v) { x /= v; y /= v; }

		template<typename T2> constexpr bool operator== (const Vector<T2, 2>& v) const { return x == v.x && y == v.y; }
		template<typename T2> constexpr bool operator!= (const Vector<T2, 2>& v) const { return x != v.x || y != v.y; }
		
		template<typename T2> constexpr void operator=(const Vector<T2, 2>& v) { x = v.x; y = v.y; }

		constexpr inline T Lenght() const { return Math::Sqrt<T>(x * x + y * y); }
		constexpr inline T SqrLenght() const { return x * x + y * y; }
		constexpr inline void Normalise()
		{
			const T lenght = Lenght();
			x /= lenght;
			y /= lenght;
		}
		constexpr inline Vector Normalised()
		{
			const T lenght = Lenght();
			return Vector(x / lenght, y / lenght);
		}
	};

	template<typename T>
	using Vec2 = Vector<T, 2>;
	using Vec2i = Vec2<int>;
	using Vec2f = Vec2<float>;
	using Vec2d = Vec2<double>;

	template<typename T>
	struct Vector<T, 3>
	{
		union {
			struct {
				T x, y, z;
			};
			T arr[3]{ };
		};

		constexpr Vector() = default;
		template<typename T2, size_t S>
		constexpr Vector(const Vector<T2, S>& v)
		{
			for (size_t i = 0; i != (S < 3 ? S : 3); ++i)
				arr[i] = v.arr[i];
			for (size_t i = (S < 3 ? S : 3); i < 3; ++i)
				arr[i] = 0;
		}
		template<typename ... Ts>
		constexpr Vector(Ts ... values)
			: arr{ T(values)... }
		{
		}
		template<typename T2>
		constexpr explicit Vector(const T2& v) : x(v), y(v), z(v) { }
		constexpr Vector(T* ptr)
		{
			for (size_t i = 0; i != 3; ++i)
				arr[i] = ptr[i];
		}

		constexpr Vector operator-() const { return Vector(-x, -y, -z); }

		template<typename T2> constexpr Vector operator+(const Vector<T2, 3>& b) const { return Vector(x + b.x, y + b.y, z + b.z); }
		template<typename T2> constexpr Vector operator-(const Vector<T2, 3>& b) const { return Vector(x - b.x, y - b.y, z - b.z); }
		template<typename T2> constexpr Vector operator*(const Vector<T2, 3>& v) const { return Vector(x * v.x, y * v.y, z * v.z); }
		template<typename T2> constexpr Vector operator/(const Vector<T2, 3>& v) const { return Vector(x / v.x, y / v.y, z / v.z); }
		template<typename T2> constexpr Vector operator*(const T2& v) const { return Vector(x * v, y * v, z * v); }
		template<typename T2> constexpr Vector operator/(const T2& v) const { return Vector(x / v, y / v, z / v); }

		template<typename T2> constexpr void operator+= (const Vector<T2, 3>& v) { x += v.x; y += v.y; z += v.z; }
		template<typename T2> constexpr void operator-= (const Vector<T2, 3>& v) { x -= v.x; y -= v.y; z -= v.z; }
		template<typename T2> constexpr void operator*= (const Vector<T2, 3>& v) { x *= v.x; y *= v.y; z *= v.z; }
		template<typename T2> constexpr void operator/= (const Vector<T2, 3>& v) { x /= v.x; y /= v.y; z /= v.z; }
		template<typename T2> constexpr void operator*= (const T2& v) { x *= v; y *= v; z *= v; }
		template<typename T2> constexpr void operator/= (const T2& v) { x /= v; y /= v; z /= v; }

		template<typename T2> constexpr bool operator== (const Vector<T2, 3>& v) const { return x == v.x && y == v.y && z == v.z; }
		template<typename T2> constexpr bool operator!= (const Vector<T2, 3>& v) const { return x != v.x || y != v.y || z != v.z; }
		
		template<typename T2> constexpr void operator=(const Vector<T2, 3>& v) { x = T(v.x); y = T(v.y); z = T(v.z); }

		constexpr inline T Lenght() const { return Math::Sqrt<T>(x * x + y * y + z * z); }
		constexpr inline T SqrLenght() const { return x * x + y * y + z * z; }
		constexpr inline void Normalise()
		{
			const T lenght = Lenght();
			x /= lenght;
			y /= lenght;
			z /= lenght;
		}
		constexpr inline Vector Normalised()
		{
			const T lenght = Lenght();
			return Vector(x / lenght, y / lenght, z / lenght);
		}
	};

	template<typename T>
	using Vec3 = Vector<T, 3>;
	using Vec3i = Vec3<int>;
	using Vec3f = Vec3<float>;
	using Vec3d = Vec3<double>;

	template<typename T>
	struct Vector<T, 4>
	{
		union {
			struct {
				T x, y, z, w;
			};
			T arr[4]{ };
		};

		constexpr Vector() = default;
		template<typename T2, size_t S>
		constexpr Vector(const Vector<T2, S>& v)
		{
			for (size_t i = 0; i != (S < 4 ? S : 4); ++i)
				arr[i] = v.arr[i];
			for (size_t i = (S < 4 ? S : 4); i < 4; ++i)
				arr[i] = 0;
		}
		template<typename ... Ts>
		constexpr Vector(Ts ... values)
			: arr{ T(values)... }
		{
		}
		template<typename T2>
		constexpr explicit Vector(const T2& v) : x(v), y(v), z(v), w(v) { }
		constexpr Vector(T* ptr)
		{
			for (size_t i = 0; i != 4; ++i)
				arr[i] = ptr[i];
		}

		constexpr Vector operator-() const { return Vector(-x, -y, -z, -w); }

		template<typename T2> constexpr Vector operator+(const Vector<T2, 4>& b) const { return Vector(x + b.x, y + b.y, z + b.z, w + b.w); }
		template<typename T2> constexpr Vector operator-(const Vector<T2, 4>& b) const { return Vector(x - b.x, y - b.y, z - b.z, w - b.w); }
		template<typename T2> constexpr Vector operator*(const Vector<T2, 4>& v) const { return Vector(x * v.x, y * v.y, z * v.z, w * v.w); }
		template<typename T2> constexpr Vector operator/(const Vector<T2, 4>& v) const { return Vector(x / v.x, y / v.y, z / v.z, w / v.w); }
		template<typename T2> constexpr Vector operator*(const T2& v) const { return Vector(x * v, y * v, z * v, w * v); }
		template<typename T2> constexpr Vector operator/(const T2& v) const { return Vector(x / v, y / v, z / v, w / v); }

		template<typename T2> constexpr void operator+= (const Vector<T2, 4>& v) { x += v.x; y += v.y; z += v.z; w += v.w; }
		template<typename T2> constexpr void operator-= (const Vector<T2, 4>& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; }
		template<typename T2> constexpr void operator*= (const Vector<T2, 4>& v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; }
		template<typename T2> constexpr void operator/= (const Vector<T2, 4>& v) { x /= v.x; y /= v.y; z /= v.z; w /= v.w; }
		template<typename T2> constexpr void operator*= (const T2& v) { x *= v; y *= v; z *= v; w *= v; }
		template<typename T2> constexpr void operator/= (const T2& v) { x /= v; y /= v; z /= v; w /= v; }

		template<typename T2> constexpr bool operator== (const Vector<T2, 4>& v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
		template<typename T2> constexpr bool operator!= (const Vector<T2, 4>& v) const { return x != v.x || y != v.y || z != v.z || w != v.w; }
		
		template<typename T2> constexpr void operator=(const Vector<T2, 4>& v) { x = v.x; y = v.y; z = v.z; w = v.w; }

		constexpr inline T Lenght() const { return Math::Sqrt<T>(x * x + y * y + z * z + w * w); }
		constexpr inline T SqrLenght() const { return x * x + y * y + z * z + w * w; }
		constexpr inline void Normalise()
		{
			const T lenght = Lenght();
			x /= lenght;
			y /= lenght;
			z /= lenght;
			w /= lenght;
		}
		constexpr inline Vector Normalised()
		{
			const T lenght = Lenght();
			return Vector(x / lenght, y / lenght, z / lenght, w / lenght);
		}
	};

	template<typename T>
	using Vec4 = Vector<T, 4>;
	using Vec4i = Vec4<int>;
	using Vec4f = Vec4<float>;
	using Vec4d = Vec4<double>;
}