#pragma once

namespace Blaze
{
	template<typename T>
	struct AxisAngle
	{
		T x, y, z, a;

		constexpr AxisAngle() = default;
		constexpr AxisAngle(const AxisAngle<T>& v) : x(v.x), y(v.y), z(v.z), a(v.a) { }
		constexpr AxisAngle(const T& x, const T& y, const T& z, const T& angle) : x(x), y(y), z(z), a(angle) { }
		constexpr AxisAngle(const Vec3<T>& axis, const T& angle) : x(axis.x), y(axis.y), z(axis.z), a(angle) { }
		template<typename T2>
		constexpr AxisAngle(const AxisAngle<T2>& v) : x(T(v.x)), y(T(v.y)), z(T(v.z)), a(T(v.a)) { }

		constexpr Vec3<T>& Axis() { return *(Vec3<T>*)this; }
		constexpr const Vec3<T>& Axis() const { return *(Vec3<T>*)this; }

		constexpr bool operator==(const AxisAngle<T>& v) const { return x == v.x && y == v.y && z == v.z && a == v.a; }
		constexpr bool operator!=(const AxisAngle<T>& v) const { return x != v.x || y != v.y || z != v.z || a != v.a; }

		constexpr void operator=(const AxisAngle<T>& v) { x = v.x; y = v.y; z = v.z; a = v.a; }
		template<typename T2>
		constexpr void operator=(const AxisAngle<T2>& v) { x = T(v.x); y = T(v.y); z = T(v.z); a = T(v.a); }
	};

	typedef AxisAngle<float> AxisAnglef;
	typedef AxisAngle<double> AxisAngled;
}