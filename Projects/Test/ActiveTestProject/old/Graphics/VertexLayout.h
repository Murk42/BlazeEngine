#pragma once

namespace Experimental::Graphics
{
	enum class VertexAttributeType
	{
		Float,
		Vec2f,
		Vec3f,
		Vec4f,
	};

	struct VertexAttributeLayout
	{				
		VertexAttributeType type;		
		uint byteOffset;
		uint location;
		uint bufferIndex;
	};

	struct VertexLayout
	{
		Array<VertexAttributeLayout> attributes;
	};

	enum class VertexInputRate
	{
		PerVertex,
		PerInstance
	};

	struct VertexBinding
	{
		uint bufferIndex;
		uint stride;
		VertexInputRate rate;
	};
}