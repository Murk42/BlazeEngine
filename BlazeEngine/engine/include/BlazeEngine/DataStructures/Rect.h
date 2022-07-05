#pragma once
#include "BlazeEngine/DataStructures/Vector.h"

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
		Rect(const Rect& r) : pos(r.pos), size(r.size) { }
		template<typename T2>
		Rect(const Rect<T2>& r) : pos(r.pos), size(r.size) { }
		Rect(const T& x, const T& y, const T& w, const T& h) : x(x), y(y), w(w), h(h) { }
		Rect(const Vec2<T>& pos, const Vec2<T>& size) : pos(pos), size(size) { }		

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