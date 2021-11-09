#pragma once
#include "Vector.h"

namespace Blaze
{
	template<typename T, size_t Sx, size_t Sy>
	struct Matrix
	{
		T arr[Sx * Sy]{ };

		constexpr Matrix() = default;
		template<typename T2>
		constexpr Matrix(const Matrix<T2, Sx, Sy>& m)
		{
			for (size_t i = 0; i != Sx * Sy; ++i)
				arr[i] = T(m.arr[i]);
		}
		template<typename ... Ts>
		constexpr Matrix(Ts ... values)
			: arr{ T(values)... }
		{

		}
		constexpr Matrix(const T& v)
		{
			for (T* ptr = arr; ptr != arr + Sx * Sy; ++ptr)
				*ptr = v;
		}
		constexpr Matrix(T* ptr)
		{
			for (size_t i = 0; i != Sx * Sy; ++i)
				arr[i] = ptr[i];
		}

		template<size_t Sx2>
		constexpr Matrix<T, Sx2, Sy> operator* (const Matrix<T, Sx2, Sx>& m) const
		{
			Matrix<T, Sx2, Sy> out = Matrix<T, Sx2, Sy>(0.0f);

			size_t i = 0, j = 0, k = 0;
			for (i = 0; i < Sy; ++i)
				for (k = 0; k < Sx; ++k)
				{
					T mul = arr[k + i * Sx];
					for (j = 0; j < Sx2; ++j)
					{
						out.arr[j + i * Sx2] += mul * m.arr[j + k * Sx2];
					}
				}
			return out;
		}
		constexpr Matrix operator* (const T& v) const
		{
			Matrix mat = *this;
			for (size_t i = 0; i < Sx * Sy; ++i)
				mat.arr[i] *= v;
			return mat;
		}
		constexpr Vector<T, Sy> operator* (const Vector<T, Sy>& v) const
		{
			Vector<T, Sy> out = Vector<T, Sy>();
			for (size_t y = 0; y != Sy; ++y)
				for (size_t x = 0; x != Sx; ++x)
					out.arr[y] += v.arr[x] * arr[x + y * Sx];
			return out;
		}

		constexpr void operator*= (const T& v)
		{
			for (size_t i = 0; i < Sx * Sy; ++i)
				arr[i] *= v;
		}
		constexpr void operator*= (const Matrix<T, Sx, Sy>& m)
		{
			Matrix<T, Sx, Sy> res;

			for (size_t i = 0; i < Sx; ++i)
				for (size_t j = 0; j < Sy; ++j)
					res.arr[i + j * Sx] = 0;
			
			for (size_t i = 0; i < Sy; ++i)
				for (size_t k = 0; k < Sx; ++k)
					for (size_t j = 0; j < Sx; ++j)
						res.arr[j + i * Sx] += arr[k + i * Sx] * m.arr[j + k * Sx];				

			for (size_t i = 0; i < Sx; ++i)
				for (size_t j = 0; j < Sy; ++j)
					arr[i + j * Sx] = res.arr[i + j * Sx];			
		}

		constexpr bool operator== (const Matrix<T, Sx, Sy>& m) const { return memcmp(arr, m.arr, sizeof(T) * Sx * Sy) == 0; }
		constexpr bool operator!= (const Matrix<T, Sx, Sy>& m) const { return memcmp(arr, m.arr, sizeof(T) * Sx * Sy) != 0; }

		constexpr void operator= (const Matrix<T, Sx, Sy>& m)
		{
			for (size_t i = 0; i != Sx * Sy; ++i)
				arr[i] = m.arr[i];
		}
		template<typename T2> constexpr void operator=(const Matrix<T2, Sx, Sy>& m)
		{
			for (size_t i = 0; i < Sx * Sy; ++i)
				arr[i] = T(m.arr[i]);
		}
	};

	template<typename T, size_t S>
	struct Matrix<T, S, S>
	{
		T arr[S * S]{ };

