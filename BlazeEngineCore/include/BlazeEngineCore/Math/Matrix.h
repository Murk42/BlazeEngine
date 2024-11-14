#pragma once
#include "BlazeEngineCore/Math/Vector.h"
#include "BlazeEngineCore/Math/Quaternion.h"

namespace Blaze
{
	template<typename T, uintMem Sx, uintMem Sy>
	struct Matrix
	{
		std::array<std::array<T, Sx>, Sy> arr;

		constexpr Matrix()
			: arr({ })
		{
		}						
		constexpr Matrix(const std::array<T, Sx * Sy>& values)
		{
			memcpy(&arr, &values, sizeof(T) * Sx * Sy);
		}		
		constexpr Matrix(T* ptr)
		{
			for (uintMem i = 0; i != Sx * Sy; ++i)
				arr[i] = ptr[i];
		}

		constexpr Matrix<T, Sy, Sx> Transposed() const
		{
			Matrix<T, Sy, Sx> mat = *this;			
			for (uintMem i = 0; i < Sy; ++i)
				for (uintMem j = 0; j < Sy; ++j)
					mat.arr[j][i] = arr[i][j];

			return mat;
		}

		template<uintMem Sx2>
		constexpr Matrix<T, Sx2, Sy> operator* (const Matrix<T, Sx2, Sx>& m) const
		{
			Matrix<T, Sx2, Sy> out = Matrix<T, Sx2, Sy>(0.0f);
			
			for (uintMem i = 0; i < Sy; ++i)
				for (uintMem k = 0; k < Sx; ++k)				
					for (uintMem j = 0; j < Sx2; ++j)					
						out.arr[i][j] += arr[i][k] * m.arr[k][j];

			return out;
		}
		constexpr Matrix operator* (const T& v) const
		{
			Matrix mat = *this;

			for (uintMem i = 0; i < Sy; ++i)
				for (uintMem j = 0; j < Sx; ++j)
					mat.arr[i][j] *= v;

			return mat;
		}
		constexpr Vector<T, Sy> operator* (const Vector<T, Sy>& v) const
		{
			Vector<T, Sy> out;

			for (uintMem i = 0; i != Sy; ++i)
				for (uintMem j = 0; j != Sx; ++j)
					out.arr[i] += v.arr[j] * arr[i][j];

			return out;
		}

		constexpr Matrix& operator*= (const T& v)
		{
			for (uintMem i = 0; i < Sy; ++i)
				for (uintMem j = 0; j < Sx; ++j)
					arr[i][j] *= v;

			return *this;
		}
		constexpr Matrix& operator*= (const Matrix<T, Sx, Sy>& m)
		{
			Matrix res;
			
			for (uintMem i = 0; i < Sy; ++i)
				for (uintMem k = 0; k < Sx; ++k)
					for (uintMem j = 0; j < Sx; ++j)
						res.arr[i][j] += arr[i][k] * m.arr[k][j];

			for (uintMem i = 0; i < Sy; ++i)
				for (uintMem j = 0; j < Sx; ++j)
					arr[i][j] = res.arr[i][j];			

			return *this;
		}

		constexpr bool operator== (const Matrix<T, Sx, Sy>& m) const 
		{ 
			return memcmp(arr, m.arr, sizeof(T) * Sx * Sy) == 0;
		}
		constexpr bool operator!= (const Matrix<T, Sx, Sy>& m) const 
		{
			return memcmp(arr, m.arr, sizeof(T) * Sx * Sy) != 0; 
		}

		constexpr std::array<T, Sy>& operator[](uintMem index)
		{
			return arr[index];
		}
		constexpr const std::array<T, Sy>& operator[](uintMem index) const
		{
			return arr[index];
		}
	};

	template<typename T, uintMem S>
	struct Matrix<T, S, S>
	{
		static constexpr uintMem Sx = S;
		static constexpr uintMem Sy = S;
		std::array<std::array<T, Sy>, Sx> arr;

