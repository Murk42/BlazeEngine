#pragma once
#include "BlazeEngine/Math/Math.h"

namespace Blaze
{	
	template<typename T>
	struct Quat
	{
		T x, y, z, w;

		constexpr Quat() : x(T(0)), y(T(0)), z(T(0)), w(T(1)) { }
		constexpr Quat(const Quat<T>& q) : x(q.x), y(q.y), z(q.z), w(q.w) { }
		constexpr Quat(const T& x, const T& y, const T& z, const T& w) : x(x), y(y), z(z), w(w) { }		
		constexpr Quat(Vec3<T> axis, T angle)
		{
			angle = angle * T(0.5);
			T temp = Math::Sin(angle);
			w = Math::Cos(angle);
			x = axis.x * temp;
			y = axis.y * temp;
			z = axis.z * temp;
		}
		constexpr Quat(Vec3<T> forward, Vec3<T> up)
		{			
			Vec3<T> vector = forward;
			Vec3<T> vector2 = up.CrossProduct(vector);
			Vec3<T> vector3 = vector.CrossProduct(vector2);
			T m00 = vector2.x;
			T m01 = vector2.y;
			T m02 = vector2.z;
			T m10 = vector3.x;
			T m11 = vector3.y;
			T m12 = vector3.z;
			T m20 = vector.x;
			T m21 = vector.y;
			T m22 = vector.z;

			T num8 = (m00 + m11) + m22;			
			if (num8 > 0.0)
			{
				T num = Math::Sqrt(num8 + 1.0);
				w = num * 0.5;
				num = 0.5 / num;
				x = (m12 - m21) * num;
				y = (m20 - m02) * num;
				z = (m01 - m10) * num;				
			}
			else if ((m00 >= m11) && (m00 >= m22))
			{
				T num7 = Math::Sqrt(((1.0 + m00) - m11) - m22);
				T num4 = 0.5 / num7;
				x = 0.5 * num7;
				y = (m01 + m10) * num4;
				z = (m02 + m20) * num4;
				w = (m12 - m21) * num4;				
			}
			else if (m11 > m22)
			{
				T num6 = Math::Sqrt(((1.0 + m11) - m00) - m22);
				T num3 = 0.5 / num6;
				x = (m10 + m01) * num3;
				y = 0.5 * num6;
				z = (m21 + m12) * num3;
				w = (m20 - m02) * num3;				
			}
			else
			{
				T num5 = Math::Sqrt(((1.0 + m22) - m00) - m11);
				T num2 = 0.5 / num5;
				x = (m20 + m02) * num2;
				y = (m21 + m12) * num2;
				z = 0.5 * num5;
				w = (m01 - m10) * num2;
			}
		}

		constexpr Quat<T> operator+(const Quat<T>& b) const
		{
			return Quat<T>(x + b.x, y + b.y, z + b.z, w + b.w);
		}
		constexpr Quat<T> operator*(const Quat<T>& b) const
		{
			Quat<T> out;
			out.x =  x * b.w + y * b.z - z * b.y + w * b.x;
			out.y = -x * b.z + y * b.w + z * b.x + w * b.y;
			out.z =  x * b.y - y * b.x + z * b.w + w * b.z;
			out.w = -x * b.x - y * b.y - z * b.z + w * b.w;
			return out;						
		}
		constexpr Quat<T> operator*(const T& b) const
		{
			return Quat<T>(x * b, y * b, z * b, w * b);
		}
		constexpr Quat<T> operator/(const T& b) const
		{
			return Quat<T>(x / b, y / b, z / b, w / b);
		}
		constexpr Vec3<T> operator*(const Vec3<T>& v) const
		{
			Vec3<T> temp = Vec3<T>(x, y, z).CrossProduct(v) * 2;
			return v + temp * w + Vec3<T>(x, y, z).CrossProduct(temp);
		}

		constexpr void operator+= (const Quat<T>& v) { *this = *this + v; }
		constexpr void operator*= (const Quat<T>& v) { *this = *this * v; }
		constexpr void operator*= (const T& v) { *this = *this * v; }
		constexpr void operator/= (const T& v) { *this = *this / v; }

		constexpr bool operator== (const Quat<T>& v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
		constexpr bool operator!= (const Quat<T>& v) const { return x != v.x || y != v.y || z != v.z || w != v.w; }

		constexpr void operator= (const Quat<T>& v) { x = v.x; y = v.y; z = v.z; w = v.w; }
		template<typename T2> constexpr void operator=(const Quat<T2>& v) { x = T(v.x); y = T(v.y); z = T(v.z); w = T(v.w); }

		constexpr inline T Magnitude() const { return Math::Sqrt(x * x + y * y + z * z + w * w); }
		constexpr inline T SqrMagnitude() const { return x * x + y * y + z * z + w * w; }
		constexpr inline void Normalise()
		{
			T mag = Magnitude();
			x /= mag;
			y /= mag;
			z /= mag;
			w /= mag;
		}
		constexpr inline Quat<T> Normalised() const
		{
			T mag = Magnitude();
			return Quat<T>(x / mag, y / mag, z / mag, w / mag);
		}
		constexpr inline bool IsNormalised() const { return (x * x + y * y + z * z + w * w) == T(1); }
		constexpr inline void Conjugate() { x = -x; y = -y; z = -z; }
		constexpr inline Quat<T> Conjugated() const { return Quat<T>(-x, -y, -z, w); }

		constexpr inline Vec4<T> Vector() { return { x, y, z, w }; }
	};

	typedef Quat<float> Quatf;
	typedef Quat<double> Quatd;
}