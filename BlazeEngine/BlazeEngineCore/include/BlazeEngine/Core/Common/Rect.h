#pragma once
#include "BlazeEngine/Core/Math/Vector.h"

namespace Blaze
{
	template<typename T>
	struct Rect
	{
		union
		{
			struct {
				Vec2<T> pos;
				Vec2<T> size;
			};
			struct {
				T x, y, w, h;
			};			
		};

		Rect() : pos(), size() { }
		Rect(const Rect& r) : x(r.x), y(r.y), w(r.w), h(r.h) { }
		Rect(const T& x, const T& y, const T& w, const T& h) : x(x), y(y), w(w), h(h) { }
		Rect(const Vec2<T>& pos, const Vec2<T>& size) : x(pos.x), y(pos.y), w(size.x), h(size.y) { }
		template<typename T2>
		explicit Rect(const Rect<T2>& r) : x(r.x), y(r.y), w(r.w), h(r.h) { }

		bool operator==(const Rect& other)
		{
			return pos == other.pos && size == other.size;
		}
		bool operator!=(const Rect& other)
		{
			return pos != other.pos || size != other.size;
		}

		Rect& operator=(const Rect& other)
		{
			x = other.x; y = other.y; w = other.w; h = other.h;
			return *this;
		}
	};

	typedef Rect<int> Recti;
	typedef Rect<unsigned> Rectu;
	typedef Rect<float> Rectf;
	typedef Rect<double> Rectd;
}