		constexpr Matrix()
			: arr({ })
		{
		}
		constexpr Matrix(const std::array<T, Sx * Sy>& values)
		{
			memcpy(&arr, &values, sizeof(T) * Sx * Sy);
		}
		constexpr Matrix(T* ptr)
		{
			for (uintMem i = 0; i != Sy; ++i)
				for (uintMem j = 0; j != Sx; ++j)
					arr[i][j] = ptr[j + i * Sx];
		}

		constexpr void Transpose()
		{
			for (uintMem i = 1; i < Sy; ++i)
				for (uintMem j = 0; j < i; ++j)
					std::swap(this->arr[i][j], this->arr[j][i]);
		}
		constexpr Matrix<T, Sx, Sy> Transposed() const
		{
			Matrix<T, Sx, Sy> mat = *this;
			mat.Transpose();
			return mat;
		}

		template<uintMem S2>
		constexpr Matrix<T, S2, Sy> operator* (const Matrix<T, S2, Sy>& m) const
		{
			Matrix<T, S2, Sy> out;
		
			for (uintMem i = 0; i != Sy; ++i)
				for (uintMem j = 0; j != S2; ++j)
					for (uintMem k = 0; k != Sx; ++k)
						out.arr[i][j] += arr[i][k] * m.arr[k][j];

			return out;
		}
		constexpr Matrix operator* (const T& v) const
		{
			Matrix mat = *this;
			
			for (uintMem i = 0; i < Sy; ++i)
				for (uintMem j = 0; j < Sx; ++j)
					mat.arr[i][j] *= v;
			return mat;
		}
		constexpr Vector<T, Sy> operator* (const Vector<T, Sy>& v) const
		{
			Vector<T, Sy> out;

			for (uintMem i = 0; i != Sy; ++i)
				for (uintMem j = 0; j != Sx; ++j)
					out[i] += v[j] * arr[i][j];

			return out;
		}

		constexpr Matrix& operator*= (const T& v)
		{
			for (uintMem i = 0; i != Sy; ++i)
				for (uintMem j = 0; j != Sx; ++j)
					arr[i][j] *= v;

			return *this;
		}
		constexpr Matrix& operator*= (const Matrix<T, Sx, Sy>& m)
		{
			Matrix<T, S, S> res;			
			
			for (uintMem i = 0; i < Sy; ++i)
				for (uintMem j = 0; j < Sx; ++j)
					for (uintMem k = 0; k < Sx; ++k)
						res.arr[i][j] += arr[i][k] * m.arr[k][j];

			for (uintMem i = 0; i < Sy; ++i)
				for (uintMem j = 0; j < Sx; ++j)
					arr[i][j] = res.arr[i][j];

			return *this;
		}

		constexpr bool operator== (const Matrix<T, Sx, Sy>& m) const 
		{ 
			return memcmp(&arr, &m.arr, sizeof(T) * Sx * Sy) == 0; 
		}
		constexpr bool operator!= (const Matrix<T, Sx, Sy>& m) const 
		{ 
			return memcmp(&arr, &m.arr, sizeof(T) * Sx * Sy) != 0; 
		}		

		constexpr std::array<T, Sy>& operator[](uintMem index)
		{
			return arr[index];
		}
		constexpr const std::array<T, Sy>& operator[](uintMem index) const
		{
			return arr[index];
		}

		static constexpr Matrix<T, Sx, Sy> Identity()
		{
			Matrix<T, Sx, Sy> mat;

			for (int i = 0; i < Sx; ++i)
				mat.arr[i][i] = 1;

			return mat;
		}
	};

	template<typename T>
	struct Matrix<T, 4, 4>
	{
		static constexpr uintMem Sx = 4;
		static constexpr uintMem Sy = 4;
		T arr[Sy][Sx];

