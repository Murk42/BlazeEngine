#pragma once
#include "BlazeEngineCore/Math/Vector.h"
#include "BlazeEngineCore/Math/Matrix.h"

namespace Blaze::Math::Shapes
{
	template<typename T>
	class Line2D
	{
	public:
		T a, b, c; 

		constexpr Line2D() 
			: a(0), b(0), c(1) 
		{ 
		}		
		constexpr Line2D(T a, T b, T c)
			: a(a), b(b), c(c)
		{
		}
		constexpr Line2D(Vec2<T> point1, Vec2<T> point2)
		{
			Set(point1, point2);
		}		

		constexpr void Set(Vec2<T> point1, Vec2<T> point2)
		{
			Vec2<T> direction = (point2 - point1).Normalised();

			a = -direction.y;
			b = direction.x;

			c = -(a * point1.x + b * point1.y);
		}
		
		constexpr T SignedDistance(Vec2<T> point) const
		{
			return a * point.x + b * point.y + c;
		}
		constexpr T Distance(Vec2<T> point) const
		{
			float value = SignedDistance(point);
			return (value < 0.0f ? -value : value);
		}
		constexpr T DistanceSqr(Vec2<T> point) const
		{
			float value = SignedDistance(point);
			return value * value;
		}
		constexpr bool SideOfLine(Vec2<T> point)
		{
			float value = SignedDistance(point);
			return value > 0.0f;
		}		
		constexpr bool ContainsPoint(Vec2<T> point, float epsilon = 0.00001f)
		{
			float distance = a * point.x + b * point.y + c;			
			return distance < epsilon && -distance < epsilon;
		}
		constexpr Vec2<T> ProjectPoint(Vec2<T> point)
		{
			return point - Vec2<T>(a, b) * SignedDistance(point);
		}

		constexpr Vec2<T> Direction() const
		{
			return Vec2<T>(b, -a);
		}
		constexpr Vec2<T> Normal() const
		{
			return Vec2<T>(a, b);
		}

		constexpr Line2D ApplyAffineTransformation(const Mat2<T>& matrix, Vec2<T> b) const
		{
			Vec2<T> u = matrix.Transposed() * Vec2<T>(this->a, this->b);
			T c = this->a * b.x + this->b * b.y + this->c;
			return Line2D(u.x, u.y, c);
		}		
	};	

	template<typename T>
	class Ray2D
	{		
	public:
		Vec2<T> point;
		Vec2<T> direction;

		constexpr Ray2D()
		{
		}		
		constexpr Ray2D(Vec2<T> point, Vec2<T> direction)
			: point(point), direction(direction)
		{
		}	

		constexpr T Distance(Vec2<T> point) const
		{
			Vec2<T> offset = point - this->point;
			T t = offset.DotProduct(direction);

			if (t < 0)
				return offset.Length();
			else
				return Line2D<T>(this->point, this->point + direction).Distance(point);			
		}
		constexpr T DistanceSqr(Vec2<T> point) const
		{
			Vec2<T> offset = point - this->point;
			T t = offset.DotProduct(direction);

			if (t < 0)
				return offset.SqrLength();
			else
				return Line2D<T>(this->point, this->point + direction).DistanceSqr(point);
		}
		constexpr bool ContainsPoint(Vec2<T> point, float epsilon = 0.00001f)
		{		
			Vec2<T> offset = point - this->point;
			T t = offset.DotProduct(direction);

			if (t < 0)			
				return offset.SqrLength() < epsilon * epsilon;			
			else
				return (point - (this->point + direction * t)).SqrLength() < epsilon * epsilon;			
		}		

		constexpr Vec2<T> PointOnRay(T t)
		{
			return point + direction * t;
		}

		constexpr Ray2D ApplyAffineTransformation(const Mat2<T>& matrix, Vec2<T> b) const
		{
			Ray2D ray;
			ray.point = matrix * point + b;
			ray.direction = matrix * direction;
			return ray;
		}		
	};

	template<typename T>
	class Segment2D
	{
	public:
		Vec2f pointA;
		Vec2f pointB;

		constexpr Segment2D()
		{

		}
		constexpr Segment2D(Vec2f A, Vec2f B)
			: pointA(A), pointB(B)
		{
		}		

