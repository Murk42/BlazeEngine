#pragma once

namespace Blaze
{
	class HorizontalAlign
	{
	public:
		enum HorizontalAlignType : uint8
		{			
			Right,			
			Left,			
			Center
		};

		HorizontalAlign() = default;
		constexpr HorizontalAlign(HorizontalAlignType value) : value(value) {}

		constexpr operator HorizontalAlignType() const { return value; }
	private:
		HorizontalAlignType value = Center;
	};

	class VerticalAlign
	{
	public:
		enum VerticalAlignType : uint8
		{
			Top,
			Bottom,
			Center
		};

		VerticalAlign() = default;
		constexpr VerticalAlign(VerticalAlignType value) : value(value) {}

		constexpr operator VerticalAlignType() const { return value; }
	private:
		VerticalAlignType value = Center;
	};

	class Align
	{
	public:
		enum AlignType : uint8
		{
			Top,
			TopRight,
			Right,
			BottomRight,
			Bottom,
			BottomLeft,
			Left,
			TopLeft,
			Center
		};

		Align() = default;
		constexpr Align(AlignType value) : value(value) {}

		constexpr operator AlignType() const { return value; }
	private:
		AlignType value = Center;
	};	
}