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

		constexpr float Radians(float degrees) { return degrees * PI / 180.0f; }
		constexpr double Radians(double degrees) { return degrees * PI / 180.0f; }				

		inline float Sin(float radians) { return sin(radians); }
		inline double Sin(double radians) { return sin(radians); }
		inline float Arcsin(float radians) { return asin(radians); }
		inline double Arcsin(double radians) { return asin(radians); }
		inline float Cos(float radians) { return cos(radians); }
		inline double Cos(double radians) { return cos(radians); }
		inline float Arccos(float radians) { return acos(radians); }
		inline double Arccos(double radians) { return acos(radians); }
		inline float Tan(float radians) { return tan(radians); }
		inline double Tan(double radians) { return tan(radians); }
		inline float Arctan(float radians) { return atan(radians); }
		inline double Arctan(double radians) { return atan(radians); }

		inline float Sqrt(float value) { return sqrt(value); }
		inline double Sqrt(double value) { return sqrt(value); }	

		template<typename T>
		constexpr T DotProduct(Vec2<T> a, Vec2<T> b)
		{
			return a.x * b.x + a.y * b.y;
		}
		template<typename T>
		constexpr T DotProduct(Vec3<T> a, Vec3<T> b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}		
		template<typename T>
		constexpr Vec3<T> CrossProduct(Vec3<T> a, Vec3<T> b)
		{
			Vec3<T> out;
			out.x = a.y * b.z - a.z * b.y;
			out.y = a.z * b.x - a.x * b.z;
			out.z = a.x * b.y - a.y * b.x;
			return out;
		}

		template<typename T>
		constexpr inline Matrix<T, 4, 4> TranslationMatrix(Vec3<T> v)
		{
			return Matrix<T, 4, 4>({
				1, 0, 0, v.x,
				0, 1, 0, v.y,
				0, 0, 1, v.z,
				0, 0, 0, 1,
				});
		}
		template<typename T>
		constexpr inline Matrix<T, 4, 4> ScalingMatrix(Vec3<T> v)
		{
			return Matrix<T, 4, 4>({
				v.x, 0, 0, 0,
				0, v.y, 0, 0,
				0, 0, v.z, 0,
				0, 0, 0, 1,
				});
		}
		template<typename T>
		constexpr inline Matrix<T, 4, 4> RotationMatrix(Quat<T> q)
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
		constexpr inline Matrix<T, 4, 4> OrthographicMatrix(T left, T right, T bottom, T top, T near, T far)
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
		constexpr inline Matrix<T, 4, 4> PerspectiveMatrix(T fov, T aspectRatio, T near, T far)
		{
			T S = T(1) / Tan(fov / T(2));

			return Matrix<T, 4, 4>({
				S, T(0), T(0), T(0),
				T(0), S * aspectRatio, T(0), T(0),
				T(0), T(0), far / (far - near), -far * near / (far - near),
				T(0), T(0), 1, T(0)
				});				
		}			
	};
}