		constexpr T Distance(Vec2<T> point) const
		{
			Vec2<T> direction = pointB - pointA;
			Vec2<T> offset = point - pointA;
			T t = offset.DotProduct(direction);

			if (t < 0)
				return offset.Length();
			else if (t > 1)
				return (point - pointB).Length();
			else
				return Line2D<T>(pointA, pointB).Distance(point);
		}
		constexpr T DistanceSqr(Vec2<T> point) const
		{
			Vec2<T> direction = pointB - pointA;
			Vec2<T> offset = point - pointA;
			T t = offset.DotProduct(direction);

			if (t < 0)
				return offset.SqrLength();
			else if (t > 1)
				return (point - pointB).SqrLength();
			else
				return Line2D<T>(pointA, pointB).DistanceSqr(point);
		}
		constexpr bool ContainsPoint(Vec2<T> point, float epsilon = 0.00001f)
		{
			Vec2<T> direction = pointB - pointA;
			Vec2<T> offset = point - pointA;
			T t = offset.DotProduct(direction);

			if (t < 0)
				return offset.SqrLength() < epsilon * epsilon;
			else if (t > 1)
				return (point - pointB).SqrLength() < epsilon * epsilon;
			else
				return Line2D<T>(pointA, pointB).DistanceSqr(point) < epsilon * epsilon;
		}

		constexpr Segment2D ApplyAffineTransformation(const Mat2<T>& matrix, Vec2<T> b) const
		{
			Segment2D segment;
			segment.pointA = matrix * pointA + b;
			segment.pointB = matrix * pointB + b;			
			return segment;
		}
	};

	template<typename T, uintMem _VertexCount>
	class Polygon2D
	{
	public:		
		static constexpr uintMem VertexCount = _VertexCount;
		Vec2<T> points[VertexCount];

		constexpr Polygon2D()
			: points({ })
		{
		}
		constexpr Polygon2D(const Vec2<T> (&points)[VertexCount])
		{
			for (uintMem i = 0; i < VertexCount; ++i)
				this->points[i] = points[i];
		}
		constexpr Polygon2D(Vec2<T>* points)			
		{
			for (uintMem i = 0; i < VertexCount; ++i)
				this->points[i] = points[i];
		}
	};

	template<typename T>
	class Quad2D : public Polygon2D<T, 4>
	{
	public:
		constexpr Quad2D()
			: Polygon2D<T, 4>()
		{
		}
		constexpr Quad2D(const Vec2<T> (&points)[4])
			: Polygon2D<T, 4>(points)
		{
		}
		constexpr Quad2D(Vec2<T>* points)
			: Polygon2D<T, 4>(points)
		{

		}

		constexpr bool IsInside(Vec2<T> point) const
		{
			return 
				!Line2D<T>(Polygon2D<T, 4>::points[0], Polygon2D<T, 4>::points[1]).SideOfLine(point) &&
				!Line2D<T>(Polygon2D<T, 4>::points[1], Polygon2D<T, 4>::points[2]).SideOfLine(point) &&
				!Line2D<T>(Polygon2D<T, 4>::points[2], Polygon2D<T, 4>::points[3]).SideOfLine(point) &&
				!Line2D<T>(Polygon2D<T, 4>::points[3], Polygon2D<T, 4>::points[0]).SideOfLine(point);
		}

		constexpr Quad2D ApplyAffineTransformation(const Mat2<T>& matrix, Vec2<T> b) const
		{
			Quad2D quad;
			quad.points[0] = matrix * Polygon2D<T, 4>::points[0] + b;
			quad.points[1] = matrix * Polygon2D<T, 4>::points[1] + b;
			quad.points[2] = matrix * Polygon2D<T, 4>::points[2] + b;
			quad.points[3] = matrix * Polygon2D<T, 4>::points[3] + b;
			return quad;
		}
	};

	template<typename T>
	constexpr bool Intersection(const Line2D<T>& A, const Line2D<T>& B, Vec2<T>& point)
	{
		T scale = B.a * A.b - A.a * B.b;

		if (scale == 0)
			return false;

		T x = A.b * B.c - B.b * A.c;
		T y = A.a * B.c - B.a * A.c;

		point = Vec2<T>(scale / x, scale / y);

		return true;
	}

	//Returns t for which (point + dir * t) is inside the line
	template<typename T>
	constexpr bool Intersection(const Ray2D<T>& ray, const Line2D<T>& line, T& t)
	{
		float temp = line.a * ray.direction.x + line.b * ray.direction.y;

		if (temp == 0)
			return false;

		t = -(line.a * ray.point.x + line.b * ray.point.y + line.c) / temp;

		return t >= 0;		
	}

	template<typename T>
	constexpr bool Intersection(const Ray2D<T>& ray, const Line2D<T>& line, Vec2<T>& point)
	{
		float temp = line.a * ray.dir.x + line.b * ray.dir.y;

		if (temp == 0)
			return false;

		T t = -(line.a * ray.point.x + line.b * ray.point.y + line.c) / temp;
		point = ray.point + ray.direction * t;

		return t >= 0;
	}