		constexpr Matrix()
			: arr()
		{
		}
		constexpr Matrix(const std::array<std::array<T, Sx>, Sy>& values)
		{
			memcpy(arr, &values, sizeof(T) * Sx * Sy);
		}
		constexpr Matrix(T* ptr)
		{
			for (uintMem i = 0; i != Sy; ++i)
				for (uintMem j = 0; j != Sx; ++j)
					arr[i][j] = ptr[j + i * Sx];
		}

		constexpr void Transpose()
		{
			for (uintMem i = 1; i < Sy; ++i)
				for (uintMem j = 0; j < i; ++j)
					std::swap(this->arr[i][j], this->arr[j][i]);
		}
		constexpr Matrix<T, Sx, Sy> Transposed() const
		{
			Matrix<T, Sx, Sy> mat = *this;
			mat.Transpose();
			return mat;
		}
		
		static constexpr Matrix TranslationMatrix(Vec3<T> v)
		{
			return Matrix<T, 4, 4>({
				1, 0, 0, v.x,
				0, 1, 0, v.y,
				0, 0, 1, v.z,
				0, 0, 0, 1,
				});
		}		
		static constexpr Matrix ScalingMatrix(Vec3<T> v)
		{
			return Matrix<T, 4, 4>({
				v.x, 0, 0, 0,
				0, v.y, 0, 0,
				0, 0, v.z, 0,
				0, 0, 0, 1,
				});
		}		
		static constexpr Matrix ScalingMatrix(T v)
		{
			return Matrix<T, 4, 4>({
				v, 0, 0, 0,
				0, v, 0, 0,
				0, 0, v, 0,
				0, 0, 0, 1,
				});
		}
		static constexpr Matrix RotationMatrix(Quat<T> q)
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
			return Matrix({
				1 - 2 * yy - 2 * zz, 2 * xy - 2 * wz, 2 * xz + 2 * wy, 0,
				2 * xy + 2 * wz, 1 - 2 * xx - 2 * zz, 2 * yz - 2 * wx, 0,
				2 * xz - 2 * wy, 2 * yz + 2 * wx, 1 - 2 * xx - 2 * yy, 0,
				0, 0, 0, 1
				});
		}
		static constexpr Matrix RotationMatrixAxisX(T radians)
		{
			const T sin = Math::Sin(radians);
			const T cos = Math::Cos(radians);
			return Matrix({
				1,   0,   0,   0,
				0, cos,-sin,   0,
				0, sin, cos,   0,
				0,   0,   0,   1
				});
		}
		static constexpr Matrix RotationMatrixAxisY(T radians)
		{
			const T sin = Math::Sin(radians);
			const T cos = Math::Cos(radians);
			return Matrix({
				 cos,   0, sin,  0,
				   0,   1,   0,  0,
				-sin,   0, cos,  0,
				   0,   0,   0,  1
				});
		}
		static constexpr Matrix RotationMatrixAxisZ(T radians)
		{
			const T sin = Math::Sin(radians);
			const T cos = Math::Cos(radians);
			return Matrix({
				cos,-sin,   0,   0,
				sin, cos,   0,   0,
				  0,   0,   1,   0,
				  0,   0,   0,   1
				});
		}
		
		static constexpr Matrix<T, 4, 4> OrthographicMatrix(T left, T right, T bottom, T top, T near, T far)
		{
			T rml = right - left;
			T tmb = top - bottom;
			T fmn = far - near;
			Matrix m({
				T(2) / rml, T(0), T(0), T(0),
				T(0), T(2) / tmb, T(0), T(0),
				T(0), T(0), T(2) / fmn, T(0),
				-(right + left) / rml, -(top + bottom) / tmb, (far + near) / fmn, T(1),
				});
			m.Transpose();
			return m;
		}
		//After multiplying a vector (the fourth component of the vector'w' is expected to be 1) the x, y, z
		//components need to be divided by w to get them in clip space (x, y, z will be in the range [-1, 1]).
		static constexpr Matrix<T, 4, 4> PerspectiveMatrix(T fov, T aspectRatio, T near, T far)
		{			
			T S = T(1) / Math::Tan(fov / T(2));

			return Matrix<T, 4, 4>({
				S	, T(0)				, T(0)							, T(0),
				T(0), S * aspectRatio	, T(0)							, T(0),
				T(0), T(0)				, (far + near) / (far - near)	,-2 * near * far / (far - near),
				T(0), T(0)				, 1	, 0
				});			
		}

