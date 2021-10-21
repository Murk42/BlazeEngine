#pragma once
#include "BlazeEngine/DataStructures/Matrix.h"
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/DataStructures/Quaternion.h"
#include "BlazeEngine/DataStructures/Rect.h"
#include <algorithm>

namespace Blaze
{
	namespace Math
	{
		constexpr long double PI = 3.14159265358979323846264338327950288419716939937510;

		template<typename T>
		constexpr inline T Radians(const T& v)
		{
			return v * T(PI) / T(180);
		}		
		template<typename T>
		constexpr inline T Degrees(const T& v)
		{
			return v * T(180) / T(PI);
		}

		template<typename T1, typename T2>
		constexpr inline T1 Mod(const T1& v, const T2& m)
		{
			return fmod(v, m);
		}

		template<typename T>
		constexpr inline T Tan(const T& v)
		{
			return tan(v);
		}
		template<typename T>
		constexpr inline T Atan(const T& v)
		{
			return atan(v);
		}
		template<typename T>
		constexpr inline T Sin(const T& v)
		{
			return sin(v);
		}
		template<typename T>
		constexpr inline T Asin(const T& v)
		{
			return asin(v);
		}
		template<typename T>
		constexpr inline T Cos(const T& v)
		{
			return cos(v);
		}
		template<typename T>
		constexpr inline T Acos(const T& v)
		{
			return acos(v);
		}

		template<typename T>
		constexpr inline T Sqrt(const T& v)
		{						
			return sqrt(v);
		}		

		template<typename T>
		constexpr inline T DotProduct(const Vec3<T>& a, const Vec3<T>& b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}
		template<typename T>
		constexpr inline Vec3<T> CrossProduct(const Vec3<T>& a, const Vec3<T>& b)
		{
			Vec3<T> out;
			out.x = a.y * b.z - a.z * b.y;
			out.y = a.z * b.x - a.x * b.z;
			out.z = a.x * b.y - a.y * b.x;
			return out;
		}

		template<typename T>
		constexpr inline Matrix<T, 4, 4> TranslationMatrix(const Vec3<T>& v)
		{
			return Matrix<T, 4, 4>({
				1, 0, 0, v.x,
				0, 1, 0, v.y,
				0, 0, 1, v.z,
				0, 0, 0, 1,
				});
		}
		template<typename T>
		constexpr inline Matrix<T, 4, 4> ScalingMatrix(const Vec3<T>& v)
		{
			return Matrix<T, 4, 4>({
				v.x, 0, 0, 0,
				0, v.y, 0, 0,
				0, 0, v.z, 0,
				0, 0, 0, 1,
				});
		}
		template<typename T>
		constexpr inline Matrix<T, 4, 4> RotationMatrix(const Quat<T>& q)
		{			
			const T xx = q.x * q.x;
			const T yy = q.y * q.y;
			const T zz = q.z * q.z;
			const T xy = q.x * q.y;
			const T xz = q.x * q.z;
			const T yz = q.y * q.z;
			const T wx = q.w * q.x;
			const T wy = q.w * q.y;
			const T wz = q.w * q.z;
			return Matrix<T, 4, 4>(
				1 - 2 * yy - 2 * zz, 2 * xy - 2 * wz, 2 * xz + 2 * wy, 0,
				2 * xy + 2 * wz, 1 - 2 * xx - 2 * zz, 2 * yz - 2 * wx, 0,
				2 * xz - 2 * wy, 2 * yz + 2 * wx, 1 - 2 * xx - 2 * yy, 0,
				0, 0, 0, 1
				);			
		}

		template<typename T>
		constexpr inline Matrix<T, 4, 4> OrthographicMatrix(const T& left, const T& right, const T& bottom, const T& top, const T& near, const T& far)
		{
			T rml = right - left;
			T tmb = top - bottom;
			T fmn = far - near;
			Matrix<T, 4, 4> m({
				T(2) / rml, T(0), T(0), T(0),
				T(0), T(2) / tmb, T(0), T(0),
				T(0), T(0), T(-2) / fmn, T(0),
				 -(right + left) / rml, -(top + bottom) / tmb, -(far + near) / fmn, T(1),
				});
			m.Transpose();
			return m;
		}
		template<typename T>
		constexpr inline Matrix<T, 4, 4> PerspectiveMatrix(const T& fov, const T& aspectRatio, const T& near, const T& far)
		{
			T S = T(1) / Tan<T>(fov / T(2));

			return Matrix<T, 4, 4>({
				S, T(0), T(0), T(0),
				T(0), S * aspectRatio, T(0), T(0),
				T(0), T(0), far / (far - near), -far * near / (far - near),
				T(0), T(0), 1, T(0)
				});				
		}			
	};
}