	template<typename T>
	constexpr bool Intersection(const Segment2D<T>& segment, const Line2D<T>& line, T& t)
	{
		float _t;
		bool intersects = Intersection(Ray2D<T>(segment.pointA, segment.pointB - segment.pointA), line, _t);

		if (!intersects || _t < 0 || _t > 1)
			return false;

		t = _t;

		return true;
	}
	template<typename T>
	constexpr bool Intersection(const Segment2D<T>& segment, const Line2D<T>& line, Vec2<T>& point)
	{
		float t;

		const Vec2<T> dir = segment.pointB - segment.pointA;
		bool intersects = Intersection(Ray2D<T>(segment.pointA, dir), line, t);

		if (!intersects || t < 0 || t > 1)
			return false;

		point = segment.pointA + dir * t;

		return true;
	}
	template<typename T>
	constexpr bool Intersection(const Segment2D<T>& segment, const Ray2D<T>& ray, Vec2<T>& point)
	{
		Vec2<T> dir1 = segment.pointB - segment.pointA;
		Vec2<T> dir2 = ray.direction;
		dir2 = Vec2<T>(-dir2.y, dir2.x);

		T dot = dir1.DotProduct(dir2);

		if (dot == 0)
			return false;

		Vec2<T> offset = ray.point - segment.pointA;

		T t1 = offset.DotProduct(dir2) / dot;
		T t2 =-offset.DotProduct(dir1) / dot;

		if (t1 < 0 || t1 > 1 || t2 < 0)
			return false;

		point = segment.pointA + dir1 * t1;

		return true;
	}
	template<typename T>
	constexpr bool Intersection(const Segment2D<T>& segment1, const Segment2D<T>& segment2, Vec2<T>& point)
	{
		Vec2<T> dir1 = segment1.pointB - segment1.pointA;
		Vec2<T> dir2 = segment2.pointB - segment2.pointA;
		dir2 = Vec2<T>(-dir2.y, dir2.x);

		T dot = dir1.DotProduct(dir2);

		if (dot == 0)
			return false;

		Vec2<T> offset = segment2.pointA - segment1.pointA;

		T t1 = offset.DotProduct(dir2) / dot;
		T t2 = offset.DotProduct(dir1) / dot;

		if (t1 < 0 || t1 > 1 || t2 < 0 || t2 > 1)
			return false;

		point = segment1.pointA + dir1 * t1;

		return true;
	}

	template<typename T>
	constexpr bool Intersection(const Quad2D<T>& quad, const Line2D<T>& line, Vec2<T>& p1, Vec2<T>& p2)
	{		
		std::array<Vec2<T>, 2> arr;
		uint i = 0;

		if (Intersection(Segment2D<T>(quad.points[0], quad.points[1]), line, arr[i])) ++i;
		if (Intersection(Segment2D<T>(quad.points[1], quad.points[2]), line, arr[i])) ++i;
		if (Intersection(Segment2D<T>(quad.points[2], quad.points[3]), line, arr[i])) ++i;
		if (Intersection(Segment2D<T>(quad.points[3], quad.points[0]), line, arr[i])) ++i;
		
		if (i == 0 || i > 2)
			return false;

		if (i == 1)
		{
			p1 = arr[0];
			p2 = arr[0];			
		}
		else
		{
			p1 = arr[0];
			p2 = arr[1];
		}

		return true;
	}

	template<typename T>
	constexpr bool Intersection(const Quad2D<T>& quad, const Ray2D<T>& ray, std::array<Vec2<T>, 2>& points, uint& count)
	{
		std::array<Vec2<T>, 2> arr;
		uint i = 0;
		
		if (Intersection(Segment2D<T>(quad.points[0], quad.points[1]), ray, arr[i])) ++i;
		if (Intersection(Segment2D<T>(quad.points[1], quad.points[2]), ray, arr[i])) ++i;
		if (Intersection(Segment2D<T>(quad.points[2], quad.points[3]), ray, arr[i])) ++i;
		if (Intersection(Segment2D<T>(quad.points[3], quad.points[0]), ray, arr[i])) ++i;

		if (i == 0 || i > 2)
			return false;

		points = arr;
		count = i;		
		return true;
	}

	template<typename T>
	constexpr bool Intersection(const Quad2D<T>& quad, const Segment2D<T>& segment, std::array<Vec2<T>, 2>& points, uint& count)
	{
		std::array<Vec2<T>, 2> arr;
		uint i = 0;

		if (Intersection(segment, Segment2D<T>(quad.points[0], quad.points[1]), arr[i])) ++i;
		if (Intersection(segment, Segment2D<T>(quad.points[1], quad.points[2]), arr[i])) ++i;
		if (Intersection(segment, Segment2D<T>(quad.points[2], quad.points[3]), arr[i])) ++i;
		if (Intersection(segment, Segment2D<T>(quad.points[3], quad.points[0]), arr[i])) ++i;

		if (i == 0 || i > 2)
			return false;

		points = arr;
		count = i;
		return true;
	}

	using Line2Df = Line2D<float>;

	using Ray2Df = Ray2D<float>;

	using Segment2Df = Segment2D<float>;

	template<typename uint VertexCount>
	using Polygon2Df = Polygon2D<float, VertexCount>;

	using Quad2Df = Quad2D<float>;
}