		template<uintMem S2>
		constexpr Matrix<T, S2, Sy> operator* (const Matrix<T, S2, Sy>& m) const
		{
			Matrix<T, S2, Sy> out;

			for (uintMem i = 0; i != Sy; ++i)
				for (uintMem j = 0; j != S2; ++j)
					for (uintMem k = 0; k != Sx; ++k)
						out.arr[i][j] += arr[i][k] * m.arr[k][j];

			return out;
		}
		constexpr Matrix operator* (const T& v) const
		{
			Matrix mat = *this;

			for (uintMem i = 0; i < Sy; ++i)
				for (uintMem j = 0; j < Sx; ++j)
					mat.arr[i][j] *= v;

			return mat;
		}
		constexpr Vector<T, Sy> operator* (const Vector<T, Sy>& v) const
		{
			Vector<T, Sy> out;

			for (uintMem i = 0; i != Sy; ++i)
				for (uintMem j = 0; j != Sx; ++j)
					out[i] += v[j] * arr[i][j];

			return out;
		}

		constexpr Matrix& operator*= (const T& v)
		{
			for (uintMem i = 0; i != Sy; ++i)
				for (uintMem j = 0; j != Sx; ++j)
					arr[i][j] *= v;

			return *this;
		}
		constexpr Matrix& operator*= (const Matrix<T, Sx, Sy>& m)
		{
			Matrix<T, Sx, Sy> res;

			for (uintMem i = 0; i < Sy; ++i)
				for (uintMem j = 0; j < Sx; ++j)
					for (uintMem k = 0; k < Sx; ++k)
						res.arr[i][j] += arr[i][k] * m.arr[k][j];

			for (uintMem i = 0; i < Sy; ++i)
				for (uintMem j = 0; j < Sx; ++j)
					arr[i][j] = res.arr[i][j];

			return *this;
		}

		constexpr bool operator== (const Matrix<T, Sx, Sy>& m) const
		{
			return memcmp(&arr, &m.arr, sizeof(T) * Sx * Sy) == 0;
		}
		constexpr bool operator!= (const Matrix<T, Sx, Sy>& m) const
		{
			return memcmp(&arr, &m.arr, sizeof(T) * Sx * Sy) != 0;
		}		

		static constexpr Matrix<T, Sx, Sy> Identity()
		{
			Matrix<T, Sx, Sy> mat;

			for (int i = 0; i < Sx; ++i)
				mat.arr[i][i] = 1;

			return mat;
		}
	};
	
	template<typename T>
	using Mat2 = Matrix<T, 2, 2>;
	template<typename T>
	using Mat3 = Matrix<T, 3, 3>;
	template<typename T>
	using Mat4 = Matrix<T, 4, 4>;

	using Mat2f = Matrix<float, 2, 2>;
	using Mat2d = Matrix<double, 2, 2>;
	using Mat3f = Matrix<float, 3, 3>;
	using Mat3d = Matrix<double, 3, 3>;
	using Mat4f = Matrix<float, 4, 4>;
	using Mat4d = Matrix<double, 4, 4>;

	template struct BLAZE_CORE_API Matrix<float, 2, 2>;
	template struct BLAZE_CORE_API Matrix<double, 2, 2>;
	template struct BLAZE_CORE_API Matrix<float, 3, 3>;
	template struct BLAZE_CORE_API Matrix<double, 3, 3>;
	template struct BLAZE_CORE_API Matrix<float, 4, 4>;
	template struct BLAZE_CORE_API Matrix<double, 4, 4>;
}