		constexpr Matrix() = default;
		template<typename T2>
		constexpr Matrix(const Matrix<T2, S, S>& m)
		{
			for (size_t i = 0; i != S * S; ++i)
				arr[i] = T(m.arr[i]);
		}
		template<typename ... Ts>
		constexpr Matrix(Ts ... values)
			: arr{ T(values)... }
		{

		}
		constexpr Matrix(const T& v)
		{
			for (T* ptr = arr; ptr != arr + S * S; ++ptr)
				*ptr = v;
		}
		constexpr Matrix(T* ptr)
		{
			for (size_t i = 0; i != S * S; ++i)
				arr[i] = ptr[i];
		}

		template<size_t S2>
		constexpr Matrix<T, S2, S> operator* (const Matrix<T, S2, S>& m) const
		{
			Matrix<T, S2, S> out = Matrix<T, S2, S>(0.0f);

			size_t i = 0, j = 0, k = 0;
			for (i = 0; i != S; ++i)
				for (j = 0; j != S2; ++j)
					for (k = 0; k != S; ++k)
						out.arr[j + i * S2] += arr[k + i * S] * m.arr[j + k * S2];
			return out;
		}
		constexpr Matrix operator* (const T& v) const
		{
			Matrix mat = *this;
			for (size_t i = 0; i < S * S; ++i)
				mat.arr[i] *= v;
			return mat;
		}
		constexpr Vector<T, S> operator* (const Vector<T, S>& v) const
		{
			Vector<T, S> out = Vector<T, S>();
			for (size_t y = 0; y != S; ++y)
				for (size_t x = 0; x != S; ++x)
					out.arr[y] += v.arr[x] * arr[x + y * S];
			return out;
		}

		constexpr void operator*= (const T& v)
		{
			for (size_t i = 0; i < S * S; ++i)
				arr[i] *= v;
		}
		constexpr void operator*= (const Matrix<T, S, S>& m)
		{
			Matrix<T, S, S> res;
			
			for (size_t i = 0; i < S; ++i)
				for (size_t j = 0; j < S; ++j)
					res.arr[i + j * S] = 0;

			size_t i, j, k;
			for (i = 0; i < S; ++i)
				for (j = 0; j < S; ++j)
					for (k = 0; k < S; ++k) {
						res.arr[j + i * S] += arr[k + i * S] * m.arr[j + k * S];
					}

			for (size_t i = 0; i < S; ++i)
				for (size_t j = 0; j < S; ++j)
					arr[i + j * S] = res.arr[i + j * S];
		}

		constexpr bool operator== (const Matrix<T, S, S>& m) const { return memcmp(arr, m.arr, sizeof(T) * S * S) == 0; }
		constexpr bool operator!= (const Matrix<T, S, S>& m) const { return memcmp(arr, m.arr, sizeof(T) * S * S) != 0; }

		constexpr void operator= (const Matrix<T, S, S>& m)
		{
			for (size_t i = 0; i != S * S; ++i)
				arr[i] = m.arr[i];
		}
		template<typename T2> constexpr void operator=(const Matrix<T2, S, S>& m)
		{
			for (size_t i = 0; i < S * S; ++i)
				arr[i] = T(m.arr[i]);
		}

		constexpr void Transpose()
		{
			for (size_t i = 1; i < S; ++i)
				for (size_t j = 0; j < i; ++j)
				{
					T temp = this->arr[i + j * S];
					this->arr[i + j * S] = this->arr[j + i * S];
					this->arr[j + i * S] = temp;
				}					
		}
		constexpr Matrix<T, S, S> Transposed() const
		{
			Matrix<T, S, S> mat = Matrix<T, S, S>(*this);
			mat.Transpose();
			return mat;
		}

		static constexpr Matrix<T, S, S> Identity()
		{
			Matrix<T, S, S> mat = Matrix<T, S, S>(0.0f);
			for (int i = 0; i < S; ++i)
				mat.arr[i * (S + 1)] = T(1.0f);
			return mat;
		}
	};

	typedef Matrix<float, 2, 2> Mat2f;
	typedef Matrix<float, 3, 3> Mat3f;
	typedef Matrix<float, 4, 4> Mat4f;
	typedef Matrix<double, 2, 2> Mat2d;
	typedef Matrix<double, 3, 3> Mat3d;
	typedef Matrix<double, 4, 4> Mat4d;
}